// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Actions/AttackAction.h"
#include "Justin/PPalCharacter.h"
#include "AIController.h"

UAttackAction::UAttackAction()
{
	bIsFacingTarget = false;
	bIsAttacking = false;
	bAlreadyAttacked = false;
}

void UAttackAction::Start()
{
	Super::Start();

	FTimerHandle Handle;

	auto AI = PalCharacter->GetController<AAIController>();
	if (AI)
	{
		if (AI->GetFocusActor())
		{
			bIsFacingTarget = true;			
		}	
		else
		{
			//Turn pal to face Target
			AI->SetFocus(PalCharacter->GetTarget());
			GetWorld()->GetTimerManager().SetTimer(Handle, this, &UAttackAction::OnFacingTarget, .2f, false);
		}
	}	
}

EPalState UAttackAction::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if (PalCharacter && !PalCharacter->HasTarget())
	{
		return EPalState::IDLE;
	}

	if (bIsFacingTarget && !bIsAttacking)
	{
		if (ensureAlways(SkillComp))
		{
			if (SkillComp->IsSkillAvailable())
			{
				if(bAlreadyAttacked) return EPalState::SETUPATTACK;
				
				StartAttack();
				/*if (SkillComp->IsWithinTarget(PalCharacter->GetTarget()))*/ 
			}
			else return EPalState::MOVEMENT;
		}
	}

	return EPalState::NONE;
}

void UAttackAction::Stop()
{
	Super::Stop();

	bIsFacingTarget = false;
	bIsAttacking = false;
	bAlreadyAttacked = false;

	auto AI = PalCharacter->GetController<AAIController>();
	if (AI)
	{
		//Turn pal to face Target
		AI->SetFocus(nullptr);
		if (AI->ReceiveMoveCompleted.IsBound())
			AI->ReceiveMoveCompleted.Clear();
	}
}

void UAttackAction::OnFacingTarget()
{
	bIsFacingTarget = true;
}

void UAttackAction::StopAttack()
{
	bIsAttacking = false;
	bAlreadyAttacked = true;
}

void UAttackAction::StartAttack()
{
	if (SkillComp)
	{
		bIsAttacking = true;
		UE_LOG(LogTemp, Warning, TEXT("STARTATTACK?"));
		SkillComp->ActivateSkill(this, "StopAttack");
	}
}
