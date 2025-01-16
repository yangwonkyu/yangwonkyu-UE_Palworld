// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ArrowComponent.h"
#include "GrizzBoltActorComponent.generated.h"

UENUM(Blueprintable)
enum class EGrizzBoltState : uint8
{
    IDLE UMETA(DisplayName = "Idle"),
    MOVE UMETA(DisplayName = "Move"),
    SKILL1 UMETA(DisplayName = "Skill1"),
    SKILL2 UMETA(DisplayName = "Skill2"),
    SKILL3 UMETA(DisplayName = "Skill3"),
    SKILL4 UMETA(DisplayName = "Skill4"),
    DAMAGE UMETA(DisplayName = "Damaged"),
    DIE UMETA(DisplayName = "Death"),
    FINDTARGET UMETA(DisplayName = "FindTarget")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PAL_WORLD_API UGrizzBoltActorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UGrizzBoltActorComponent();
    float Skill1CurrentTime;
    float Skill2CurrentTime;
    float Skill3CurrentTime;

protected:
    // Called when the game starts
    virtual void BeginPlay() override;
    
    //Binds to OnPalInteract Delegate in APCharacter
    void OnPalRetrieved(bool bIsSummon);

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    EGrizzBoltState CurrentState = EGrizzBoltState::IDLE;
    
    UFUNCTION(BlueprintCallable)
    AActor* FindTarget();

private:
    

    void TickIdle(const float& DeltaTime);
    void TickMove(const float& DeltaTime);
    void TickSkill1(const float& DeltaTime);
    void TickSkill2(const float& DeltaTime);
    void TickSkill3(const float& DeltaTime);
    void TickSkill4(const float& DeltaTime);
    void TickDAMAGE(const float& DeltaTime);
    void TickDIE(const float& DeltaTime);
    void TickFindTarget(const float& DeltaTime);

    UFUNCTION()
    void Test(FVector SpawnLocation, FRotator SpawnRotation);

    // 공격 가능 거리
    float AttackDistance = 1500;  // 범위를 1500으로 늘림
    float SkillDistance = 1500;    // 범위를 1500으로 늘림
    float DetectionRange = 11000;  // 범위를 11000으로 늘림
    float NearDistance = 300; // 플레이어와 가까운 거리 지정
    float MiddleDistance = 700; // 타겟한테서 다시 떨어질 거리
    
    
    float AttackWaitTime;

    //플레이어 발견 여부 
    bool bHasFoundTarget;
    // 타이머 핸들
    FTimerHandle TransitionTimerHandle;

public:
    bool bIsDead;

    // 목적지를 찾아야 한다.
    UPROPERTY(BlueprintReadOnly)
    class ACharacter* Target;

    UPROPERTY()
    class AGrizzBolt* Me;

    // 번개 클래스를 설정할 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<class AThunder> ThunderClass;

    // 번개가 떨어질 위치를 저장할 Arrow Component 배열
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TArray<UArrowComponent*> ArrowComponents;

    UPROPERTY()
    class UBoltAnimInstance* BoltAnimInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    class UAnimMontage* AM_GrizzBolt_Skill;

    // 번개를 생성하는 함수
    void SpawnThunderWithDelay(int32 Index, float Delay);

    // 한 번만 실행되도록 플래그 변수 추가
    bool bSkill1Executed;
    bool bSkill2Executed;
    bool bSkill3Executed;
    bool bSkill4Executed;

    // 몽타주 한 번만 실행되도록 변수 추가
    bool bMontageActive;

    // 타이머 핸들 추가
    FTimerHandle Skill1TimerHandle;

    // 타이머 핸들 배열
    TArray<FTimerHandle> TimerHandles;


    // 볼트 클래스를 설정할 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<class Abolt> BoltClass;

    // 볼트2를 설정할 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<class Abolt2> Bolt2Class;

    // 볼트3를 설정할 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<class ABolt3> Bolt3Class;

    // 플레이어를 발견했는지 여부를 설정하는 함수
    void SetHasFoundTarget(bool bFound);

    // 플레이어를 발견했는지 여부를 반환하는 함수
    bool HasFoundTarget() const;

    // 스킬 쿨다운 여부를 설정하는 변수
    bool bIsSkillCooldown;


    // 스킬 쿨다운을 초기화하는 함수
    void ResetSkillCooldown();

    // 스킬 쿨다운 시간
    float CooldownTime;

    // 스킬 쿨다운 타이머 핸들
    FTimerHandle SkillCooldownTimerHandle;

    // FINDTARGET -> MOVE 상태 전환을 위한 함수
    void TransitionToMoveState();

    // MOVE -> SKILL1 또는 SKILL2,SKILL3 상태 전환을 위한 함수
    void TransitionToSkillState();


    void ReturnToMoveStateFromSkill1();
    void ReturnToMoveStateFromSkill2();
    void ReturnToMoveStateFromSkill3();
    void ReturnToMoveStateFromSkill4();


    // 스킬2 타이머 핸들 추가
    FTimerHandle Skill2TimerHandle;

    // 스킬3 타이머 핸들 추가
    FTimerHandle Skill3CooldownHandle;


    UPROPERTY()
    class UBoltAnimInstance* Anim;

    // 조이가 상태를 따라가게 할 수 있게 blueprint로 넘기기
    UFUNCTION(BlueprintCallable, Category = "State")
    EGrizzBoltState GetCurrentState() const { return CurrentState; }

private:
    float CachedDistanceToTarget; // Target과의 거리를 저장할 변수

};
