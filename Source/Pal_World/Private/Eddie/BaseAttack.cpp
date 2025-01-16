// Fill out your copyright notice in the Description page of Project Settings.


#include "Eddie/BaseAttack.h"
#include "Components/CapsuleComponent.h"
// Sets default values
ABaseAttack::ABaseAttack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 박스 컴포넌트 추가
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComp;
	CapsuleComp->SetCollisionProfileName("BossAttack");
}

// Called when the game starts or when spawned
void ABaseAttack::BeginPlay()
{
	Super::BeginPlay();

	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseAttack::OnOverlapBegin);
}

void ABaseAttack::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CapsuleComp->OnComponentBeginOverlap.Clear();
}
