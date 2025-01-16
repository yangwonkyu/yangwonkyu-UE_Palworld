// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AnubisSkill_SideStep.generated.h"

/**
 * 
 */

DECLARE_DELEGATE(FOnSideStepFinishedSignature);

UCLASS(Blueprintable)
class PAL_WORLD_API UAnubisSkill_SideStep : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
public:
	UAnubisSkill_SideStep();

	void StartInit(AActor* _Owner);
	bool StartSideStep(AActor* Target);
	FOnSideStepFinishedSignature OnSideStepFinished;

protected:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsAllowedToTick() const override;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float MaxTime;
	UPROPERTY(EditAnywhere, Category = "Settings")
	float SideStepDistance;
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = "0.0"), meta = (ClampMax = "1.0"))
	float Probability;
private:
	FVector Start;
	FVector End;
	float CurrentTime;
	bool bSideStepping;
	TObjectPtr<AActor> Owner;
};
