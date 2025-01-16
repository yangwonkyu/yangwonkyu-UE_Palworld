// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/PalSkillObject.h"
#include "PalSkill_ForcefulCharge.generated.h"

/**
 * 
 */

class UParticleSystemComponent;
class UNiagaraComponent;

UCLASS()
class PAL_WORLD_API UPalSkill_ForcefulCharge : public UPalSkillObject, public FTickableGameObject
{
	GENERATED_BODY()
public:
	UPalSkill_ForcefulCharge();
protected:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsAllowedToTick() const override;
	
	virtual void Activate(UObject* Obj, FName FuncName) override;
	virtual bool Prepare() override;
	virtual bool Attack() override;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|ForcefulCharge|Charge")
	float AttackOffsetDist;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|ForcefulCharge|Charge")
	float AttackRadius;	
	UPROPERTY(EditDefaultsOnly, Category = "Settings|ForcefulCharge|Charge")
	float Velocity;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|ForcefulCharge|Charge")
	float DistanceTraveled;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|ForcefulCharge|Charge")
	float StartAttackDistance;


	//Extra time to move forward during attack animation.
	/*UPROPERTY(EditDefaultsOnly, Category = "Settings|ForcefulCharge")
	float AttackMoveDuration;*/

	UPROPERTY(EditDefaultsOnly, Category = "Settings|ForcefulCharge|Slide")
	float MaxSlideTime;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|ForcefulCharge|Slide")
	float SlideDist;	
	UPROPERTY(EditDefaultsOnly, Category = "Settings|ForcefulCharge|Slide")
	float AllowedSlideDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Sound")
	TObjectPtr<USoundBase> PrepareSound;

	UPROPERTY(EditDefaultsOnly, Category="Settings|Effects|ForcefulCharge")
	TObjectPtr<UParticleSystem> PrepareParticleSystem;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Effects|ForcefulCharge")
	float PrepareScale;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Effects|ForcefulCharge")
	TObjectPtr<UParticleSystem> AttackParticleSystem;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Effects|ForcefulCharge")
	float AttackScale;

	void SpawnAttackEffect(FVector Location);
private:

	TObjectPtr<UParticleSystemComponent> PrepareSystemComp;
	TObjectPtr<UParticleSystemComponent> AttackSystemComp;


	TObjectPtr<UNiagaraComponent> AttackNiagaraSystemComp;
	TObjectPtr<UNiagaraComponent> PrepareNiagaraSystemComp;
	TObjectPtr<UAudioComponent> AudioComp;
	TObjectPtr<UPPalSkillComponent> SkillComp;

	float TotalMoveTime;
	
	//Sliding back
	bool bSlidingBack;
	FVector EndSlide;
	FVector StartSlide;

	// Charging
	bool bCharging;
	FVector EndLoc;
	FVector StartLoc;
	float MaxMoveTime;
	float TotalMoveDistance;

	bool bTicking;
	bool bHasAttacked;
	bool bMontageJumped;
	bool bSlowingDown;
	float EaseOutSine(float x)
	{
		return FMath::Sin((x * PI) / 2);
	}

	float EaseOutQuart(float x){
		return 1 - FMath::Pow(1 - x, 4);
	}

	float EaseOutCubic(float x){
		return 1 - FMath::Pow(1 - x, 3);
	}
};
