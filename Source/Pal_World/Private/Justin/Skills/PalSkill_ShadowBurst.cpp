// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Skills/PalSkill_ShadowBurst.h"
#include "Justin/PPalCharacter.h"
#include "Justin/Frameworks/PStatusLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

UPalSkill_ShadowBurst::UPalSkill_ShadowBurst()
{
	AttackRadius = 500.f;
	SkillRangeType = ESkillRange::NEAR;
}

bool UPalSkill_ShadowBurst::Attack()
{
	if (Super::Attack())
	{
		FHitResult Hit;

		FVector Start = MyOwner->GetActorLocation();
		//Start.Z = 0.f;
		Start.Z += 300.f;
		FCollisionShape Shape;
		Shape.SetSphere(AttackRadius);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		//DrawDebugSphere(GetWorld(), Start, AttackRadius, 24, FColor::Blue, false, 4.f);
		if (GetWorld()->SweepSingleByObjectType(Hit, Start, Start, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel1, Shape, QueryParams))
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *GetNameSafe(Hit.GetActor()));
			//DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.f, 10, FColor::Blue, false, 4.f);
			UPStatusLibrary::ApplyDamage(MyOwner, Hit.GetActor(), Hit.ImpactPoint, 50.f);
		}
		SpawnParticleEffect(MyOwner->GetActorLocation());

		return true;
	}
	else return false;
}
