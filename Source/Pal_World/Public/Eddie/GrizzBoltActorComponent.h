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

    // ���� ���� �Ÿ�
    float AttackDistance = 1500;  // ������ 1500���� �ø�
    float SkillDistance = 1500;    // ������ 1500���� �ø�
    float DetectionRange = 11000;  // ������ 11000���� �ø�
    float NearDistance = 300; // �÷��̾�� ����� �Ÿ� ����
    float MiddleDistance = 700; // Ÿ�����׼� �ٽ� ������ �Ÿ�
    
    
    float AttackWaitTime;

    //�÷��̾� �߰� ���� 
    bool bHasFoundTarget;
    // Ÿ�̸� �ڵ�
    FTimerHandle TransitionTimerHandle;

public:
    bool bIsDead;

    // �������� ã�ƾ� �Ѵ�.
    UPROPERTY(BlueprintReadOnly)
    class ACharacter* Target;

    UPROPERTY()
    class AGrizzBolt* Me;

    // ���� Ŭ������ ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<class AThunder> ThunderClass;

    // ������ ������ ��ġ�� ������ Arrow Component �迭
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TArray<UArrowComponent*> ArrowComponents;

    UPROPERTY()
    class UBoltAnimInstance* BoltAnimInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    class UAnimMontage* AM_GrizzBolt_Skill;

    // ������ �����ϴ� �Լ�
    void SpawnThunderWithDelay(int32 Index, float Delay);

    // �� ���� ����ǵ��� �÷��� ���� �߰�
    bool bSkill1Executed;
    bool bSkill2Executed;
    bool bSkill3Executed;
    bool bSkill4Executed;

    // ��Ÿ�� �� ���� ����ǵ��� ���� �߰�
    bool bMontageActive;

    // Ÿ�̸� �ڵ� �߰�
    FTimerHandle Skill1TimerHandle;

    // Ÿ�̸� �ڵ� �迭
    TArray<FTimerHandle> TimerHandles;


    // ��Ʈ Ŭ������ ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<class Abolt> BoltClass;

    // ��Ʈ2�� ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<class Abolt2> Bolt2Class;

    // ��Ʈ3�� ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<class ABolt3> Bolt3Class;

    // �÷��̾ �߰��ߴ��� ���θ� �����ϴ� �Լ�
    void SetHasFoundTarget(bool bFound);

    // �÷��̾ �߰��ߴ��� ���θ� ��ȯ�ϴ� �Լ�
    bool HasFoundTarget() const;

    // ��ų ��ٿ� ���θ� �����ϴ� ����
    bool bIsSkillCooldown;


    // ��ų ��ٿ��� �ʱ�ȭ�ϴ� �Լ�
    void ResetSkillCooldown();

    // ��ų ��ٿ� �ð�
    float CooldownTime;

    // ��ų ��ٿ� Ÿ�̸� �ڵ�
    FTimerHandle SkillCooldownTimerHandle;

    // FINDTARGET -> MOVE ���� ��ȯ�� ���� �Լ�
    void TransitionToMoveState();

    // MOVE -> SKILL1 �Ǵ� SKILL2,SKILL3 ���� ��ȯ�� ���� �Լ�
    void TransitionToSkillState();


    void ReturnToMoveStateFromSkill1();
    void ReturnToMoveStateFromSkill2();
    void ReturnToMoveStateFromSkill3();
    void ReturnToMoveStateFromSkill4();


    // ��ų2 Ÿ�̸� �ڵ� �߰�
    FTimerHandle Skill2TimerHandle;

    // ��ų3 Ÿ�̸� �ڵ� �߰�
    FTimerHandle Skill3CooldownHandle;


    UPROPERTY()
    class UBoltAnimInstance* Anim;

    // ���̰� ���¸� ���󰡰� �� �� �ְ� blueprint�� �ѱ��
    UFUNCTION(BlueprintCallable, Category = "State")
    EGrizzBoltState GetCurrentState() const { return CurrentState; }

private:
    float CachedDistanceToTarget; // Target���� �Ÿ��� ������ ����

};
