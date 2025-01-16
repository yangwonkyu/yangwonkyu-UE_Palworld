// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PalAnimInstance.generated.h"

/**
 *
 */
class APPalCharacter;
class UPFSMComponent;
class UPPalSkillComponent;
class AAIController;
enum class EPalState : uint8;

UCLASS()
class PAL_WORLD_API UPalAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	EPalState PalState;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	FVector LookLoc;
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	bool HasTarget;
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	bool bIsInAir;

	UFUNCTION()
	void AnimNotify_BeginAttack();
	UFUNCTION()
	void AnimNotify_PrepareAttack();

	UFUNCTION()
	void AnimNotify_SpawnEffect();

private:
	UPROPERTY()
	TObjectPtr<APPalCharacter> MyOwner;
	TObjectPtr<UPFSMComponent> FSMComp;
	TObjectPtr<UPPalSkillComponent> SkillComp;
	TObjectPtr<AAIController> AI;

};

