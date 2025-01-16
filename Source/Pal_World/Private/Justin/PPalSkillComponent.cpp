// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/PPalSkillComponent.h"
#include "Justin/PalSkillObject.h"
#include "Justin/PPalCharacter.h"

UPPalSkillComponent::UPPalSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPPalSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	for (TSubclassOf<UPalSkillObject> SkillClass : SkillClasses)
	{
		auto SkillObj = NewObject<UPalSkillObject>(GetOwner(), SkillClass);
		if (SkillObj)
		{
			SkillObj->Initialize(GetOwner<APPalCharacter>());
			Skills.Add(SkillObj);
		}
	}
	Skills.Sort();

	for (int i = Skills.Num() - 1; i >= 0; --i)
	{
		AvailableSkill.Enqueue(FAvailableSkill(i, Skills[i]));
	}

	TotalTimes.SetNum(Skills.Num(), false);
	for (auto& Total : TotalTimes) Total = 0.f;

	InCooldown.SetNum(Skills.Num());
	for (auto& Cooldown : InCooldown) Cooldown = false;
}

void UPPalSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (int i = 0; i < TotalTimes.Num(); ++i)
	{
		if (InCooldown[i])
		{
			TotalTimes[i] += DeltaTime;
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("[%s] Time: %f"),
				*GetNameSafe(Skills[i]), TotalTimes[i]));
			if (TotalTimes[i] >= Skills[i]->GetCooldownTime() && InCooldown[i])
			{
				TotalTimes[i] = 0.f;
				InCooldown[i] = false;
				AvailableSkill.Enqueue(FAvailableSkill(i, Skills[i]));
			}
		}
	}
	const FAvailableSkill* ASkill= AvailableSkill.Peek();
	if(ASkill)
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(TEXT("Skills: %s"), *GetNameSafe(ASkill->Skill)));
	else GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(TEXT("Empty")));
}

void UPPalSkillComponent::Enable()
{
	SetComponentTickEnabled(true);
	const FAvailableSkill* ASkill = AvailableSkill.Peek();
	if (ASkill && ASkill->Skill)
		ASkill->Skill->Enable();
}

void UPPalSkillComponent::Disable()
{
	SetComponentTickEnabled(false);
	const FAvailableSkill* ASkill = AvailableSkill.Peek();
	if (ASkill && ASkill->Skill)
		ASkill->Skill->Disable();
}

bool UPPalSkillComponent::IsSkillAvailable()
{
	return !AvailableSkill.IsEmpty();
}

void UPPalSkillComponent::ActivateSkill(UObject* Obj, FName FuncName)
{
	if (IsSkillAvailable())
	{
		FAvailableSkill* ASkill = AvailableSkill.Peek();
		if (ASkill)
		{
			UPalSkillObject* SkillObj = ASkill->Skill;
			if (SkillObj)
			{
				SkillObj->Activate(Obj, FuncName);
				
			}
		}
	}
}

void UPPalSkillComponent::PrepareAttack()
{
	if (GetAvailableSkill())
		GetAvailableSkill()->PrepareAttack();
}

void UPPalSkillComponent::StartAttack()
{
	if (GetAvailableSkill())
		GetAvailableSkill()->BeginAttack();
}

void UPPalSkillComponent::OnSkillDeactivated()
{
	AvailableSkill.Pop();
}

void UPPalSkillComponent::OnAttackExecuted(const UPalSkillObject* ActiveSkill)
{
	StartCooldown(ActiveSkill);
}

void UPPalSkillComponent::StartCooldown(const UPalSkillObject* ActiveSkill)
{
	FAvailableSkill* ASkill = AvailableSkill.Peek();
	if (ASkill && ASkill->Skill == ActiveSkill)
	{
		InCooldown[ASkill->Index] = true;
	}
}

UPalSkillObject* UPPalSkillComponent::GetAvailableSkill() const
{
	const FAvailableSkill* ASkill = AvailableSkill.Peek();

	if (ensureAlways(ASkill && ASkill->Skill)) return ASkill->Skill;
	else return nullptr;
}

float UPPalSkillComponent::GetSkillDistance() const
{
	const FAvailableSkill* ASkill = AvailableSkill.Peek();
	if (ASkill && ASkill->Skill)
		return ASkill->Skill->GetSkillDistance();
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR!"));
		return -1.f;
	}
}

ESkillRange UPPalSkillComponent::GetSkillRangeType() const
{
	const FAvailableSkill* ASkill = AvailableSkill.Peek();
	if (ASkill && ASkill->Skill)
		return ASkill->Skill->GetSkillRangeType();

	return ESkillRange();
}

float UPPalSkillComponent::GetStepBackTriggerDistance() const
{
	if (GetAvailableSkill())
		return GetAvailableSkill()->GetStepBackTriggerDistance();
	return false;
}

bool UPPalSkillComponent::MustStepBack() const
{
	const FAvailableSkill* ASkill = AvailableSkill.Peek();
	if (ensureAlways(ASkill && ASkill->Skill))
		return ASkill->Skill->MustStepBack();
	
	return false;
}

int32 UPPalSkillComponent::GetStepBackAmount() const
{
	const FAvailableSkill* ASkill = AvailableSkill.Peek();
	if (ensureAlways(ASkill && ASkill->Skill))
		return ASkill->Skill->GetStepBackAmount();

	return -1;
}

float UPPalSkillComponent::GetStepBackDistance() const
{
	if (GetAvailableSkill())
	{
		return GetAvailableSkill()->GetStepBackDistance();
	}
	return -1.f;
}
float UPPalSkillComponent::GetMinStepBackNeeded() const
{
	if (GetAvailableSkill())
	{
		return GetAvailableSkill()->GetMinStepBackNeeded();
	}
	return -1.f;
}

bool UPPalSkillComponent::IsWithinTarget(AActor* Target) const
{
	if (Target)
	{
		FVector ToTargetVec = Target->GetActorLocation() - GetOwner()->GetActorLocation();

		return ToTargetVec.SquaredLength() <= (GetSkillDistance() * GetSkillDistance()) + 10.f;
	}
	return false;
}

//bool UPPalSkillComponent::IsInProperRange(AActor* Target, float Offset) const
//{
//	//Near skill vs Far Skill
//
//	switch(GetSkillRangeType())
//	{
//	case ESkillRange::NEAR:
//	{
//		return IsWithinTarget(Target);
//	}
//	case ESkillRange::FAR:
//	{
//		return !IsWithinTarget(Target);
//	}
//	}
//
//	return false;
//}
