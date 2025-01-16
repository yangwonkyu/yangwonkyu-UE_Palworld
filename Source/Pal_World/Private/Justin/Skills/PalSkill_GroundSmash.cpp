// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Skills/PalSkill_GroundSmash.h"
#include "Justin/PPalCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Justin/Frameworks/PStatusLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"

UPalSkill_GroundSmash::UPalSkill_GroundSmash()
{
	State = EGroundSmashState::ONGROUND;
	TotalTravelTime = 0.f;
	AttackRadius = 400.f;
	SkillRangeType = ESkillRange::NEAR;
}

void UPalSkill_GroundSmash::Tick(float DeltaTime)
{
	TotalTravelTime += DeltaTime;

	switch (State)
	{
	case EGroundSmashState::TOAIR:
	{
		if (TotalTravelTime < MaxTravelTime)
		{
			FVector Pos = FMath::Lerp(From, To, TotalTravelTime / MaxTravelTime);
			MyOwner->SetActorLocation(Pos);
		}
		else
		{
			TotalTravelTime = 0.f;
			MyOwner->SetActorLocation(To);
			State = EGroundSmashState::INAIR;
		}
		break;
	}
	case EGroundSmashState::INAIR:
	{
		if (TotalTravelTime > GetPrepareDuration())
		{
			TotalTravelTime = 0.f;

			FVector Loc = MyOwner->GetActorLocation();
			Loc.Z = HeightZ;
			To = Loc;

			From = MyOwner->GetActorLocation();

			MaxTravelTime = FromAirTravelTime;
			State = EGroundSmashState::FROMAIR;

			StopPreparing();
		}
		break;
	}
	case EGroundSmashState::FROMAIR:
	{
		if (TotalTravelTime < MaxTravelTime)
		{
			FVector Pos = FMath::Lerp(From, To, TotalTravelTime / MaxTravelTime);
			MyOwner->SetActorLocation(Pos);
		}
		else
		{
			TotalTravelTime = 0.f;
			MyOwner->SetActorLocation(To);
			State = EGroundSmashState::ONGROUND;

			MyOwner->GetCharacterMovement()->GravityScale = Gravity;

			BeginAttack();
		}
		break;
	}
	}
}

TStatId UPalSkill_GroundSmash::GetStatId() const
{
	return TStatId();
}

bool UPalSkill_GroundSmash::IsAllowedToTick() const
{
	return State != EGroundSmashState::ONGROUND && IsActive();
}

bool UPalSkill_GroundSmash::Prepare()
{
	if (Super::Prepare())
	{
		if (MyOwner)
		{
			HeightZ = MyOwner->GetActorLocation().Z;

			From = MyOwner->GetActorLocation();
			To = MyOwner->GetTargetLocation();

			FVector Direction;
			if (MyOwner->GetTarget()) Direction = MyOwner->GetTarget()->GetActorForwardVector();
			else Direction = MyOwner->GetActorForwardVector();
			
			FQuat DirQuat = Direction.Rotation().Quaternion();
			FQuat RotateQuat = FVector(0.f, FMath::RandRange(-40.f, 40.f), 0.f).Rotation().Quaternion();

			FQuat ResultQuat = DirQuat * RotateQuat;

			To += ResultQuat.GetForwardVector() * FMath::RandRange(250.f, 300.f);
			To += FVector(0.f, 0.f, 2000.f);


			MaxTravelTime = ToAirTravelTime;
			State = EGroundSmashState::TOAIR;

			Gravity = MyOwner->GetCharacterMovement()->GravityScale;
			MyOwner->GetCharacterMovement()->GravityScale = 0.f;

			MyOwner->GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
			MyOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		}
		return true;
	}
	else return false;
}

bool UPalSkill_GroundSmash::Attack()
{
	if (Super::Attack())
	{
		FHitResult Hit;

		FVector Start = MyOwner->GetActorLocation();
		Start.Z = 0.f;
		FCollisionShape Shape;
		Shape.SetSphere(AttackRadius);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		//DrawDebugSphere(GetWorld(), Start, AttackRadius, 24, FColor::Blue, false, 2.f);
		if (GetWorld()->SweepSingleByObjectType(Hit, Start, Start, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel1, Shape, QueryParams))
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *GetNameSafe(Hit.GetActor()));
			//DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.f, 10, FColor::Blue, false, 2.f);
			UPStatusLibrary::ApplyDamage(MyOwner, Hit.GetActor(), Hit.ImpactPoint, 50.f);
		}

		Start.Z += 143.3f;
		SpawnNiagaraEffect(Start);
		SpawnParticleEffect(Start);

		return true;
	}
	else return false;
}
