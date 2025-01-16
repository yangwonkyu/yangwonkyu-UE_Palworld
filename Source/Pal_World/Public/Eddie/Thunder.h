// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Eddie/BaseAttack.h"
#include "Components/BoxComponent.h"
#include "Thunder.generated.h"

UCLASS()
class PAL_WORLD_API AThunder : public ABaseAttack
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThunder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComp;

	float Speed = 1500;

	// 번개 시간 지나면 없어지게 하는 함수
	void DestroyAfterDelay();

private:
	FTimerHandle UnusedHandle;
};

