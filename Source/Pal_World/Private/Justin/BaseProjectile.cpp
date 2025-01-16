// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/BaseProjectile.h"
#include "Components/SphereComponent.h"

ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetupAttachment(RootComponent);
	SphereComp->SetHiddenInGame(true);

	SphereComp->SetCollisionProfileName("Projectile");
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ABaseProjectile::OnOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ABaseProjectile::OnPalRetrieved()
{

}

void ABaseProjectile::Activate()
{
	//Start Niagara Particle system.
}

void ABaseProjectile::Fire()
{
	if (!SphereComp->OnComponentBeginOverlap.IsBound())
	{
		SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnOverlapBegin);
	}
	SetActorTickEnabled(true);
}

void ABaseProjectile::Despawn()
{
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(2.f);
}

