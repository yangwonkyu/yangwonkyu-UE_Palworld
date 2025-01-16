// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/PBullet.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
APBullet::APBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetSphereRadius(13.0f);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetRelativeScale3D(FVector(0.25f));

	// 충돌 설정
	SphereComp->SetCollisionProfileName(TEXT("BlockAll"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	// 발사체 컴포넌트 설정
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->SetUpdatedComponent(RootComponent);

	MovementComp->InitialSpeed = 5000.f;    // 초기 속도
	MovementComp->MaxSpeed = 5000.f;        // 최대 속도
	MovementComp->bShouldBounce = true;     // 반동 여부
	MovementComp->Bounciness = 0.1f;		// 반동 값

}

// Called when the game starts or when spawned
void APBullet::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle deathTime;
	GetWorld()->GetTimerManager().SetTimer(deathTime, this, &APBullet::Die, 2.0f, false);
	
}

// Called every frame
void APBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APBullet::FireInDirection(const FVector& ShootDirection)
{
	UE_LOG(LogTemp, Warning, TEXT("FireInDirection()"));
	MovementComp->Velocity = ShootDirection * MovementComp->InitialSpeed;
}

void APBullet::Die()
{
	Destroy();
}

