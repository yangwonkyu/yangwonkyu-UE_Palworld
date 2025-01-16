// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Skills/PalSkill_SeedMine.h"
#include "Justin/PPalCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Justin/Projectiles/Projectile_SeedTrap.h"

UPalSkill_SeedMine::UPalSkill_SeedMine()
{
	SkillRangeType = ESkillRange::FAR;
}

bool UPalSkill_SeedMine::Attack()
{
	if (Super::Attack())
	{
		if (MyOwner)
		{
			FVector End = MyOwner->GetTargetLocation();
			FVector Start = MyOwner->GetActorLocation();

			FVector DirVec = End - Start;
			DirVec.Normalize();

			FRotator Rot = DirVec.Rotation();

			FTransform Trans;
			Trans.SetLocation(Start + SpawnOffset);
			Trans.SetRotation(Rot.Quaternion());
			Trans.SetScale3D(FVector::One());

			auto SpawnActor = Cast<AProjectile_SeedTrap>(GetWorld()->SpawnActorDeferred<ABaseProjectile>(ProjectileClass, Trans, MyOwner, MyOwner));
			if (SpawnActor)
			{
				SpawnActor->Initialize(MyOwner->GetTarget(), ESeedTrapState::LAUNCHED);
				UGameplayStatics::FinishSpawningActor(SpawnActor, Trans);
			}
		}
		return true;
	}
	else return false;
}