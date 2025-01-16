// Fill out your copyright notice in the Description page of Project Settings.


#include "Eddie/Thunder.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "Justin/Frameworks/PStatusLibrary.h"

// Sets default values
AThunder::AThunder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	if (tempMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(tempMesh.Object);
	}
	MeshComp->SetRelativeScale3D(FVector(1.4f, 1.4f, 3.0f));

	// 스피드 값
	Speed = 3000.0f;
}

// Called when the game starts or when spawned
void AThunder::BeginPlay()
{
	Super::BeginPlay();

	//1.0초 지나면 파괴하는 함수
	GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &AThunder::DestroyAfterDelay, 1.0f, false);
}

void AThunder::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	UPStatusLibrary::ApplyDamage(GetOwner(), OtherActor, SweepResult.ImpactPoint, 500);
}

// Called every frame
void AThunder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 위치
	FVector CurrentLocation = GetActorLocation();

	// 아래로 가게 하기
	FVector UpVector = GetActorUpVector() * -1.0f;

	// 새 위치 계산하기
	FVector NewLocation = CurrentLocation + (UpVector * Speed * DeltaTime);

	// 새 위치로 설정
	SetActorLocation(NewLocation);
}

void AThunder::DestroyAfterDelay()
{
	// 파괴되게 하기.
	Destroy();
}


