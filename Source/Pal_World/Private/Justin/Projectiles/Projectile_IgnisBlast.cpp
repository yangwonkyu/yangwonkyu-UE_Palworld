// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Projectiles/Projectile_IgnisBlast.h"
#include "Components/SphereComponent.h"
#include "Justin/Frameworks/PStatusLibrary.h"

AProjectile_IgnisBlast::AProjectile_IgnisBlast()
{
	TravelSpeed = 5000.f;
}

void AProjectile_IgnisBlast::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile_IgnisBlast::OnOverlapBegin);

	SetLifeSpan(10.f);
}

void AProjectile_IgnisBlast::OnOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin_Implementation(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	UE_LOG(LogTemp, Warning, TEXT("OVerlapS START %s"), *GetNameSafe(OtherActor));

	UPStatusLibrary::ApplyDamage(GetOwner(), OtherActor, SweepResult.ImpactPoint, 50.f);
}

void AProjectile_IgnisBlast::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector MoveVec = GetActorForwardVector()* TravelSpeed * DeltaTime;

	SetActorLocation(MoveVec + GetActorLocation());
}


