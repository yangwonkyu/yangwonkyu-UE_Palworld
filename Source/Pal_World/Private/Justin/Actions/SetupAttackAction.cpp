// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Actions/SetupAttackAction.h"
#include "Justin/PPalCharacter.h"
#include "AIController.h"
#include "Justin/PPalSkillComponent.h"
#include "Components/CapsuleComponent.h"

USetupAttackAction::USetupAttackAction()
{
	RunSpeed = 300.f;
	ClearOffset = 150.f;
	ClearOffsetRate = 25.f;
	Radius = 80.f;

	bIsClear = false;

	bHadObstacles = false;

	bSteppingBack = false;
	StepBackCount = 0;
	StepBackSpeed = 1200.f;
	StepBackOffsetFromObstacle = 100.f;

	//MaxStepBackDistance = 700.f;
	//AllowedMinStepBackDistance = 300.f;
	//MaxStepBackCount = 2;
}

void USetupAttackAction::Start()
{
	Super::Start();

	StartingClearOffset = ClearOffset;
	auto AI = PalCharacter->GetController<AAIController>();
	if (AI && !AI->ReceiveMoveCompleted.IsBound())
	{
		AI->ReceiveMoveCompleted.AddDynamic(this, &USetupAttackAction::OnMoveComplete);
	}
}

EPalState USetupAttackAction::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	{
		FVector Start = PalCharacter->GetTargetLocation();
		FVector End = PalCharacter->GetActorLocation();
		Start.Z = 50.f;
		End.Z = 50.f;
		FVector DirVec = Start - End;
		DirVec.Normalize();

		FVector DistanceVec = DirVec * SkillComp->GetStepBackTriggerDistance();

		DrawDebugLine(GetWorld(), Start, Start + DistanceVec, FColor::Green, false);
	}

	if (PalCharacter && !PalCharacter->HasTarget())
	{
		return EPalState::IDLE;
	}

	if (bIsClear)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can Attack"));
		return EPalState::ATTACK;
	}

	if (SkillComp)
	{
		if (bSteppingBack)
		{
			if (!StepBack(DeltaTime))
			{
				if (SkillComp->MustStepBack())
				{
					if (CanStepBack() && StepBackCount < StepBackMaxCount)
					{
						StepBackEnd = FindStepBackLocation();
						StepBackDirection = StepBackEnd - PalCharacter->GetActorLocation();
						StepBackDirection.Normalize();
						PalCharacter->SetIsInAir(true);
					}
					else bIsClear = true;
				}
				else if (/*TooCloseToTarget() || */CanStepBack() && StepBackCount < StepBackMaxCount)
				{
					StepBackEnd = FindStepBackLocation();
					StepBackDirection = StepBackEnd - PalCharacter->GetActorLocation();
					StepBackDirection.Normalize();
					PalCharacter->SetIsInAir(true);
					//bIsClear = true;
				}
				else bIsClear = true;
			}

			return EPalState::NONE;
		}
	}

	UpdateMovement();

	return EPalState::NONE;
}

void USetupAttackAction::Stop()
{
	Super::Stop();
	auto AI = PalCharacter->GetController<AAIController>();
	if (AI)
	{
		if (AI->ReceiveMoveCompleted.IsBound()) AI->ReceiveMoveCompleted.Clear();
		AI->StopMovement();
	}
	bIsClear = false;
	bHadObstacles = false;
	bSteppingBack = false;
	StepBackCount = 0;

	PalCharacter->SetIsInAir(false);
}

void USetupAttackAction::UpdateMovement()
{
	if (HasObstacles() && !TargetInfront())
	{
		bHadObstacles = true;
		Adjust();
	}
	else
	{
		if (SkillComp)
		{
			switch (SkillComp->GetSkillRangeType())
			{
			case ESkillRange::FAR:
			{
				if (!bHadObstacles && 
					(
						(SkillComp->MustStepBack() && CanStepBack()) || 
						(TooCloseToTarget() && CanStepBack()))
					)
				{
					auto AI = PalCharacter->GetController<AAIController>();
					if (AI) AI->SetFocus(PalCharacter->GetTarget());

					bSteppingBack = true;
					StepBackEnd = FindStepBackLocation();
					StepBackDirection = StepBackEnd - PalCharacter->GetActorLocation();
					TotalStepBackDistance = StepBackDirection.Size2D();
					StepBackDirection.Normalize();
					StepBackMaxCount = SkillComp->GetStepBackAmount();
					if (FSMComp)
					{
						FSMComp->SetStepBack(true);
					}
				}
				else
				{
					FSMComp->SetStepBack(false);
					Move();
				}

				break;
			}
			case ESkillRange::NEAR:
			{
				FSMComp->SetStepBack(false);
				Move();
				break;
			}
			}
		}
	}
}

void USetupAttackAction::OnMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	switch (Result)
	{
	case EPathFollowingResult::Type::Success:
	{
		UE_LOG(LogTemp, Warning, TEXT("Adjustment SUCCESS"));
		//bIsReady = true;
		ClearOffset = StartingClearOffset;
		break;
	}
	case EPathFollowingResult::Type::Aborted:
	{
		ClearOffset = StartingClearOffset;
		break;
	}
	case EPathFollowingResult::Type::Invalid:
	{
		ClearOffset += ClearOffsetRate;
		break;
	}
	default:
		UE_LOG(LogTemp, Warning, TEXT("WHAT IS THIS NONESENSE %s"), *UEnum::GetValueAsString(Result));
	}
}

bool USetupAttackAction::HasObstacles() const
{
	FHitResult Hit;
	FVector Start = PalCharacter->GetActorLocation();
	FVector End = PalCharacter->GetTargetLocation();

	FVector DirVec = End - Start;
	DirVec.Normalize();

	Start += DirVec * Radius;

	FCollisionShape Shape;
	Shape.SetSphere(Radius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PalCharacter);

	return GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECollisionChannel::ECC_Visibility, Shape, QueryParams);
}

void USetupAttackAction::Adjust()
{
	FHitResult Hit;
	FVector Start = PalCharacter->GetActorLocation();
	FVector End = PalCharacter->GetTargetLocation();

	FVector DirVec = End - Start;
	DirVec.Normalize();

	Start += DirVec * Radius;

	FCollisionShape Shape;
	Shape.SetSphere(Radius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PalCharacter);

	//From pal to Target sweep trace
	if (GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECollisionChannel::ECC_Visibility, Shape, QueryParams))
	{
		Start = PalCharacter->GetTargetLocation();
		End = PalCharacter->GetActorLocation();

		DirVec = End - Start;
		DirVec.Normalize();

		Start += DirVec * Radius;

		//From Target to Pal sweep trace to find good location to move pal to.
		if (GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECollisionChannel::ECC_Visibility, Shape, QueryParams))
		{
			FVector DirectionVec = Start - Hit.ImpactPoint;
			float Dist = DirectionVec.Length();
			DirectionVec.Normalize();

			FVector result = DirectionVec * ClearOffset + Hit.ImpactPoint;

			PalCharacter->MoveToLocation(RunSpeed, result);
		}
	}
}

void USetupAttackAction::Move()
{
	if (ensureAlways(SkillComp) && SkillComp->IsWithinTarget(PalCharacter->GetTarget()))
	{
		bIsClear = true;
		return;
	}
	else
	{
		FVector Start = PalCharacter->GetTargetLocation();
		FVector End = PalCharacter->GetActorLocation();

		FVector DirVec = End - Start;
		DirVec.Normalize();

		FVector MoveVec = DirVec * 50.f;

		PalCharacter->MoveToLocation(RunSpeed, MoveVec + Start);
	}
}

bool USetupAttackAction::TargetInfront() const
{
	FHitResult Hit;
	FVector Start = PalCharacter->GetActorLocation();
	FVector End = PalCharacter->GetTargetLocation();

	FVector DirVec = End - Start;
	DirVec.Normalize();

	Start += DirVec * Radius;

	FCollisionShape Shape;
	Shape.SetSphere(Radius);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PalCharacter);

	return GetWorld()->SweepSingleByObjectType(Hit, Start, End, FQuat::Identity, ObjectQueryParams ,Shape, QueryParams);
}

bool USetupAttackAction::CanStepBack() const
{
	if (!ensure(SkillComp)) return false;

	FVector Start = PalCharacter->GetTargetLocation();
	FVector End = PalCharacter->GetActorLocation();

	//Adjustments for SH's map
	Start.Z = PalCharacter->GetBaseHeight() + 210.f;
	End.Z = PalCharacter->GetBaseHeight() + 210.f;

	FVector DirVec = End - Start;
	DirVec.Normalize();

	FVector DistanceVec = DirVec * SkillComp->GetStepBackDistance();

	FVector StepBackLocation = DistanceVec + End;

	FCollisionShape Shape;
	Shape.SetSphere(Radius);

	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PalCharacter);
	QueryParams.AddIgnoredActor(PalCharacter->GetTarget());

	if (GetWorld()->SweepSingleByChannel(Hit, End, StepBackLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, Shape, QueryParams))
	{
		FVector DistVec = Hit.ImpactPoint - End;
		
		//DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 24.f, 50, FColor::Green, true, 8.f);
		UE_LOG(LogTemp, Warning, TEXT("14Hit:%s"), *Hit.GetActor()->GetName());
		float Dist = DistVec.Size2D();
		float DistOffsetted = Dist - StepBackOffsetFromObstacle;

		return DistOffsetted > SkillComp->GetMinStepBackNeeded();
	}
	//else DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 24.f, 50, FColor::White, false, 2.f);

	return true;
}

bool USetupAttackAction::TooCloseToTarget() const
{
	if (PalCharacter && SkillComp)
	{
		FVector DistVec = PalCharacter->GetTargetLocation() - PalCharacter->GetActorLocation();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("Too Close with Far type skill? %s"), DistVec.Length() < SkillComp->GetStepBackTriggerDistance() ? TEXT("YES") : TEXT("NO")));

		float ToTargetLength = DistVec.Length();
		float StepBackTriggerDistance = SkillComp->GetStepBackTriggerDistance();
		return ToTargetLength < StepBackTriggerDistance;
	}
	return false;
}

FVector USetupAttackAction::FindStepBackLocation()
{
	if (PalCharacter)
	{
		if (SkillComp)
		{
			FVector Start = PalCharacter->GetTargetLocation();
			FVector End = PalCharacter->GetActorLocation();

			//Adjustments for SH's map
			Start.Z = PalCharacter->GetBaseHeight() + 210.f;
			End.Z = PalCharacter->GetBaseHeight() + 210.f;

			//DrawDebugSphere(GetWorld(), End, 24.f, 1, FColor::Blue, true);

			FVector DirVec = End - Start;
			DirVec.Normalize();

			FVector DistanceVec = DirVec * SkillComp->GetStepBackDistance();

			FVector StepBackLocation = DistanceVec + End;

			FHitResult Hit;
			FCollisionShape Shape;
			Shape.SetSphere(Radius);
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(PalCharacter);
			QueryParams.AddIgnoredActor(PalCharacter->GetTarget());

			if (GetWorld()->SweepSingleByChannel(Hit, End, StepBackLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, Shape, QueryParams))
			{
				//There is an obstacle in the way
				DirVec = Hit.ImpactPoint - End;
				float Dist = DirVec.Size2D();
				DirVec.Normalize();
				DistanceVec = DirVec * (Dist - StepBackOffsetFromObstacle);

				StepBackLocation = End + DistanceVec;
			}

			End.Z = PalCharacter->GetBaseHeight();
			StepBackLocation.Z = PalCharacter->GetBaseHeight();

			//DrawDebugSphere(GetWorld(), StepBackLocation, Radius, 1, FColor::Emerald, true, 5.f);
			return StepBackLocation;
		}
	}

	return FVector::Zero();
}

bool USetupAttackAction::StepBack(float DeltaTime)
{
	if (!PalCharacter) return true;

	if(!PalCharacter->GetIsInAir())
		PalCharacter->SetIsInAir(true);


	FVector PalLoc = PalCharacter->GetActorLocation();
	FVector ToDestination = StepBackEnd - PalLoc;
	float Dist = ToDestination.Size2D();
	float TotalDist = TotalStepBackDistance * .22f;
	UE_LOG(LogTemp, Warning, TEXT("111TotalDist: %f, CurrDist: %f"), TotalDist, Dist);
	if (TotalDist > Dist)
	{
		UE_LOG(LogTemp, Warning, TEXT("111Stop JUMP!"));
		PalCharacter->SetIsInAir(false);
	}

	ToDestination.Normalize();
	float result = FVector::DotProduct(StepBackDirection, ToDestination);

	GEngine->AddOnScreenDebugMessage(01, 0.f, FColor::Magenta, FString::Printf(TEXT("Result: %f"), result));


	if (result > 0.5f) //not yet to step back destination
	{
		FVector MovementVec = StepBackDirection * StepBackSpeed;
		PalCharacter->SetActorLocation(PalLoc + MovementVec * DeltaTime);
		return true;
	}
	else
	{
		StepBackCount++;
		PalCharacter->SetIsInAir(false);
		return false;
	}
}
