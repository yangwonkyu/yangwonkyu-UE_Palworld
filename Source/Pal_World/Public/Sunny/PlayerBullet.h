// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerBullet.generated.h"

UCLASS()
class PAL_WORLD_API APlayerBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	// 충돌체
	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* SphereComp;

	// 외관
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* MeshComp;

	// 발사체 컴포넌트
	UPROPERTY(EditDefaultsOnly)
	class UProjectileMovementComponent* MovementComp;


	UPROPERTY(EditAnywhere)
	int32 Damage = 2;



	void Die();

};
