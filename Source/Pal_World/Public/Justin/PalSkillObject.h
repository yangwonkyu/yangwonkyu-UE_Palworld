// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PalSkillObject.generated.h"

/**
 *
 */

UENUM()
enum class ESkillRange
{
	NEAR,
	FAR
};

class ABaseProjectile;
class APPalCharacter;
class USoundBase;
class UNiagaraSystem;

DECLARE_MULTICAST_DELEGATE(FSkillDeactivatedDelegate);
DECLARE_DELEGATE_OneParam(FAttackExecutedDelegate, const UPalSkillObject* /*PalSkill*/);

UCLASS(Blueprintable)
class PAL_WORLD_API UPalSkillObject : public UObject
{
	GENERATED_BODY()
	friend class UPPalSkillComponent;

public:
	UPalSkillObject();

	void Initialize(APPalCharacter* _MyOwner);

	void Enable();
	void Disable();

	float GetCooldownTime() const;
	float GetSkillDistance() const;

	virtual void Activate(UObject* Obj, FName FuncName);
	virtual void PrepareAttack();
	virtual void BeginAttack();

	FSkillDeactivatedDelegate OnSkillDeactivated;
	FAttackExecutedDelegate OnAttackExecuted;

	bool operator<(const UPalSkillObject& Other) const;

	ESkillRange GetSkillRangeType() const;
	bool MustStepBack() const;
	int32 GetStepBackAmount() const;
	float GetStepBackDistance() const;
	float GetMinStepBackNeeded() const;
	float GetStepBackTriggerDistance() const; 

protected:
	void SpawnNiagaraEffect(FVector Location);
	void SpawnParticleEffect(FVector Location);
	virtual bool Prepare();
	virtual bool Attack();
	bool IsActive() const;
	UAnimMontage* GetAttackMontage() const;

	UFUNCTION()
	void StopPreparing();
	UFUNCTION()
	void StopAttack();

	float GetPrepareDuration();

	UFUNCTION()
	void Deactivate(UAnimMontage* Montage, bool bInterrupted);

	virtual UWorld* GetWorld() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Sound")
	TObjectPtr<USoundBase> SkillSound;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|StepBack")
	ESkillRange SkillRangeType;

	UPROPERTY()
	TObjectPtr<APPalCharacter> MyOwner;
	UPROPERTY()
	TObjectPtr<UAnimInstance> MyAnimInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Effects")
	TObjectPtr<UParticleSystem> ParticleSystem;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Effects")
	TObjectPtr<UNiagaraSystem> NiagaraSystem;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Effects")
	float ParticleSystemScale;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Effects")
	float NiagaraSystemScale;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Effects")
	float ParticleHeightOffset;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Effects")
	float NiagaraHeightOffset;

private:
	FTimerHandle PrepareHandle;
	FTimerHandle AttackHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (AllowPrivateAccess))
	bool bIgnoreCollision;
	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (AllowPrivateAccess))
	float IgnoreCollisionDuration;
	UFUNCTION()
	void ReEnableCollision();

	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (AllowPrivateAccess))
	float CooldownTime;
	//Max distance pal needs to be away from target to activate
	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (AllowPrivateAccess))
	float SkillDistance;
	//Move these to PalCharacter later when you have time
	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (AllowPrivateAccess))
	TObjectPtr<UAnimMontage> AttackMontage;

	//How long Attack loop animation lasts
	UPROPERTY(EditDefaultsOnly, Category = "Settings|AnimDuration", meta = (ClampMin = "0.0"), meta = (AllowPrivateAccess))
	float AttackDuration;
	//How long Prepare loop animation lasts
	UPROPERTY(EditDefaultsOnly, Category = "Settings|AnimDuration", meta = (ClampMin = "0.0"), meta = (AllowPrivateAccess))
	float PrepareDuration;

	//Must step back no matter how far away the pal is to the target.
	UPROPERTY(EditDefaultsOnly, Category = "Settings|StepBack", meta = (AllowPrivateAccess))
	bool bMustStepBack;
	//
	UPROPERTY(EditDefaultsOnly, Category = "Settings|StepBack", meta = (AllowPrivateAccess))
	bool bRandomizeStepBackMaxCount;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|StepBack", meta = (AllowPrivateAccess))
	int32 StepBackMaxCount;
	//Maximum distance this pal will step back if there're no walls
	UPROPERTY(EditDefaultsOnly, Category = "Settings|StepBack", meta = (AllowPrivateAccess))
	float StepBackDistance;
	//How close to the wall will this pal step back to?
	UPROPERTY(EditDefaultsOnly, Category = "Settings|StepBack")
	float MinStepBackNeeded;
	//How close Pal needs to be to Target to start stepping back
	UPROPERTY(EditDefaultsOnly, Category = "Settings|StepBack")
	float StepBackTriggerDistance;

	bool bEnabled;
	bool IsAttacking;
	bool IsPreparing;
	TObjectPtr<UObject> BoundObj;
};
