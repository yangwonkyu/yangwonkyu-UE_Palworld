// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/PalSkillObject.h"
#include "PalSkill_GroundSmash.generated.h"

/**
 * 
 */

UENUM()
enum class EGroundSmashState
{
	ONGROUND,
	TOAIR, 
	INAIR,
	FROMAIR
};
class UParticleSystem;

UCLASS()
class PAL_WORLD_API UPalSkill_GroundSmash : public UPalSkillObject , public FTickableGameObject
{
	GENERATED_BODY()
public:
	UPalSkill_GroundSmash();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsAllowedToTick() const override;
	virtual bool Prepare() override;
	virtual bool Attack() override;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|GroundSmash")
	float AttackRadius;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|GroundSmash")
	float ToAirTravelTime;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|GroundSmash")
	float FromAirTravelTime;
private:
	EGroundSmashState State;

	float HeightZ;

	FVector From; 
	FVector To;
	float MaxTravelTime;
	float TotalTravelTime;
	float Gravity;
};
