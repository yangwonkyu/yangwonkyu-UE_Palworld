// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/Components/PStatusComponent.h"
#include "PalStatusComp.generated.h"

/**
 *
 */

class APPalCharacter;
class UImage;
class UTexture2D;

UCLASS()
class PAL_WORLD_API UPalStatusComp : public UPStatusComponent
{
	GENERATED_BODY()
public:
	UPalStatusComp();

	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void DisplayWidget(bool bDisplay);

	virtual void TakeDamage(AActor* Initiator, FVector ImpactLocation, float Damage) override;

	void SetInvincibility(bool bEnable);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<UTexture2D> PalUIImage;
private:


	APPalCharacter* Pal;
	bool bInvincible;
};
