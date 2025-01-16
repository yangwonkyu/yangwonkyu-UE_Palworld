// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Skills/PalSkill_IgnisBlast.h"
#include "Justin/PPalCharacter.h"
#include "Justin/BaseProjectile.h"
#include "Kismet/KismetMathLibrary.h"

UPalSkill_IgnisBlast::UPalSkill_IgnisBlast()
{
	SkillRangeType = ESkillRange::FAR;
}

bool UPalSkill_IgnisBlast::Attack()
{
	if (Super::Attack())
	{
		if (MyOwner)
		{
			FVector End = MyOwner->GetTargetLocation();
			FVector Start = MyOwner->GetActorLocation();

			FVector DirVec = End - Start;
			DirVec.Normalize();

			FRotator Rot = UKismetMathLibrary::FindLookAtRotation(Start, End);

			FTransform Trans;
			Trans.SetLocation(Start + SpawnOffset);
			Trans.SetRotation(DirVec.Rotation().Quaternion());
			Trans.SetScale3D(FVector::One());

			FActorSpawnParameters Param;
			Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			Param.Instigator = MyOwner;
			Param.Owner = MyOwner;

			GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, Trans, Param);
			return true;
		}
	}

	return false;
}
