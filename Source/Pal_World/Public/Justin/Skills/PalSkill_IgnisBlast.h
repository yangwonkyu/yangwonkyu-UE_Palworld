// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/ProjectileSkillObject.h"
#include "PalSkill_IgnisBlast.generated.h"

/**
 * 
 */
UCLASS()
class PAL_WORLD_API UPalSkill_IgnisBlast : public UProjectileSkillObject
{
	GENERATED_BODY()
public:
	UPalSkill_IgnisBlast();
	virtual bool Attack() override;
};
