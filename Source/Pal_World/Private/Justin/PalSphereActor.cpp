// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/PalSphereActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Sunny/PCharacter.h"
#include "Sunny/PalSlotUI.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"

APalSphereActor::APalSphereActor()
{
	PrimaryActorTick.bCanEverTick = true;
	FlightSpeed = 800.f;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	RootComponent = SphereComp;
	SphereComp->SetCollisionProfileName("PalSphere");

	SMComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComp");
	SMComp->SetupAttachment(RootComponent);
	SMComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NiagaraComp_Trail = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComp_Trail");
	NiagaraComp_Trail->SetupAttachment(RootComponent);
	NiagaraComp_Trail->SetAutoActivate(false);
}

void APalSphereActor::Initialize(UPalSlotUI* _PalSlotUI)
{
	PalSlotUI = _PalSlotUI;
}

void APalSphereActor::BeginPlay()
{
	Super::BeginPlay();
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &APalSphereActor::OnOverlapBegin);

	DirectionVec = GetActorForwardVector();
	Start = GetActorLocation();

	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APalSphereActor::BeginThrow()
{
	NiagaraComp_Trail->SetActive(true, true);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void APalSphereActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PalSlotUI)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SummonSound, GetActorLocation());

		PalSlotUI->SpawnSelectedPal(GetActorLocation());
	}

	if (GetOwner())
	{
		auto Player = GetOwner<APCharacter>();
		if (Player)
		{
			Player->OnPalInteract.Broadcast(true);
		}
	}

	Destroy();
}

void APalSphereActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SetActorLocation(GetActorLocation() + GetActorForwardVector() * FlightSpeed * DeltaTime);

	Time += DeltaTime;
	float gravity = 0.5f * -GetWorld()->GetDefaultGravityZ() *  Time * Time;

	FVector NewLocation = Start + DirectionVec * FlightSpeed * Time - FVector(0, 0, gravity);
	SetActorLocation(NewLocation);
}

