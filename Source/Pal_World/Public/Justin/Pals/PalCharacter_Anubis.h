// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/PPalCharacter.h"
#include "PalCharacter_Anubis.generated.h"

/**
 * 
 */

class UAnubisSkill_SideStep;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class PAL_WORLD_API APalCharacter_Anubis : public APPalCharacter
{
	GENERATED_BODY()
public:
	APalCharacter_Anubis();
	void SpawnForcefulChargeEffect();
	void DeactivateForcefulChargeEffect();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TSubclassOf<UAnubisSkill_SideStep> AnubisSkillClass;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Effects")
	TObjectPtr<UArrowComponent> ArrowComp_SkillEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Effects")
	TObjectPtr<UNiagaraComponent> NiagaraComp_SkillEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Effects")
	TObjectPtr<UNiagaraComponent> NiagaraComp_DashEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Effects")
	TObjectPtr<UNiagaraSystem> NS_ForcefulCharge;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Effects")
	float EffectScale;

private:
	virtual void OnTakenDamage(float Health, float MaxHealth) override;
	void OnSideStepCompleted();
	
	UPROPERTY()
	TObjectPtr<UAnubisSkill_SideStep> AnubisSkill;

	void SpawnEffect();
	//FTimerHandle EffectHandle; 
};
