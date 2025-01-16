#pragma once

#include "CoreMinimal.h"
#include "Eddie/BaseAttack.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Bolt3.generated.h"

UCLASS()
class PAL_WORLD_API ABolt3 : public ABaseAttack
{
    GENERATED_BODY()
    
public:    
    // Sets default values for this actor's properties
    ABolt3();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

public:    
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere)
    class UStaticMeshComponent* MeshComp;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    class UProjectileMovementComponent* ProjectileMovementComponent;

    void InitializeProjectile(FVector Direction);

private:
    void DestroyThisActor();
    FTimerHandle TimerHandle;
};


