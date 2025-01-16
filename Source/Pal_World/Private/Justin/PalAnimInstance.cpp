// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/PalAnimInstance.h"
#include "Justin/PPalCharacter.h"
#include "Justin/PFSMComponent.h"
#include "Justin/PPalSkillComponent.h"
#include "AIController.h"
#include "Justin/Pals/PalCharacter_Anubis.h"

void UPalAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	HasTarget = false;
	PalState = EPalState::NONE;
}

void UPalAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	MyOwner = Cast<APPalCharacter>(TryGetPawnOwner());
	if (ensureAlways(MyOwner))
	{
		FSMComp = MyOwner->GetComponentByClass<UPFSMComponent>();
		SkillComp = MyOwner->GetComponentByClass<UPPalSkillComponent>();
		AI = MyOwner->GetOwner<AAIController>();
	}
	else UE_LOG(LogTemp, Warning, TEXT("NO OWNER in animation instance!"));
}

void UPalAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (FSMComp)
	{
		PalState = FSMComp->GetCurrentState();
	}

	if (MyOwner)
	{
		LookLoc = MyOwner->GetTargetLocation();
		HasTarget = MyOwner->HasTarget();
		//UE_LOG(LogTemp, Warning, TEXT("Has target? %s"), HasTarget ? TEXT("YES") : TEXT("NO"));
		bIsInAir = MyOwner->GetIsInAir();
	}	
}

void UPalAnimInstance::AnimNotify_BeginAttack()
{
	if (SkillComp)
	{
		SkillComp->StartAttack();
	}
}

void UPalAnimInstance::AnimNotify_PrepareAttack()
{
	if (SkillComp)
	{
		SkillComp->PrepareAttack();
	}
}

void UPalAnimInstance::AnimNotify_SpawnEffect()
{
	if (MyOwner)
	{
		auto Anubis = Cast<APalCharacter_Anubis>(MyOwner);
		if (Anubis)
		{
			Anubis->SpawnForcefulChargeEffect();
		}
	}
}