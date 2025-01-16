// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/GunPlayerAnimInstance.h"
#include "Sunny/PCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

void UGunPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	Player = Cast<APCharacter>(TryGetPawnOwner());

	if (nullptr == Player)
	{
		return;
	}

	FVector velocity = Player->GetVelocity();
	FVector forwardVector = Player->GetActorForwardVector();
	FVector rightVector = Player->GetActorRightVector();

	// 앞 뒤 속도
	Speed = FVector::DotProduct(forwardVector, velocity);

	// 좌 우 속도
	Direction = FVector::DotProduct(rightVector, velocity);

	// 캐릭터가 공중에 떠있는가?
	IsInAir = Player->GetCharacterMovement()->IsFalling();
}


void UGunPlayerAnimInstance::SetAimValue(float Value)
{
	if (Value <= 0)
	{
		IsAiming = false;
	}
	else
	{
		IsAiming = true;
	}

	AimValue = 100 * Value;
	UE_LOG(LogTemp, Warning, TEXT("AimValue : %f"), AimValue);
}

void UGunPlayerAnimInstance::AnimNotify_ThrowEnd()
{
	if (nullptr == Player)
	{
		return;
	}

	bHoldEnd = false;
	Player->ThrowPal();
}

void UGunPlayerAnimInstance::AnimNotify_ThrowDone()
{
	if (nullptr == Player)
	{
		return;
	}
	Player->HidePalAimUI();
	Player->ShowGunMesh();
	IsThrowing = false;
}

void UGunPlayerAnimInstance::AnimNotify_HoldEnd()
{
	if (nullptr == Player)
	{
		return;
	}
	Player->CameraZoomIn();
}

void UGunPlayerAnimInstance::PlayThrowAnim()
{
	if (nullptr == Player)
	{
		return;
	}
	//IsThrowing = Player->bSummonPal;

	bHoldEnd = IsThrowing;
	Player->CameraZoomOut();
}

void UGunPlayerAnimInstance::PlayHoldAnim(float Value)
{
	if (nullptr == Player)
	{
		return;
	}
	IsThrowing = Player->bSummonPal;

	if (!bHoldEnd)
	{
		if (Value <= 0)
		{
			Value = 0.f;
		}

		HoldValue = 100 * Value;
		UE_LOG(LogTemp, Warning, TEXT("HoldValue : %f"), HoldValue);

		//Player->CameraZoomIn();
	}
}

void UGunPlayerAnimInstance::PlayRetrieveAnim()
{
	if (nullptr == Player)
	{
		return;
	}
	if (!Player->bSummonPal)
	{
		IsRetrieving = true;
	}
}

void UGunPlayerAnimInstance::AnimNotify_PutBackEnd()
{
	if (nullptr == Player)
	{
		return;
	}
	IsRetrieving = false;
}

void UGunPlayerAnimInstance::PlayFireMontage()
{
	Montage_Play(FireMontage);
}

void UGunPlayerAnimInstance::PlayDiveRollMontage()
{
	Montage_Play(DiveRollMontage, 3.0f);
}

void UGunPlayerAnimInstance::PlayDeathMontage()
{
	//Montage_Play(DeathMontage);
	if (nullptr == Player)
	{
		return;
	}
	IsDead = Player->bPlayerDead;
}
