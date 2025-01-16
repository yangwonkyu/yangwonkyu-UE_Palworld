// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Skills/AnubisSkill_SideStep.h"

UAnubisSkill_SideStep::UAnubisSkill_SideStep()
{
	SideStepDistance = 500.f;
	MaxTime = .09f;
	CurrentTime = 0.f;
	Probability = .5f;
}

void UAnubisSkill_SideStep::StartInit(AActor* _Owner)
{
	Owner = _Owner;
	if (Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("YES"));
	}
	else UE_LOG(LogTemp, Warning, TEXT("Why is this happening?"));
}

bool UAnubisSkill_SideStep::StartSideStep(AActor* Target)
{
	if (FMath::RandRange(0.f, 1.f) < Probability)
	{
		if (!Owner || !Target) return false;
		
		bSideStepping = true;

		FVector Loc;
		if (!Target) Loc = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 100.f;
		else Loc = Target->GetActorLocation();

		Loc.Z = Owner->GetActorLocation().Z;

		FVector DirectionVector = Loc - Owner->GetActorLocation();
		DirectionVector.Normalize();

		//FQuat Rot = Owner->GetActorRotation().Quaternion();
		DirectionVector = FVector::CrossProduct(DirectionVector, Owner->GetActorUpVector());
		//FQuat RotResult = Rot * Rotate;

		FVector DistanceVector = DirectionVector * SideStepDistance;

		Start = Owner->GetActorLocation();
		float Rand = FMath::RandRange(1, 10) <= 5 ? -1.f : 1.f;
		End = Start + DistanceVector * Rand;

		DrawDebugSphere(GetWorld(), Start, 60.f, 10, FColor::White, false, 2.f);
		DrawDebugSphere(GetWorld(), End, 60.f, 10, FColor::Red, false, 2.f);
		return true;
	}

	return false;
}

void UAnubisSkill_SideStep::Tick(float DeltaTime)
{
	if (!Owner) return;

	CurrentTime += DeltaTime;
	if (CurrentTime < MaxTime)
	{
		FVector Location = FMath::Lerp(Start, End, CurrentTime / MaxTime);

		Owner->SetActorLocation(Location);
	}
	else
	{
		Owner->SetActorLocation(End);
		CurrentTime = 0.f;
		bSideStepping = false;
		OnSideStepFinished.ExecuteIfBound();
	}
}

TStatId UAnubisSkill_SideStep::GetStatId() const
{
	return TStatId();
}

bool UAnubisSkill_SideStep::IsAllowedToTick() const
{
	return bSideStepping;
}
