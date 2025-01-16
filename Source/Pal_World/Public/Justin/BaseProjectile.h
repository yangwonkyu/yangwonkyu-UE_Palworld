// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class USphereComponent;

UCLASS()
class PAL_WORLD_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseProjectile();
	void Activate();
	void Fire();
	void Despawn();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
protected:	

	UPROPERTY(EditDefaultsOnly, Category="Settings|Components", BlueprintReadOnly)
	TObjectPtr<USphereComponent> SphereComp;

	UFUNCTION(BlueprintNativeEvent)
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnPalRetrieved();
};
