// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/PAction.h"
#include "AttackAction.generated.h"

/**
 * 
 */
UCLASS()
class PAL_WORLD_API UAttackAction : public UPAction
{
	GENERATED_BODY()
public:
	UAttackAction();
	virtual void Start() override;
	virtual EPalState Update(float DeltaTime) override;
	virtual void Stop() override;

	

protected:
	UFUNCTION()
	void OnFacingTarget();
	UFUNCTION()
	void StopAttack();
	void StartAttack();

private:
	bool bIsFacingTarget;
	bool bIsAttacking;
	bool bAlreadyAttacked;
};
