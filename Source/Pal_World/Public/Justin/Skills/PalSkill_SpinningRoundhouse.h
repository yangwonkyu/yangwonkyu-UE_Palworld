// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/PalSkillObject.h"
#include "PalSkill_SpinningRoundhouse.generated.h"

/**
 * 
 */

class UParticleSystem;

UCLASS()
class PAL_WORLD_API UPalSkill_SpinningRoundhouse : public UPalSkillObject
{
	GENERATED_BODY()
public:
	UPalSkill_SpinningRoundhouse();
	virtual bool Attack() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings|SpinningRoundhouse")
	float Radius;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|GroundSmash")
	TObjectPtr<UParticleSystem> Particle;
};
