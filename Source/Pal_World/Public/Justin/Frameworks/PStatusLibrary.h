// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PStatusLibrary.generated.h"

/**
 * 
 */

class ACharacter;

UCLASS()
class PAL_WORLD_API UPStatusLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void ApplyDamage(AActor* Initiator, AActor* Target, FVector ImpactLocation, float Damage);
};
