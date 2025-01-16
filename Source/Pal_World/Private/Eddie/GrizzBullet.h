// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrizzBullet.generated.h"

UCLASS()
class AGrizzBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrizzBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �浹ü �ܰ��� ����� �ʹ�.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USphereComponent* SphereComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* MeshComp;

	// �߻�ü ������Ʈ�� ����� �ʹ�.
	UPROPERTY(EditDefaultsOnly)
	class UProjectileMovementComponent* MovementComp;

	// �߻� �ӵ�
	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed = 3000.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* VFXProjectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* VFXMuzzle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* VFXHit;

private:
	void Die();

	FTimerHandle DestroyTimerHandle; // �ı� Ÿ�̸� �ڵ�
};
