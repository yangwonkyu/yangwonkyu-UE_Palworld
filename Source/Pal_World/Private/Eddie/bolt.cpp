// Fill out your copyright notice in the Description page of Project Settings.


#include "Eddie/bolt.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Justin/Frameworks/PStatusLibrary.h"

// Sets default values
Abolt::Abolt()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);

    //ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    //ProjectileMovementComponent->InitialSpeed = 1000.f;
    //ProjectileMovementComponent->MaxSpeed = 1000.f;
    //ProjectileMovementComponent->bRotationFollowsVelocity = true;
    //ProjectileMovementComponent->bShouldBounce = fa;
    //ProjectileMovementComponent->ProjectileGravityScale = 0.f; // 중력 영향을 받지 않도록 설정
}

void Abolt::BeginPlay()
{
    Super::BeginPlay();
    //ProjectileMovementComponent->SetVelocityInLocalSpace(FVector(1000, 0.f, 0.f));


}

void Abolt::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void Abolt::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    UPStatusLibrary::ApplyDamage(GetOwner(), OtherActor, SweepResult.ImpactPoint, 240);
}

void Abolt::InitializeProjectile(FVector LaunchDirection)
{
	/*  if (ProjectileMovementComponent)
	  {
		  ProjectileMovementComponent->Velocity = LaunchDirection * ProjectileMovementComponent->InitialSpeed;
		  ProjectileMovementComponent->Activate();
	  }*/
}

void Abolt::DestroyThisActor()
{
    Destroy();
}




