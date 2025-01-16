// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class PAL_WORLD_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	class APCharacter* Player;

	// Gun Mesh
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class USkeletalMeshComponent* GunMeshComp;


	void RotateTarget(FVector LookAtTarget);


	FTimerHandle FireRateTimerHandle;
	float FireRate = 2.5f;
	void CheckFireCondition();
	void Fire();

	float AttackDistance = 800.f;


	// 체력 관리 ========================================================================
	// 현재 체력
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float Hp;
	// 초기 체력 값
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHp = 4000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float Damage = 100.f;
	// 피격 당했을 때 처리
	UFUNCTION(BlueprintCallable, Category = "Health")
	void OnEnemyTakeDamage(float InDamage);
	UFUNCTION(BlueprintCallable, Category = "Health")
	void OnDie();
	bool bEnemyDead = false;


	// 발사 타이머 ======================================================================
	UFUNCTION()
	void SetFireTimer();

	UFUNCTION()
	void ClearFireTimer();




	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class APlayerBullet> BulletFatory;



	// 총알 자국 VFX
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class UParticleSystem* BulletImpactVFXFactory;


};
