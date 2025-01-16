// Fill out your copyright notice in the Description page of Project Settings.


#include "Eddie/LastBullet.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Justin/Frameworks/PStatusLibrary.h"

// Sets default values
ALastBullet::ALastBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetSphereRadius(13.0f);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetRelativeScale3D(FVector(0.25f, 0.5f, 0.15f));

	// 충돌 설정
	//SphereComp->SetCollisionProfileName(TEXT("BlockAll"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 발사체 컴포넌트 제거
	// MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	// MovementComp->SetUpdatedComponent(RootComponent);
	// MovementComp->InitialSpeed = 4000;
	// MovementComp->MaxSpeed = 5000;
	// MovementComp->bShouldBounce = true;
}

// Called when the game starts or when spawned
void ALastBullet::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ALastBullet::Die, 2.0f, false);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ALastBullet::OnOverlapBegin);
}

void ALastBullet::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UPStatusLibrary::ApplyDamage(GetOwner(), OtherActor, SweepResult.ImpactPoint, 1200);
	Destroy();

}

// Called every frame
void ALastBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation() + (GetActorForwardVector() * Speed * DeltaTime);
	SetActorLocation(NewLocation);

	// 현재 위치를 로그로 출력해 방향과 이동 여부 확인
	//UE_LOG(LogTemp, Log, TEXT("Bullet Location: %s"), *NewLocation.ToString());

}

void ALastBullet::Die()
{
	Destroy();
}

