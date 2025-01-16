// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/Actions/MovementAction.h"
#include "Movement_Attack.generated.h"

/**
 * 
 */



UCLASS()
class PAL_WORLD_API UMovement_Attack : public UMovementAction
{
	GENERATED_BODY()
public:
	virtual void Start() override;
	virtual EPalState Update(float DeltaTime) override;
	virtual void Stop() override;
protected:
	virtual void OnMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result) override;
};
