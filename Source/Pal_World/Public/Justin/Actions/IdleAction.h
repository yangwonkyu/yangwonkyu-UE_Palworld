// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/PAction.h"
#include "IdleAction.generated.h"

/**
 * 
 */
UCLASS()
class PAL_WORLD_API UIdleAction : public UPAction
{
	GENERATED_BODY()
	
public:
	UIdleAction();
	virtual void Start() override;
	virtual EPalState Update(float DeltaTime) override;
	virtual void Stop() override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float ToMovementDelayTime;
private:
	bool bHasTarget;
	FTimerHandle HasTargetHandle;
	UFUNCTION()
	void HasTargetElapsed();
};
