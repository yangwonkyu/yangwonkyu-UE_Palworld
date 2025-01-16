// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/PAction.h"
#include "Navigation/PathFollowingComponent.h"
#include "MovementAction.generated.h"

/**
 * 
 */

UENUM()
enum class EMovement : uint8
{
	FAR,
	NEAR
};

UCLASS()
class PAL_WORLD_API UMovementAction : public UPAction
{
	GENERATED_BODY()
public:
	UMovementAction();
	virtual void Start() override;
	virtual EPalState Update(float DeltaTime) override;
	virtual void Stop() override;

	virtual void UpdateAction() override;

protected:
	//Stops walking if Target is within value of WalkThreshold
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float WalkThreshold;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float WalkSpeed;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float StrafeAngle;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float StrafeDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float ToSetupStateDelayTime;

	UFUNCTION()
	virtual void OnMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result);

private:
	EMovement MovementType;

	bool bIsMoving;
	bool bCanMove;
	bool bIsSkillAvailable;
	FTimerHandle SkillHandle;
	UFUNCTION()
	void DelayElapsed();
	FVector TargetLocation;
	FVector Destination;
	float CurrStrafeDist;
	FVector DirVector;
	float NegateVal;
	void SetMoveVector(FVector& MoveVec);
	bool IsOutofSight() const;
	FVector GetNewDestination(const FVector& _MoveVector) const;


	FVector RotateVectorByAngle(const FVector& OriginalVector, float AngleDegrees);
};
