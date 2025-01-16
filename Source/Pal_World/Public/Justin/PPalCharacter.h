// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PPalCharacter.generated.h"

class UPFSMComponent;
class UPPalSkillComponent;
class UPalStatusComp;
class APCharacter;
class UPalInfo; 

UCLASS()
class PAL_WORLD_API APPalCharacter : public ACharacter
{
	GENERATED_BODY()

	friend class UPalInfo;

public:
	APPalCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Components")
	TObjectPtr<UPFSMComponent> FSMComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Components")
	TObjectPtr<UPPalSkillComponent> SkillComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Components")
	TObjectPtr<UPalStatusComp> StatusComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Components")
	TObjectPtr<USceneComponent> WidgetLocationComp;

public:
	UFUNCTION(BlueprintCallable)
	void Initialize(FVector SpawnLoc, AActor* _Player, UPalInfo* _Info, AActor* NewTarget = nullptr);

	const UPalInfo* GetPalInfo() const;

	void LookAtTarget(bool bEnabled);

	UFUNCTION(BlueprintCallable)
	void StartPalBounce(FVector Impulse, bool VelocityChange = true);
	UFUNCTION(BlueprintCallable)
	void MoveToLocation(float Speed, FVector GoalLocation);
	
	void SetIsInAir(bool _bIsInAir);
	bool GetIsInAir() const;
	
	float GetBaseHeight() const;
	
	AActor* GetTarget();
	bool HasTarget() const;
	FVector GetTargetLocation(AActor* RequestedBy = nullptr) const override;

	APCharacter* GetPlayer() const;

	//Move this back to Protected after testing
	void SetTarget(AActor* _Target);
protected:
	UFUNCTION()
	void OnBossDead();

	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (AllowPrivateAccess))
	TObjectPtr<UAnimMontage> DamagedMontage;

	virtual void OnTakenDamage(float Health, float MaxHealth);

private:
	UPROPERTY()
	TObjectPtr<APCharacter> Player;
	UPROPERTY()
	TObjectPtr<UPalInfo> Info;
	UPROPERTY(EditInstanceOnly, Category = "Settings", meta = (AllowPrivateAccess))
	TObjectPtr<AActor> Target;
	void DeclareDead();
	void Retrieve(FVector RetrieveLoc);
	void Summon(FVector SpawnLoc, AActor* _Target);
	bool bIsSummoning;
	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (AllowPrivateAccess))
	FVector StartSummonScale;
	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (AllowPrivateAccess))
	float MaxSummonTime;
	float TotalSummonTime;

	void StartIsInAir();
	bool bIsInAir;

	float BaseHeight;
};
