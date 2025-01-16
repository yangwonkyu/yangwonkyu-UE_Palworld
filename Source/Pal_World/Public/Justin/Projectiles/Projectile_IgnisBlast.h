// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/BaseProjectile.h"
#include "Projectile_IgnisBlast.generated.h"

/**
 * 
 */
UCLASS()
class PAL_WORLD_API AProjectile_IgnisBlast : public ABaseProjectile
{
	GENERATED_BODY()
	
public:
	AProjectile_IgnisBlast();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	
	virtual void OnOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float TravelSpeed;
};
