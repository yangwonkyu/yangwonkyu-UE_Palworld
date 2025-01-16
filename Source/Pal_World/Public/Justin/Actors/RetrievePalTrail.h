// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RetrievePalTrail.generated.h"

class USplineComponent;
class UNiagaraComponent;
class USceneComponent;

UCLASS()
class PAL_WORLD_API ARetrievePalTrail : public AActor
{
	GENERATED_BODY()
	
public:	
	ARetrievePalTrail();
	UFUNCTION(BlueprintCallable)
	void Initialize(AActor* StartActor, AActor* EndActor);
	UFUNCTION(BlueprintCallable)
	void Update();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USplineComponent> SplineComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UNiagaraComponent> NiagaraComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USceneComponent> SceneComp;
public:	
	virtual void Tick(float DeltaTime) override;

private:
	TObjectPtr<AActor> Start;
	TObjectPtr<AActor> End;
};
