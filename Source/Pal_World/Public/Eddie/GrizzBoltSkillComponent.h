// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ArrowComponent.h"
#include "Eddie/GrizzBolt.h"
#include "Eddie/GrizzBoltActorComponent.h"
#include "GrizzBoltSkillComponent.generated.h"


// ���� ����
class UGrizzBoltActorComponent;
class APCharacter;
class AGrizzBolt;


UENUM(Blueprintable)
enum class Skill1 : uint8
{
	SKILL1 UMETA(DisplayName = "LineThunder")
};

UENUM(Blueprintable)
enum class Skill2 : uint8
{
	SKILL2 UMETA(DisplayName = "TripleSpark")
};

UENUM(Blueprintable)
enum class Skill3 : uint8
{
    SKILL3 UMETA(DisplayName = "ElectricBomb")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAL_WORLD_API UGrizzBoltSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrizzBoltSkillComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AM_GrizzBolt_Skill;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ��ų1�� state
	void TickLineThunder(const float& DeltaTime);

	// ��ų2�� state
	void TickTripleSpark(const float& DeltaTime);


    // ��ų3�� state
    void TickElectricBomb(const float& DeltaTime);

    // ��ų4�� state
    void TickElectricMinigun(const float& DeltaTime);

    


public:

    // ���� ���� �Ÿ�
    float AttackDistance = 1000;  // ������ 1000���� �ø�
    float SkillDistance = 1000;    // ������ 600���� �ø�
    float ExplosionDistance = 500; // ���߹��� 500����
	
	UPROPERTY(BlueprintReadOnly)
    UGrizzBoltActorComponent* FSM;

    UPROPERTY()
    AGrizzBolt* Me;

 

    UPROPERTY()
    UBoltAnimInstance* BoltAnimInstance;

	
	// Sets default values for this component's properties
	float Skill1CurrentTime;
	float Skill2CurrentTime;
    float Skill3CurrentTime;
    float Skill4CurrentTime;

	// ���� Ŭ������ ������ ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<class AThunder> ThunderClass;

    // ������ ������ ��ġ�� ������ Arrow Component �迭
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    AGrizzBolt* ArrowComponents;

    // ������ �����ϴ� �Լ�
    void SpawnThunderWithDelay(int32 Index, float Delay);

    UPROPERTY()
    // ���� ���߹��� �Լ�
    AGrizzBolt* RadialComp;

    // �� ���� ����ǵ��� �÷��� ���� �߰�
    bool bSkill1Executed;
    bool bSkill2Executed;
    bool bSkill3Executed;
    bool bSkill4Executed;
    bool bAnimMontage;
    

    // �̴ϰ� �߻� �ð�
    float MinigunShootTimer;
    
    // ���Ƽ� �ѹ� �߻� �ð�
    float LastShootTimer;

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

    // �Ѿ� ����
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class AGrizzBullet> BulletFactory;

    // �Ѿ� ����2
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class ALastBullet> LastFactory;


    //���ǰ���
    class UAudioComponent* Audio;

    UPROPERTY(EditAnywhere)
    USoundBase* Sound_MachineGun;

    UPROPERTY(EditAnywhere)
    USoundBase* Sound_MachineGun_Last;

    UPROPERTY(EditAnywhere)
    USoundBase* Sound_TripleBolt;

    UPROPERTY(EditAnywhere)
    USoundBase* Sound_LineThunder;

    // ��ų ��ٿ� ���θ� �����ϴ� ����
    bool bIsSkillCooldown;

    // ��ų ��ٿ��� �ʱ�ȭ�ϴ� �Լ�
    void ResetSkillCooldown();

    // ��ų ��ٿ� �ð�
    float CooldownTime;

    // ��ų ��ٿ� Ÿ�̸� �ڵ�
    FTimerHandle SkillCooldownTimerHandle;


    // ��ų2 Ÿ�̸� �ڵ� �߰�
    FTimerHandle Skill2TimerHandle;

    // ��ų3 Ÿ�̸� �ڵ� �߰�
    FTimerHandle Skill3TimerHandle;

    // ��ų4 Ÿ�̸� �ڵ� �߰�
    FTimerHandle Skill4TimerHandle;

    void ReturnToMoveStateFromSkill1();
    void ReturnToMoveStateFromSkill2();
    void ReturnToMoveStateFromSkill3();
    void ReturnToMoveStateFromSkill4();

};
