// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PalSphereActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UPalSlotUI;
class UNiagaraComponent;
UCLASS()
class PAL_WORLD_API APalSphereActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APalSphereActor();

	void Initialize(UPalSlotUI* _PalSlotUI);
	void BeginThrow();

protected:
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<USphereComponent> SphereComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<UStaticMeshComponent> SMComp;

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float FlightSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<UNiagaraComponent> NiagaraComp_Trail;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<USoundBase> SummonSound;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPalSlotUI* PalSlotUI;
	float Time;
	FVector DirectionVec;
	FVector Start;
};
