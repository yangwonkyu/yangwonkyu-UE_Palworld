// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/PFSMComponent.h"
#include "Justin/PAction.h"
#include "Justin/PPalCharacter.h"

UPFSMComponent::UPFSMComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bIsAttacking = false;
	bIsSteppingBack = false;
}

void UPFSMComponent::BeginPlay()
{
	Super::BeginPlay();
	for (auto what : MapActionClasses)
	{
		UPAction* Action = NewObject<UPAction>(GetOwner(), what.Value);
		if (Action)
		{
			Action->Initialize(Cast<APPalCharacter>(GetOwner()));
			MapActions.Add(what.Key, Action);
		}
	}
	if (StartingState != EPalState::NONE)
	{
		ExecuteAction(StartingState);
	}
}

void UPFSMComponent::Enable()
{
	SetComponentTickEnabled(true);
}

void UPFSMComponent::Disable()
{
	ExecuteAction(EPalState::IDLE);
	SetComponentTickEnabled(false);
}

void UPFSMComponent::ExecuteAction(EPalState NewState)
{
	UPAction** NextAction = MapActions.Find(NewState);
	if (NextAction && *NextAction)
	{
		if (CurrentAction) CurrentAction->Stop();
		(*NextAction)->Start();
		CurrentAction = *NextAction;
	}
}	

void UPFSMComponent::SetCurrentState(EPalState _CurrentState)
{
	CurrentState = _CurrentState;
}

EPalState UPFSMComponent::GetCurrentState() const
{
	return CurrentState;
}

void UPFSMComponent::UpdateAction(EPalState StateToUpdate)
{
	if (StateToUpdate == EPalState::MOVEMENT)
	{
		if (CurrentAction) CurrentAction->UpdateAction();
	}
}

bool UPFSMComponent::GetStepBack() const
{
	return bIsSteppingBack;
}

void UPFSMComponent::SetStepBack(bool _bIsSteppingBack)
{
	bIsSteppingBack = _bIsSteppingBack;
}

void UPFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("CurrentState: %s"),
		*UEnum::GetValueAsString(CurrentState)));

	if (CurrentAction)
	{
		EPalState NewState = CurrentAction->Update(DeltaTime);
		if (NewState != EPalState::NONE)
		{
			ExecuteAction(NewState);
		}
	}
}