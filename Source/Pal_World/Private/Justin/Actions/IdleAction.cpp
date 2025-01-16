// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Actions/IdleAction.h"
#include "Justin/PPalCharacter.h"

UIdleAction::UIdleAction()
{
	bHasTarget = false;
	ToMovementDelayTime = .15f;
}

void UIdleAction::Start()
{
	Super::Start();
}

EPalState UIdleAction::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if (PalCharacter && PalCharacter->HasTarget())
	{
		if (!bHasTarget && !HasTargetHandle.IsValid())
		{
			GetWorld()->GetTimerManager().SetTimer(HasTargetHandle, this, &UIdleAction::HasTargetElapsed, ToMovementDelayTime, false);
		}

		if (bHasTarget)
		{
			return EPalState::MOVEMENT;
		}
	}

	return EPalState::NONE;
}

void UIdleAction::Stop()
{
	Super::Stop();
	bHasTarget = false;
	HasTargetHandle.Invalidate();
}

void UIdleAction::HasTargetElapsed()
{
	bHasTarget = true;
}
