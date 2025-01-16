// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Actions/MovementAction.h"
#include "Justin/PPalCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Justin/PFSMComponent.h"


UMovementAction::UMovementAction()
{
	bIsMoving = false;
	bCanMove = true;
	bIsSkillAvailable = false;
	MovementType = EMovement::FAR;
	WalkThreshold = 200.f;
	WalkSpeed = 50.f;
	StrafeAngle = 60.f;
	StrafeDistance = 200.f;
	NegateVal = 1.f;
	ToSetupStateDelayTime = 1.5f;
}

void UMovementAction::Start()
{
	Super::Start();

	auto AI = PalCharacter->GetController<AAIController>();
	if (AI && !AI->ReceiveMoveCompleted.IsBound())
	{
		AI->ReceiveMoveCompleted.AddDynamic(this, &UMovementAction::OnMoveComplete);
	}
}
void UMovementAction::DelayElapsed()
{
	bIsSkillAvailable = true;
}
EPalState UMovementAction::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if (PalCharacter && !PalCharacter->HasTarget())
	{
		return EPalState::IDLE;
	}

	if (SkillComp && SkillComp->IsSkillAvailable())
	{
		if (!bIsSkillAvailable && !SkillHandle.IsValid())
		{
			GetWorld()->GetTimerManager().SetTimer(SkillHandle, this, &UMovementAction::DelayElapsed, ToSetupStateDelayTime, false);
		}
		if (bIsSkillAvailable)
		{
			return EPalState::SETUPATTACK;
		}
	}

	if (PalCharacter)
	{
		FVector DirVec = PalCharacter->GetTargetLocation() - PalCharacter->GetActorLocation();
		if (DirVec.Length() > WalkThreshold)
		{
			if (MovementType == EMovement::NEAR && bIsMoving)
			{
				bIsMoving = false;
				MovementType = EMovement::FAR;
			}
		}
		else
		{
			if (MovementType == EMovement::FAR && bIsMoving)
			{
				bIsMoving = false;
				MovementType = EMovement::NEAR;
			}
		}

		if (!bIsMoving)
		{
			FVector MoveVector;
			SetMoveVector(MoveVector);

			TargetLocation = PalCharacter->GetTargetLocation();
			Destination = GetNewDestination(MoveVector);

			PalCharacter->MoveToLocation(WalkSpeed, Destination);
			bIsMoving = true;
		}
		else
		{
			DirVec = Destination - PalCharacter->GetActorLocation();
			CurrStrafeDist = FMath::Sqrt(FVector::DotProduct(DirVec, DirVec));

			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("Remaining Strafe dist: %f"), CurrStrafeDist));
			DrawDebugLine(GetWorld(), PalCharacter->GetActorLocation(), PalCharacter->GetActorLocation() +
				DirVector * CurrStrafeDist, FColor::Black, false, 0.f);

			if (TargetLocation != PalCharacter->GetTargetLocation())
			{
				UpdateAction();
			}
		}
	}

	return EPalState::NONE;
}

void UMovementAction::Stop()
{
	Super::Stop();
	auto AI = PalCharacter->GetController<AAIController>();
	if (AI)
	{
		AI->StopMovement();

		if (AI->ReceiveMoveCompleted.IsBound())
		{
			AI->ReceiveMoveCompleted.Clear();
		}
	}

	bIsMoving = false;
	bIsSkillAvailable = false;
	SkillHandle.Invalidate();
}

void UMovementAction::UpdateAction()
{
	Super::UpdateAction();


	FVector MoveVector;
	SetMoveVector(MoveVector);
	TargetLocation = PalCharacter->GetTargetLocation();

	Destination = GetNewDestination(MoveVector);

	PalCharacter->MoveToLocation(WalkSpeed, Destination);
}

void UMovementAction::OnMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	switch (Result)
	{
	case EPathFollowingResult::Type::Success:
	{
		UE_LOG(LogTemp, Warning, TEXT("SUCCESS"));
		bIsMoving = false;
		break;
	}
	}
}

//Apply quaternion rotation to this later!
void UMovementAction::SetMoveVector(FVector& MoveVec)
{
	if (!PalCharacter) return;

	FVector DirVec = PalCharacter->GetTargetLocation() - PalCharacter->GetActorLocation();

	if (!bIsMoving)
	{
		if (FMath::RandRange(0, 1) == 1)
		{
			NegateVal = -1.f;
		}
		else NegateVal = 1.f;
	}

	UE_LOG(LogTemp, Warning, TEXT("StrafeAngle %f"), StrafeAngle);
	FVector NewDirVec = RotateVectorByAngle(DirVec, StrafeAngle * NegateVal);
	NewDirVec.Normalize();
	DirVector = NewDirVec;

	float NewDirDist = DirVec.X * NewDirVec.X + DirVec.Y * NewDirVec.Y + DirVec.Z * NewDirVec.Z;
	FVector MaxDirVec = NewDirVec * NewDirDist;
	FVector ToMaxDirVec = (PalCharacter->GetActorLocation() + MaxDirVec) - PalCharacter->GetTargetLocation();
	CurrStrafeDist = StrafeDistance;

	if (!bIsMoving)
	{
		switch (MovementType)
		{
		case EMovement::FAR:
		{
			CurrStrafeDist = StrafeDistance;
			break;
		}
		case EMovement::NEAR:
		{
			CurrStrafeDist = StrafeDistance / 1.5f;
			break;
		}
		}
	}
	MoveVec = NewDirVec * CurrStrafeDist;
}

bool UMovementAction::IsOutofSight() const
{
	float Dot = 1.f;
	if (PalCharacter)
	{
		FVector ToTargetVector = PalCharacter->GetTargetLocation() - PalCharacter->GetActorLocation();
		ToTargetVector.Normalize();

		Dot = FVector::DotProduct(DirVector, ToTargetVector);

	}
	return Dot < 0.5f;
}

FVector UMovementAction::GetNewDestination(const FVector& _MoveVector) const
{
	if (PalCharacter)
	{
		FVector NewLoc = PalCharacter->GetActorLocation() + _MoveVector;
		return NewLoc;
	}
	return FVector::Zero();
}

FVector UMovementAction::RotateVectorByAngle(const FVector& OriginalVector, float AngleDegrees)
{
	FQuat WorkingQuat = OriginalVector.Rotation().Quaternion();

	FQuat RotateQuat = FRotator(0.f, AngleDegrees, 0.f).Quaternion();

	FQuat ResultQuat = WorkingQuat * RotateQuat;


	return ResultQuat.GetForwardVector() * OriginalVector.Length();
}
