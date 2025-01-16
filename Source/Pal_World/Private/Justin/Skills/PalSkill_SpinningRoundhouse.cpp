// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Skills/PalSkill_SpinningRoundhouse.h"
#include "Justin/PPalCharacter.h"
#include "Justin/Frameworks/PStatusLibrary.h"
#include "Particles/ParticleSystemComponent.h"

UPalSkill_SpinningRoundhouse::UPalSkill_SpinningRoundhouse()
{
	Radius = 200.f;
	SkillRangeType = ESkillRange::NEAR;
}

bool UPalSkill_SpinningRoundhouse::Attack()
{
	if (Super::Attack())
	{
		if (MyOwner)
		{
			FHitResult Hit;

			FVector Start = MyOwner->GetActorLocation();
			Start.Z = 0.f;

			FCollisionShape Shape;
			Shape.SetSphere(Radius);

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(MyOwner);

			//DrawDebugSphere(GetWorld(), Start, Radius, 24, FColor::Blue, false, 2.f);

			if (GetWorld()->SweepSingleByObjectType(Hit, Start, Start, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel1, Shape, QueryParams))
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *GetNameSafe(Hit.GetActor()));
				UPStatusLibrary::ApplyDamage(MyOwner, Hit.GetActor(), Hit.ImpactPoint, 50.f);
				SpawnParticleEffect(Hit.ImpactPoint);
				//DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 15.f, 23,FColor::Red, false, 2.f);
			}

			Start.Z = MyOwner->GetBaseHeight();
			Start.Z += 210.f;
			SpawnNiagaraEffect(Start);

		}

		return true;
	}
	else return false;
}
