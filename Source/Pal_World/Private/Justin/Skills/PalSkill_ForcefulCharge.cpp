// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Skills/PalSkill_ForcefulCharge.h"
#include "Justin/PPalCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Justin/Frameworks/PStatusLibrary.h"
#include "Justin/PPalSkillComponent.h"
#include "Justin/PFSMComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshCOmponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Justin/Pals/PalCharacter_Anubis.h"

UPalSkill_ForcefulCharge::UPalSkill_ForcefulCharge()
{
	TotalMoveTime = 0.f;
	bTicking = false;
	AttackOffsetDist = 200.f;
	AttackRadius = 200.f;
	SkillRangeType = ESkillRange::FAR;
	bSlidingBack = true;
	bCharging = false;
	Velocity = 500.f;
	DistanceTraveled = 2000.f;
	StartAttackDistance = 400.f;

	SlideDist = 1600.f;
	AllowedSlideDistance = 1400.f;
	MaxSlideTime = 3.f;
	bHasAttacked = false;
	bMontageJumped = false;
	bSlowingDown = false;

	PrepareScale = 2.f;
	AttackScale = 2.f;
}

void UPalSkill_ForcefulCharge::Tick(float DeltaTime)
{
	TotalMoveTime += DeltaTime;

	if (bSlidingBack)
	{
		if (TotalMoveTime < MaxSlideTime)
		{
			FVector CurrLoc = FMath::Lerp(StartSlide, EndSlide, TotalMoveTime / MaxSlideTime);
			MyOwner->SetActorLocation(CurrLoc);
		}
		else
		{
			MyOwner->SetActorLocation(EndSlide);
			TotalMoveTime = 0.f;
			bSlidingBack = false;
		}
	}
	else if (bCharging)
	{
		FVector Dist = EndLoc - MyOwner->GetActorLocation();

		float what = Dist.Size2D();

		if (what < TotalMoveDistance * .3f)
		{
			if (!bSlowingDown)
			{
				auto Anubis = Cast<APalCharacter_Anubis>(MyOwner);
				if (Anubis)
				{
					Anubis->DeactivateForcefulChargeEffect();
				}
				bSlowingDown = true;
			}
		}

		if (what < TotalMoveDistance * .22f)
		{
			if (!bMontageJumped)
			{
				MyOwner->GetMesh()->GetAnimInstance()->Montage_JumpToSection("2", GetAttackMontage()); // continue with punch montage
				bMontageJumped = true;
			}
		}

		if (what < TotalMoveDistance * .1f)
		{
			if (!bHasAttacked)
			{
				bHasAttacked = true;
			}
		}

		if (!bHasAttacked)
		{
			FHitResult Hit;
			FCollisionObjectQueryParams ObjQueryParam;
			ObjQueryParam.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);
			FCollisionShape Shape;
			Shape.SetSphere(AttackRadius);

			if (GetWorld()->SweepSingleByObjectType(Hit, MyOwner->GetActorLocation(), MyOwner->GetActorLocation(), FQuat::Identity, ObjQueryParam, Shape))
			{
				SkillComp->StartAttack();
				bHasAttacked = true;
				SpawnAttackEffect(Hit.ImpactPoint);
			}
		}

		if (TotalMoveTime < MaxMoveTime)
		{
			float DistToCheck = (MyOwner->GetTargetLocation() - MyOwner->GetActorLocation()).Size2D();
			if (DistToCheck < StartAttackDistance)
			{
				if (!bMontageJumped)
				{
					UE_LOG(LogTemp, Warning, TEXT("DistToCheck: %f, StartAttackDistance: %f"), DistToCheck, StartAttackDistance);
					MyOwner->GetMesh()->GetAnimInstance()->Montage_JumpToSection("2", GetAttackMontage()); // continue with punch montage
					bMontageJumped = true;
				}
			}

			FVector CurrLoc;
			CurrLoc = FMath::Lerp(StartLoc, EndLoc, EaseOutSine(TotalMoveTime / MaxMoveTime));
			MyOwner->SetActorLocation(CurrLoc);

		}
		else
		{
			MyOwner->SetActorLocation(EndLoc);

			TotalMoveTime = 0.f;
			bTicking = false;
			bCharging = false;
			bMontageJumped = false;
		}
	}
}

TStatId UPalSkill_ForcefulCharge::GetStatId() const
{
	return TStatId();
}

bool UPalSkill_ForcefulCharge::IsAllowedToTick() const
{
	return bTicking;
}

void UPalSkill_ForcefulCharge::Activate(UObject* Obj, FName FuncName)
{
	Super::Activate(Obj, FuncName);

	bTicking = true;
	bHasAttacked = false;
	bMontageJumped = false;
	bSlowingDown = false;

	if (MyOwner)
	{
		if (!SkillComp)
		{
			SkillComp = MyOwner->GetComponentByClass<UPPalSkillComponent>();
		}

		auto FSM = MyOwner->GetComponentByClass<UPFSMComponent>();
		if (FSM && FSM->GetStepBack())
		{
			FVector Start = MyOwner->GetActorLocation();
			FVector End = MyOwner->GetTargetLocation();

			End.Z = MyOwner->GetBaseHeight();
			Start.Z = MyOwner->GetBaseHeight();

			FVector DirectionVector = Start - End;
			DirectionVector.Normalize();

			DirectionVector *= SlideDist; //SlideDist;

			End = Start + DirectionVector;

			TArray<FHitResult> Hits;
			FCollisionShape Shape;
			Shape.SetSphere(20.f);
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(MyOwner);
			//DrawDebugSphere(GetWorld(), Start, 32.f, 24, FColor::Black, false, 5.f);
			//DrawDebugSphere(GetWorld(), End, 32.f, 24, FColor::Black, false, 5.f);
			if (GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECollisionChannel::ECC_Visibility, Shape, QueryParams))
			{
				FVector NewDirectionVector = Hits[0].ImpactPoint - Start;

				if (NewDirectionVector.Size2D() < AllowedSlideDistance) //AllowedSlideDistance)
				{
					bSlidingBack = false;
					return;
				}
				else EndSlide = MyOwner->GetActorLocation() + MyOwner->GetActorForwardVector() * -1.f * NewDirectionVector.Size2D();
			}
			else EndSlide = MyOwner->GetActorLocation() + MyOwner->GetActorForwardVector() * -1.f * SlideDist;

			StartSlide = MyOwner->GetActorLocation();

			bSlidingBack = true;
		}
		else bSlidingBack = false;

	}
}

bool UPalSkill_ForcefulCharge::Prepare()
{
	if (Super::Prepare())
	{
		auto Attach = UGameplayStatics::SpawnSoundAttached(PrepareSound, MyOwner->GetMesh(), FName("pelvis"));

		bSlidingBack = false;
		bCharging = true;
		TotalMoveTime = 0.f;

		StartLoc = MyOwner->GetActorLocation();

		FVector End = MyOwner->GetTargetLocation();
		End.Z = StartLoc.Z;

		FVector DirectionVec = End - StartLoc;
		DirectionVec.Normalize();

		FHitResult Hit;
		FCollisionShape Shape;
		Shape.SetSphere(32.f);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(MyOwner->GetTarget());

		if (GetWorld()->SweepSingleByChannel(Hit, StartLoc + DirectionVec * 32.f, StartLoc + DirectionVec * DistanceTraveled,
			FQuat::Identity, ECollisionChannel::ECC_Visibility, Shape, QueryParams))
		{
			//There is an obstacle in the way
			//DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 24.f, 24, FColor::White, true);
			EndLoc = Hit.ImpactPoint;
			EndLoc.Z = StartLoc.Z;
		}
		else
		{
			EndLoc = StartLoc + DirectionVec * DistanceTraveled;
			EndLoc.Z = StartLoc.Z;
		}

		FVector ToOwnerVec = StartLoc - EndLoc;
		FVector OffsetDirVec = ToOwnerVec.GetSafeNormal();
		EndLoc += OffsetDirVec * AttackOffsetDist;

		float DistanceToTarget = (EndLoc - StartLoc).Size2D();

		MaxMoveTime = DistanceToTarget / Velocity;

		TotalMoveDistance = (EndLoc - StartLoc).Size2D();

		//DrawDebugSphere(GetWorld(), StartLoc, 24.f, 24, FColor::White, true);
		//DrawDebugSphere(GetWorld(), EndLoc, 24.f, 24, FColor::Black, true);

		auto Owner = MyOwner->GetOwner();
		auto AI = Cast<AAIController>(Owner);
		if (AI)
		{
			AI->SetFocus(nullptr);
		}

		return true;
	}
	else return false;
}

bool UPalSkill_ForcefulCharge::Attack()
{
	if (Super::Attack())
	{
		FHitResult Hit;

		FVector Start = MyOwner->GetActorLocation();
		FVector End = MyOwner->GetTargetLocation();

		FVector DirVec = End - Start;
		DirVec.Normalize();
		Start += DirVec * AttackOffsetDist;

		FCollisionShape Shape;
		Shape.SetSphere(AttackRadius);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		//DrawDebugSphere(GetWorld(), Start, AttackRadius, 24, FColor::Blue, false, 3.f);
		if (GetWorld()->SweepSingleByObjectType(Hit, Start, Start, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel1, Shape, QueryParams))
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *GetNameSafe(Hit.GetActor()));
			//DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.f, 10, FColor::Blue, false, 3.f);
			UPStatusLibrary::ApplyDamage(MyOwner, Hit.GetActor(), Hit.ImpactPoint, 50.f);
		}
		return true;
	}
	else return false;
}

void UPalSkill_ForcefulCharge::SpawnAttackEffect(FVector Location)
{
	if (PrepareParticleSystem && AttackParticleSystem)
	{
		UParticleSystemComponent* Sys = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PrepareParticleSystem, Location, FRotator());
		if (Sys)
		{
			Sys->SetWorldScale3D(FVector(PrepareScale));
		}
		Sys = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AttackParticleSystem, Location, FRotator());
		if (Sys)
		{
			Sys->SetWorldScale3D(FVector(AttackScale));
		}
	}
}