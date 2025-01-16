// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PBullet.generated.h"

UCLASS()
class PAL_WORLD_API APBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APBullet();

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


	// 발사체 방향 설정 함수
	void FireInDirection(const FVector& ShootDirection);

	// 총알 제거 함수
	void Die();
};
