#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BoltAnimInstance.generated.h"

enum class EGrizzBoltState : uint8;

UCLASS()
class PAL_WORLD_API UBoltAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UGrizzBoltActorComponent* GrizzBoltActorComponent;

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    virtual void NativeInitializeAnimation() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float Speed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float Direction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    bool FindTarget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float DistanceToPlayer;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    bool IsSkillCooldown;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EGrizzBoltState state;

    UPROPERTY(BlueprintReadOnly)
    class UGrizzBoltActorComponent* FSM;

    UPROPERTY()
    class AGrizzBolt* Me;

    UPROPERTY()
    class URadialForceComponent* RadialComp;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float CurrentTime;

    UPROPERTY(EditDefaultsOnly)
    class UAnimMontage* AM_GrizzBolt_Skill;

    void SetFindTarget(bool bTarget);

    UFUNCTION()
    void AnimNotify_BeginPre();
    UFUNCTION()
    void AnimNotify_BeginAttack();
    UFUNCTION()
    void AnimNotify_EndState();
    UFUNCTION()
    void AnimNotify_StartTime();

    bool bIsElectricBomb;
};
