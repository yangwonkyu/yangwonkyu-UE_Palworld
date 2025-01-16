// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/PalSkillObject.h"
#include "ProjectileSkillObject.generated.h"

/**
 * 
 */
UCLASS()
class PAL_WORLD_API UProjectileSkillObject : public UPalSkillObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Projectile")
	TSubclassOf<ABaseProjectile> ProjectileClass;

	//How long skill lasts
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Projectile")
	FVector SpawnOffset;
};
	