// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Projectiles/Projectile_SeedTrap.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Justin/Frameworks/PStatusLibrary.h"
#include "Justin/PPalCharacter.h"
#include "Sunny/PCharacter.h"
#include "Kismet/GameplayStatics.h"

AProjectile_SeedTrap::AProjectile_SeedTrap()
{
	LaunchSpeed = 1200.f;
	LaunchWeight = 1.f;

	DisperseSeedSpawnTotal = 12;
	DisperseSpeed = 1200.f;
	DisperseWeight = 1.38f;
	DispersePitch = 40.f;
	DispersePitchMultiplier = 2.f;
	DisperseSpawnScale = .4f;

	CountDownDuration = 2.f;
	TotalTime = 0.f;
	bBeganCountDown = false;
	ExplosionRadius = 100.f;

	MainSeedSpawnTotal = 8;
	MainSeedDisperseSpeed = 200.f;
	MainSeedDisperseWeight = .75f;
	MainSeedDisperseMultiplier = .6;
}

void AProjectile_SeedTrap::Initialize(AActor* _TargetActor, ESeedTrapState State, float _Magnitude, bool _bIsMainSeed)
{
	TargetActor = _TargetActor;
	if (!TargetActor) TargetLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 400.f;
	else TargetLocation = TargetActor->GetActorLocation();

	SetState(State);
	switch (SeedTrapState)
	{
	case ESeedTrapState::DISPERSE:
	{
		DisperseSpeed = _Magnitude;
		bIsMainSeed = _bIsMainSeed;
	}
	}
}

void AProjectile_SeedTrap::SetState(ESeedTrapState State)
{
	SeedTrapState = State;
}

void AProjectile_SeedTrap::BeginPlay()
{
	Super::BeginPlay();

	TotalTime = 0.f;
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile_SeedTrap::OnOverlapBegin);

	switch (SeedTrapState)
	{
	case ESeedTrapState::LAUNCHED:
	{
		Start = GetActorLocation();
		End = TargetLocation;

		DirectionVec = End - Start;

		float ZLength = DirectionVec.Z;
		float XLength = DirectionVec.Size2D();

		float Pitch;

		CalculateAngle(LaunchSpeed, ZLength, XLength, Pitch);

		FRotator Rot = GetActorRotation();
		Rot.Pitch = Pitch;

		SetActorRotation(Rot);

		DirectionVec = Rot.Vector();
		DirectionVec.Normalize();
		SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);

		break;
	}
	case ESeedTrapState::DISPERSE:
	{
		Start = GetActorLocation();
		SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);

		if (bIsMainSeed)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SeedSound, GetActorLocation());

			End = TargetLocation;
			FVector DirVec = End - Start;
			float ZLength = DirVec.Z;
			float XLength = DirVec.Size2D();
			float Pitch;

			CalculateAngle(DisperseSpeed, ZLength, XLength, Pitch);

			FRotator Rot = GetActorRotation();
			Rot.Pitch = Pitch;

			SetActorRotation(Rot);

			DirectionVec = Rot.Vector();
			DirectionVec.Normalize();

			DisperseWeight = MainSeedDisperseWeight;
		}
		else
		{
			DirectionVec = GetActorForwardVector();
		}

		break;
	}
	case ESeedTrapState::LANDED:
	{
		CountDownDuration = FMath::RandRange(CountDownDuration, CountDownDuration + CountDownDurationOffset);
		break;
	}
	}
}

void AProjectile_SeedTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (SeedTrapState)
	{
	case ESeedTrapState::LAUNCHED:
	{
		TotalTime += DeltaTime;
		float Time = TotalTime;
		float gravity = 0.5f * -GetWorld()->GetDefaultGravityZ() * LaunchWeight * LaunchWeight * Time * Time;

		FVector NewLocation = Start + DirectionVec * LaunchSpeed * Time - FVector(0, 0, gravity);

		SetActorLocation(NewLocation);

		break;
	}
	case ESeedTrapState::DISPERSE:
	{
		TotalTime += DeltaTime;
		float Time = TotalTime;
		float gravity = 0.5f * -GetWorld()->GetDefaultGravityZ() * DisperseWeight * DisperseWeight * Time * Time;

		FVector NewLocation = Start + DirectionVec * DisperseSpeed * Time - FVector(0, 0, gravity);

		SetActorLocation(NewLocation);

		break;
	}
	case ESeedTrapState::LANDED:
	{
		if (!bBeganCountDown && !LandedHandle.IsValid())
		{
			bBeganCountDown = true;
			GetWorld()->GetTimerManager().SetTimer(LandedHandle, this, &AProjectile_SeedTrap::StartExplosion, CountDownDuration, false);
		}
		if (bBeganCountDown && LandedHandle.IsValid())
		{
			//Countdown Effect
		}
		break;
	}
	}
}

void AProjectile_SeedTrap::OnOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin_Implementation(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	TotalTime = 0.f;

	switch (SeedTrapState)
	{
	case ESeedTrapState::LAUNCHED:
	{
		UPStatusLibrary::ApplyDamage(GetOwner(), OtherActor, SweepResult.ImpactPoint, 50.f);
		//DrawDebugSphere(GetWorld(), GetActorLocation(), 40.f, 10, FColor::Red, false, 4.f);

		SpawnDisperseSeeds();

		//Spawn a seed that fires towards the feet of target. This seed explodes and spawns more seeds on contact with the ground.
		AProjectile_SeedTrap* Trap;
		Trap = GetWorld()->SpawnActorDeferred<AProjectile_SeedTrap>(
			SeedTrapClass, FTransform::Identity, GetOwner(), GetInstigator());
		if (Trap)
		{
			FVector Begin = GetActorLocation();
			FVector Target = TargetLocation;
			Target.Z = 0.f;

			FVector DirVec = Target - Begin;

			FRotator Rot(DirVec.Rotation());
			Rot.Pitch = 0.f;

			FTransform Trans;
			Trans.SetLocation(GetActorLocation());
			Trans.SetRotation(Rot.Quaternion());

			Trap->Initialize(TargetActor, ESeedTrapState::DISPERSE, MainSeedDisperseSpeed, true);
			UGameplayStatics::FinishSpawningActor(Trap, Trans);
		}
		Trap = nullptr;

		Explode();
		break;
	}
	case ESeedTrapState::DISPERSE:
	{
		if (bIsMainSeed)
		{
			SpawnDisperseSeeds(MainSeedDisperseMultiplier);
			Explode();
		}
		else
		{
			SetState(ESeedTrapState::LANDED);
			SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
			//Countdown Effect
		}

		break;
	}
	case ESeedTrapState::LANDED:
	{
		//DrawDebugSphere(GetWorld(), GetActorLocation(), 40.f, 10, FColor::Red, false, 4.f);		
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SeedSound, GetActorLocation());
		UPStatusLibrary::ApplyDamage(GetOwner(), OtherActor, SweepResult.ImpactPoint, 50.f);
		Explode();
		break;
	}
	}
}

void AProjectile_SeedTrap::SpawnDisperseSeeds(float Multiplier)
{
	int Total = DisperseSeedSpawnTotal;
	if (bIsMainSeed)
	{
		Total = MainSeedSpawnTotal;
	}

	float Yaw = 360.f / Total;

	if (SeedTrapClass)
	{
		FTransform Trans;
		AProjectile_SeedTrap* Trap;
		for (int i = 0; i < Total; ++i)
		{
			//Spawn seeds that travel in cones.
			Trap = GetWorld()->SpawnActorDeferred<AProjectile_SeedTrap>(
				SeedTrapClass, Trans.Identity, GetOwner(), GetInstigator());

			if (Trap)
			{
				FQuat ActorQuat = Trap->GetActorRotation().Quaternion();
				FQuat RotateQuat;

				float Speed = DisperseSpeed;
				float SpeedOffset = DisperseSpeedOffset;
				float Pitch = DispersePitch;
				float PitchMulti = DispersePitchMultiplier;

				//if main seed is spawning disperse seeds
				if (bIsMainSeed)
				{
					auto SeedTrapDefault = AProjectile_SeedTrap::StaticClass()->GetDefaultObject<AProjectile_SeedTrap>();
					if (SeedTrapDefault)
					{
						Speed = SeedTrapDefault->DisperseSpeed;
						SpeedOffset = SeedTrapDefault->DisperseSpeedOffset;
					}
					Pitch = FMath::RandRange(-1.f * Pitch, Pitch);
				}

				if (i % 2 == 1)
				{
					RotateQuat = FRotator(Pitch* PitchMulti, Yaw * i, 0).Quaternion();
				}
				else
				{
					RotateQuat = FRotator(Pitch, Yaw * i, 0).Quaternion();
				}
				
				//Speed = FMath::RandRange(Speed * Multiplier, (Speed + SpeedOffset) * Multiplier);
				Speed = FMath::RandRange((Speed - SpeedOffset) * Multiplier, Speed * Multiplier);
				
				Trap->Initialize(TargetActor, ESeedTrapState::DISPERSE, Speed);

				FQuat ResultQuat = ActorQuat * RotateQuat;
				Trans.SetRotation(ResultQuat);
				Trans.SetLocation(GetActorLocation());
				Trans.SetScale3D(GetActorScale3D() * DisperseSpawnScale);

				UGameplayStatics::FinishSpawningActor(Trap, Trans);
			}
		}

		if (!bIsMainSeed)
		{
			//Spawn Seed that fires straight down;
			Trap = GetWorld()->SpawnActorDeferred<AProjectile_SeedTrap>(
				SeedTrapClass, Trans.Identity, GetOwner(), GetInstigator());
			if (Trap)
			{
				Trap->Initialize(TargetActor, ESeedTrapState::DISPERSE);
				FQuat ActorQuat = Trap->GetActorRotation().Quaternion();

				FQuat RotateQuat = FRotator(-90.f, 0, 0).Quaternion();

				FQuat ResultQuat = ActorQuat * RotateQuat;
				Trans.SetRotation(ResultQuat);
				Trans.SetLocation(GetActorLocation());
				Trans.SetScale3D(GetActorScale3D() * DisperseSpawnScale);

				UGameplayStatics::FinishSpawningActor(Trap, Trans);
			}
		}
	}
}

void AProjectile_SeedTrap::StartExplosion()
{
	FHitResult Hit;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), SeedSound, GetActorLocation());

	Start = GetActorLocation();
	FCollisionShape Shape;
	Shape.SetSphere(ExplosionRadius);
	FCollisionQueryParams QueryParams;

	auto Pal = Cast<APPalCharacter>(GetOwner());
	if (Pal)
	{
		QueryParams.AddIgnoredActor(GetOwner());
		QueryParams.AddIgnoredActor(Cast<AActor>(Pal->GetPlayer()));
	}

	
	if (GetWorld()->SweepSingleByObjectType(Hit, Start, Start, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel1, Shape, QueryParams))
	{
		//DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 40.f, 10, FColor::Red, false, 4.f);
		UPStatusLibrary::ApplyDamage(GetOwner(), Hit.GetActor(), Hit.ImpactPoint, 5.f);
	}
	//else DrawDebugSphere(GetWorld(), Start, ExplosionRadius, 24, FColor::Blue, false, 3.f);

	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorHiddenInGame(true);
	
	Explode();
}