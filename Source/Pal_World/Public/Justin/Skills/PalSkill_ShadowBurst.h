// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/PalSkillObject.h"
#include "PalSkill_ShadowBurst.generated.h"

/**
 * 
 */

class UNiagaraSystem;

UCLASS()
class PAL_WORLD_API UPalSkill_ShadowBurst : public UPalSkillObject
{
	GENERATED_BODY()
public:
	UPalSkill_ShadowBurst();
	virtual bool Attack() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings|ShadowBurst")
	float AttackRadius;
};
