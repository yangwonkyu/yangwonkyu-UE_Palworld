// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GunPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PAL_WORLD_API UGunPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APCharacter* Player;

	// Vertical
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Speed;

	// Horizontal
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Direction;

	// Jump
	UPROPERTY(EditDefaultsOnly , BlueprintReadWrite)
	bool IsInAir;

	// Aim
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
    float AimValue;
	UPROPERTY(EditDefaultsOnly , BlueprintReadWrite)
	bool IsAiming;
	UFUNCTION(BlueprintCallable)
    void SetAimValue(float Value);

	// Pal Summon
	UPROPERTY(EditDefaultsOnly , BlueprintReadWrite)
	bool IsThrowing;
	UPROPERTY(EditDefaultsOnly , BlueprintReadWrite)
	bool bHoldEnd;
	UPROPERTY(EditDefaultsOnly , BlueprintReadWrite)
	bool IsRetrieving;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
    float HoldValue;
	UFUNCTION()
	void AnimNotify_ThrowEnd();
	UFUNCTION()
	void AnimNotify_ThrowDone();
	UFUNCTION()
	void AnimNotify_HoldEnd();
	UFUNCTION()
	void PlayThrowAnim();
	UFUNCTION()
	void PlayHoldAnim(float Value);
	UFUNCTION()
	void PlayRetrieveAnim();
	UFUNCTION()
	void AnimNotify_PutBackEnd();


	// Death
	UPROPERTY(EditDefaultsOnly , BlueprintReadWrite)
	bool IsDead;



	// Montage Fire
	UPROPERTY(EditDefaultsOnly , BlueprintReadWrite)
	class UAnimMontage* FireMontage;
	void PlayFireMontage();

	// Montage DiveRoll
	UPROPERTY(EditDefaultsOnly , BlueprintReadWrite)
	class UAnimMontage* DiveRollMontage;
	void PlayDiveRollMontage();

	// Montage Death
	UPROPERTY(EditDefaultsOnly , BlueprintReadWrite)
	class UAnimMontage* DeathMontage;
	void PlayDeathMontage();
	
	
};
