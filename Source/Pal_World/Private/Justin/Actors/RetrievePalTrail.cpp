// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Actors/RetrievePalTrail.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"

ARetrievePalTrail::ARetrievePalTrail()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComp = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	RootComponent = SceneComp;
	SplineComp = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComp->SetupAttachment(RootComponent);
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	NiagaraComp->SetupAttachment(SplineComp);
}

void ARetrievePalTrail::Initialize(AActor* StartActor, AActor* EndActor)
{
	Start = StartActor;
	End = EndActor;

	if (Start && End)
	{
		SetActorLocation(Start->GetActorLocation());
		int32 points = SplineComp->GetNumberOfSplinePoints();
		UE_LOG(LogTemp, Warning, TEXT("Points:%d "), points);
		SplineComp->SetLocationAtSplinePoint(points - 1, End->GetActorLocation(), ESplineCoordinateSpace::World);
	}

	//FVector Mid = End->GetActorLocation() - Start->GetActorLocation();

	//SplineComp->AddSplinePoint(Mid + Start->GetActorLocation(), ESplineCoordinateSpace::World, false);
}

void ARetrievePalTrail::Update()
{
	NiagaraComp->SetActive(true, true);
}

void ARetrievePalTrail::BeginPlay()
{
	Super::BeginPlay();

}

void ARetrievePalTrail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}

