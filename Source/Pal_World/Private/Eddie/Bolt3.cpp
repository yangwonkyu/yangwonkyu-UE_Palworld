#include "Eddie/Bolt3.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Justin/Frameworks/PStatusLibrary.h"

// Sets default values
ABolt3::ABolt3()
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

void ABolt3::BeginPlay()
{
    Super::BeginPlay();
}

void ABolt3::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    UPStatusLibrary::ApplyDamage(GetOwner(), OtherActor, SweepResult.ImpactPoint, 200);
}

void ABolt3::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABolt3::InitializeProjectile(FVector LaunchDirection)
{
	/* if (ProjectileMovementComponent)
	 {
		 ProjectileMovementComponent->Velocity = LaunchDirection * ProjectileMovementComponent->InitialSpeed;
		 ProjectileMovementComponent->Activate();
	 }*/
}


void ABolt3::DestroyThisActor()
{
    Destroy();
}



