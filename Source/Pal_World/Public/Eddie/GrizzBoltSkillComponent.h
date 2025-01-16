// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ArrowComponent.h"
#include "Eddie/GrizzBolt.h"
#include "Eddie/GrizzBoltActorComponent.h"
#include "GrizzBoltSkillComponent.generated.h"


// 전방 선언
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

	// 스킬1의 state
	void TickLineThunder(const float& DeltaTime);

	// 스킬2의 state
	void TickTripleSpark(const float& DeltaTime);


    // 스킬3의 state
    void TickElectricBomb(const float& DeltaTime);

    // 스킬4의 state
    void TickElectricMinigun(const float& DeltaTime);

    


public:

    // 공격 가능 거리
    float AttackDistance = 1000;  // 범위를 1000으로 늘림
    float SkillDistance = 1000;    // 범위를 600으로 늘림
    float ExplosionDistance = 500; // 폭발범위 500으로
	
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

	// 번개 클래스를 설정할 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<class AThunder> ThunderClass;

    // 번개가 떨어질 위치를 저장할 Arrow Component 배열
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    AGrizzBolt* ArrowComponents;

    // 번개를 생성하는 함수
    void SpawnThunderWithDelay(int32 Index, float Delay);

    UPROPERTY()
    // 전기 폭발범위 함수
    AGrizzBolt* RadialComp;

    // 한 번만 실행되도록 플래그 변수 추가
    bool bSkill1Executed;
    bool bSkill2Executed;
    bool bSkill3Executed;
    bool bSkill4Executed;
    bool bAnimMontage;
    

    // 미니건 발사 시간
    float MinigunShootTimer;
    
    // 기모아서 한방 발사 시간
    float LastShootTimer;

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

    // 총알 공장
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class AGrizzBullet> BulletFactory;

    // 총알 공장2
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class ALastBullet> LastFactory;


    //음악공장
    class UAudioComponent* Audio;

    UPROPERTY(EditAnywhere)
    USoundBase* Sound_MachineGun;

    UPROPERTY(EditAnywhere)
    USoundBase* Sound_MachineGun_Last;

    UPROPERTY(EditAnywhere)
    USoundBase* Sound_TripleBolt;

    UPROPERTY(EditAnywhere)
    USoundBase* Sound_LineThunder;

    // 스킬 쿨다운 여부를 설정하는 변수
    bool bIsSkillCooldown;

    // 스킬 쿨다운을 초기화하는 함수
    void ResetSkillCooldown();

    // 스킬 쿨다운 시간
    float CooldownTime;

    // 스킬 쿨다운 타이머 핸들
    FTimerHandle SkillCooldownTimerHandle;


    // 스킬2 타이머 핸들 추가
    FTimerHandle Skill2TimerHandle;

    // 스킬3 타이머 핸들 추가
    FTimerHandle Skill3TimerHandle;

    // 스킬4 타이머 핸들 추가
    FTimerHandle Skill4TimerHandle;

    void ReturnToMoveStateFromSkill1();
    void ReturnToMoveStateFromSkill2();
    void ReturnToMoveStateFromSkill3();
    void ReturnToMoveStateFromSkill4();

};
