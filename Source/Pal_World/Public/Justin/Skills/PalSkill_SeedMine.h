// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/ProjectileSkillObject.h"
#include "PalSkill_SeedMine.generated.h"

/**
 * 
 */
UCLASS()
class PAL_WORLD_API UPalSkill_SeedMine : public UProjectileSkillObject
{
	GENERATED_BODY()
protected:
	UPalSkill_SeedMine();
	virtual bool Attack() override;
};
