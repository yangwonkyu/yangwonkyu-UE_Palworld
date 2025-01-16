// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Actions/Movement_Attack.h"

void UMovement_Attack::Start()
{
	Super::Start();
}

EPalState UMovement_Attack::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	return EPalState::NONE;
}

void UMovement_Attack::Stop()
{
	Super::Stop();
}

void UMovement_Attack::OnMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	Super::OnMoveComplete(RequestID, Result);
}
