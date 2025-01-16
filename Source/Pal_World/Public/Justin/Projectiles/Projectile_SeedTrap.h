// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Justin/BaseProjectile.h"
#include "Projectile_SeedTrap.generated.h"

/**
 *
 */

UENUM(Blueprintable)
enum class ESeedTrapState : uint8 
{
	LAUNCHED UMETA(DisplayName = "Launched"),
	DISPERSE UMETA(DisplayName = "Dispersed"),
	LANDED UMETA(DisplayName = "Landed")
};

class USoundBase;

UCLASS()
class PAL_WORLD_API AProjectile_SeedTrap : public ABaseProjectile
{
	GENERATED_BODY()

public:
	AProjectile_SeedTrap();

	void Initialize(AActor* _TargetActor, ESeedTrapState State, float _Magnitude = 0.f, bool _bIsMainSeed = false);
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void OnOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	UPROPERTY(EditAnywhere, Category = "Settings|Sound")
	TObjectPtr<USoundBase> SeedSound;
	UPROPERTY(EditAnywhere, Category = "Settings|Sound")
	TObjectPtr<USoundBase> SeedExplosionSound;

	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|MainSeed")
	int MainSeedSpawnTotal;
	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|MainSeed")
	float MainSeedDisperseSpeed;
	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|MainSeed")
	float MainSeedDisperseWeight;
	//Multiplier for SpawnDisperseSeeds() when main seed hits the ground.
	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|MainSeed")
	float MainSeedDisperseMultiplier;

	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|Launch")
	float LaunchSpeed;
	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|Launch")
	float LaunchWeight;

	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|Disperse")
	int DisperseSeedSpawnTotal;
	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|Disperse")
	float DisperseSpeed;
	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|Disperse")
	float DisperseSpeedOffset;
	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|Disperse")
	float DisperseWeight;
	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|Disperse")
	float DispersePitch;
	//How much to offset 2nd ring of seed trap.
	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|Disperse")
	float DispersePitchMultiplier;
	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|Disperse")
	float DisperseSpawnScale;

	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|Land")
	float CountDownDuration;
	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|Land")
	float CountDownDurationOffset;
	UPROPERTY(EditAnywhere, Category = "Settings|Projectile|MainSeed")
	float ExplosionRadius;

	UPROPERTY(EditAnywhere, Category = "Settings")
	TSubclassOf<AProjectile_SeedTrap> SeedTrapClass;

	UFUNCTION(BlueprintImplementableEvent)
	void Explode();

	UPROPERTY(BlueprintReadOnly)
	ESeedTrapState SeedTrapState;
private:
	void SpawnDisperseSeeds(float Multiplier = 1.f);

	void SetState(ESeedTrapState State);

	void CalculateAngle(float Speed, float ZLength, float XLength, float& angle)
	{
		float Gravity = -1.f * GetWorld()->GetDefaultGravityZ();

		float Numerator = FMath::Pow(Speed, 4) - Gravity * (Gravity * XLength * XLength + 2 * ZLength * FMath::Pow(Speed, 2));

		if (Numerator < 0.f)
		{
			angle = 0.0f;
			return;
		}

		Numerator = FMath::Sqrt(Numerator);

		float Denominator = Gravity * XLength;

		float GreaterAngle = FMath::Atan((FMath::Pow(Speed, 2) + Numerator) / Denominator);
		float LesserAngle = FMath::Atan((FMath::Pow(Speed, 2) - Numerator) / Denominator);

		angle = FMath::RadiansToDegrees(LesserAngle);
	}


	AActor* TargetActor;
	FVector TargetLocation;

	bool ehTick = false;
	float TotalTime;
	FVector Start;
	FVector End;
	FVector DirectionVec;

	bool bBeganCountDown;
	FTimerHandle LandedHandle;
	void StartExplosion();

	bool bIsMainSeed;
};
