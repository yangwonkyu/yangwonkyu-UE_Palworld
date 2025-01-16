// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/PalSkillObject.h"
#include "Justin/PPalCharacter.h"
#include "Justin/PPalSkillComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

UPalSkillObject::UPalSkillObject()
{
	CooldownTime = 8.f;
	SkillDistance = 200.f;

	AttackDuration = 0.f;
	PrepareDuration = 0.f;
	IsAttacking = true;
	IsPreparing = true;
	bEnabled = true;

	bMustStepBack = false;
	bRandomizeStepBackMaxCount = false;
	StepBackMaxCount = 1;
	StepBackDistance = 700.f;
	MinStepBackNeeded = 500.f;
	StepBackTriggerDistance= 300.f;

	IgnoreCollisionDuration = .9f;
	bIgnoreCollision = false;

	ParticleSystemScale = 5.f;
	NiagaraSystemScale = 5.f;
}

void UPalSkillObject::Initialize(APPalCharacter* _MyOwner)
{
	if (_MyOwner)
	{
		MyOwner = _MyOwner;

		if (MyOwner)
		{
			UPPalSkillComponent* SkillComp = MyOwner->GetComponentByClass<UPPalSkillComponent>();
			if (SkillComp)
			{
				OnSkillDeactivated.AddUObject(SkillComp, &UPPalSkillComponent::OnSkillDeactivated);
				OnAttackExecuted.BindUObject(SkillComp, &UPPalSkillComponent::OnAttackExecuted);
			}

			if (MyOwner->GetMesh())
			{
				UAnimInstance* AnimInstance = MyOwner->GetMesh()->GetAnimInstance();
				if (AnimInstance)
				{
					MyAnimInstance = AnimInstance;
				}
			}
		}
	}
}

void UPalSkillObject::Enable()
{
	bEnabled = true;
}

void UPalSkillObject::Disable()
{
	if (MyAnimInstance->OnMontageEnded.IsBound())
		MyAnimInstance->OnMontageEnded.RemoveAll(this);
	MyAnimInstance->StopAllMontages(0.f);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	bEnabled = false;
}

UAnimMontage* UPalSkillObject::GetAttackMontage() const
{
	return AttackMontage;
}

float UPalSkillObject::GetCooldownTime() const
{
	return CooldownTime;
}

float UPalSkillObject::GetSkillDistance() const
{
	return SkillDistance;
}
float UPalSkillObject::GetMinStepBackNeeded() const
{
	return MinStepBackNeeded;
}

float UPalSkillObject::GetStepBackTriggerDistance() const
{
	return StepBackTriggerDistance;
}

bool UPalSkillObject::MustStepBack() const
{
	return bMustStepBack;
}
int32 UPalSkillObject::GetStepBackAmount() const
{
	if (bRandomizeStepBackMaxCount && StepBackMaxCount > 0) return FMath::RandRange(1, StepBackMaxCount);

	return StepBackMaxCount;
}
float UPalSkillObject::GetStepBackDistance() const
{
	return StepBackDistance;
}

void UPalSkillObject::Activate(UObject* Obj, FName FuncName)
{
	if (!bEnabled)
	{
		UE_LOG(LogTemp, Error, TEXT("Skill %s is disabled!"), *GetNameSafe(this));
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("Attack! %s"), *GetNameSafe(this));

	BoundObj = Obj;
	OnSkillDeactivated.AddUFunction(BoundObj, FuncName);

	if (MyAnimInstance && ensureAlways(!MyAnimInstance->OnMontageEnded.IsBound()))
	{
		MyAnimInstance->OnMontageEnded.AddDynamic(this, &UPalSkillObject::Deactivate);
		MyAnimInstance->Montage_Play(AttackMontage);
	}

	IsPreparing = true;
	IsAttacking = true;
}

void UPalSkillObject::PrepareAttack()
{
	Prepare();

	if (bIgnoreCollision)
	{
		MyOwner->GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		MyOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		FTimerHandle Handle; 
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &UPalSkillObject::ReEnableCollision, IgnoreCollisionDuration, false);
	}
	IsPreparing = false;
}

void UPalSkillObject::BeginAttack()	
{
	Attack();
	if (PrepareHandle.IsValid()) GetWorld()->GetTimerManager().ClearTimer(PrepareHandle);
	IsAttacking = false;
}

void UPalSkillObject::SpawnParticleEffect(FVector Location)
{	
	UParticleSystemComponent* System = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, MyOwner->GetActorLocation());
	if (System)
	{
		System->SetWorldScale3D(FVector(ParticleSystemScale));
		FVector Loc = System->GetComponentLocation();
		Loc.Z += ParticleHeightOffset;
		System->SetWorldLocation(Loc);
	}
}
void UPalSkillObject::SpawnNiagaraEffect(FVector Location)
{
	UNiagaraComponent* Niagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, Location);
	if (Niagara)
	{
		Niagara->SetWorldScale3D(FVector(NiagaraSystemScale));
		FVector Loc = Niagara->GetComponentLocation();
		Loc.Z += NiagaraHeightOffset;
		Niagara->SetWorldLocation(Loc);
	}
}

void UPalSkillObject::ReEnableCollision()
{
	MyOwner->GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	MyOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}

bool UPalSkillObject::IsActive() const
{
	if (ensure(MyAnimInstance))
	{
		return MyAnimInstance->Montage_IsActive(AttackMontage);
	}
	return false;
}

bool UPalSkillObject::Prepare()
{
	if (IsPreparing)
	{
		if (PrepareDuration != 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer(PrepareHandle, this, &UPalSkillObject::StopPreparing, PrepareDuration, false);
		}
		else StopPreparing();
	}
	return IsPreparing;
}

bool UPalSkillObject::Attack()
{
	if (IsAttacking)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SkillSound, MyOwner->GetActorLocation());

		if (AttackDuration != 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer(AttackHandle, this, &UPalSkillObject::StopAttack, AttackDuration, false);
		}
		else StopAttack();
		OnAttackExecuted.ExecuteIfBound(this);
	}	
	return IsAttacking;
}

void UPalSkillObject::StopPreparing()
{
	if (MyAnimInstance)
	{
		MyAnimInstance->Montage_JumpToSection("2", AttackMontage);
	}
}

void UPalSkillObject::StopAttack()
{
	if (MyAnimInstance)
	{
		MyAnimInstance->Montage_JumpToSection("4", AttackMontage);
	}
}
float UPalSkillObject::GetPrepareDuration()
{
	return PrepareDuration;
}
void UPalSkillObject::Deactivate(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != AttackMontage) return;
	
	if (!bInterrupted)
	{
		if (MyAnimInstance->OnMontageEnded.IsBound())
		{
			MyAnimInstance->OnMontageEnded.RemoveAll(this);
		}

		OnSkillDeactivated.Broadcast();
		OnSkillDeactivated.RemoveAll(BoundObj);
		BoundObj = nullptr;

		IsPreparing = true;
		IsAttacking = true;
	}
	else UE_LOG(LogTemp, Warning, TEXT("How was this interrupted? %s"), *GetNameSafe(this));
}

bool UPalSkillObject::operator<(const UPalSkillObject& Other) const
{
	return GetCooldownTime() < Other.GetCooldownTime();
}

UWorld* UPalSkillObject::GetWorld() const
{
	auto Actor = Cast<AActor>(GetOuter());
	if (Actor)
	{
		return Actor->GetWorld();
	}
	else return nullptr;
}

ESkillRange UPalSkillObject::GetSkillRangeType() const
{
	return SkillRangeType;
}