#include "Eddie/bolt2.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Justin/Frameworks/PStatusLibrary.h"

// Sets default values
Abolt2::Abolt2()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    ProjectileMovementComponent->InitialSpeed = 800.f;
    ProjectileMovementComponent->MaxSpeed = 800.f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = true;
    ProjectileMovementComponent->bIsHomingProjectile = true;
    ProjectileMovementComponent->HomingAccelerationMagnitude = 4000;
    ProjectileMovementComponent->ProjectileGravityScale = 0.f; // 중력 영향을 받지 않도록 설정
}

void Abolt2::BeginPlay()
{
    Super::BeginPlay();
}

void Abolt2::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    UPStatusLibrary::ApplyDamage(GetOwner(), OtherActor, SweepResult.ImpactPoint, 200);
}

void Abolt2::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void Abolt2::InitializeProjectile(FVector LaunchDirection)
{
	/*if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->Velocity = LaunchDirection * ProjectileMovementComponent->InitialSpeed;
		ProjectileMovementComponent->Activate();
	}*/
}


void Abolt2::DestroyThisActor()
{
    Destroy();
}

