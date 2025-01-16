// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/PAction.h"
#include "Navigation/PathFollowingComponent.h"
#include "SetupAttackAction.generated.h"

/**
 *
 */

UCLASS()
class PAL_WORLD_API USetupAttackAction : public UPAction
{
	GENERATED_BODY()
public:
	USetupAttackAction();

	virtual void Start() override;
	virtual EPalState Update(float DeltaTime) override;
	virtual void Stop() override;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float RunSpeed;

	//How far off should pal move away from a detected obstacle
	UPROPERTY(EditDefaultsOnly, Category = "Settings|HasObstacle")
	float ClearOffset;
	//how much to increase ClearOffset by when AI Movement is invalid
	UPROPERTY(EditDefaultsOnly, Category = "Settings|HasObstacle")
	float ClearOffsetRate;
	//Obstacle detection sphere radius
	UPROPERTY(EditDefaultsOnly, Category = "Settings|HasObstacle")
	float Radius;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|StepBack")
	float StepBackSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|StepBack")
	float StepBackOffsetFromObstacle;

	//UPROPERTY(EditDefaultsOnly, Category = "Settings|StepBack")
	//float MaxStepBackDistance;
	//How far off from obstacle should this pal step back when there is an obstacle blocking the way of step back movement?

	//UPROPERTY(EditDefaultsOnly, Category = "Settings|StepBack")
	//int32 MaxStepBackCount;


protected:
	void UpdateMovement();
	//Adjust if an obstacle is blocking the pal's target

	void Adjust();
	void Move();

	FVector FindStepBackLocation();
	bool StepBack(float DeltaTime);

	bool TargetInfront() const;
	bool CanStepBack() const;
	bool HasObstacles() const;
	bool TooCloseToTarget() const;

	UFUNCTION()
	virtual void OnMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result);

private:
	bool bIsClear;

	bool bHadObstacles;
	float StartingClearOffset;

	bool bSteppingBack;
	FVector StepBackDirection;
	FVector StepBackEnd;
	float TotalStepBackTime;
	int32 StepBackCount;
	int32 StepBackMaxCount;
	float TotalStepBackDistance;
};
