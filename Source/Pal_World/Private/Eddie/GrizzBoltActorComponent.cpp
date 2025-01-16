// Fill out your copyright notice in the Description page of Project Settings.

#include "Eddie/GrizzBoltActorComponent.h"
#include "Sunny/PCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "TimerManager.h"
#include "Eddie/Thunder.h"
#include "Eddie/GrizzBolt.h"
#include "Eddie/bolt.h"
#include "Eddie/bolt2.h"
#include "Eddie/Bolt3.h"
#include "Justin/Components/PStatusComponent.h"
#include "Eddie/GrizzBoltSkillComponent.h"
#include "Eddie/BoltAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values for this component's properties
UGrizzBoltActorComponent::UGrizzBoltActorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    bIsDead = false; // 초기화
    bSkill1Executed = false; // 플래그 초기화 
    bIsSkillCooldown = false; // 스킬 쿨다운 초기화
    CooldownTime = 5.0f; // 스킬 쿨다운 시간 설정
    Skill1CurrentTime = 0.0f; // Skill1 타이머 초기화
    Skill2CurrentTime = 0.0f; // Skill2 타이머 초기화
    bSkill3Executed = false; // SKILL3 상태 실행 여부 초기화

}

void UGrizzBoltActorComponent::BeginPlay()
{
    Super::BeginPlay();

    // Me 객체 초기화
    Me = Cast<AGrizzBolt>(GetOwner());
    if (!Me)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast to AGrizzBolt in BeginPlay"));
        return;
    }

    if (Me->WidgetComponent)
    {
        Me->WidgetComponent->SetVisibility(false);  // 기본적으로 UI를 숨김
    }


    // 목적지를 찾아야 한다.
    AActor* Result = FindTarget();
    if (Result)
    {
        Target = Cast<ACharacter>(Result);

        auto Player = Cast<APCharacter>(Target);
        if (Player)
        {
            Player->OnPalInteract.AddUObject(this, &UGrizzBoltActorComponent::OnPalRetrieved);
        }
    }

    if (!Target || !Me)
    {
        UE_LOG(LogTemp, Error, TEXT("Target or Me is Null in BeginPlay"));
        return;
    }

    if (Me->GetMesh())
    {
        UAnimInstance* AnimInstance = Me->GetMesh()->GetAnimInstance();
        BoltAnimInstance = Cast<UBoltAnimInstance>(AnimInstance);
        if (BoltAnimInstance)
        {
            AM_GrizzBolt_Skill = BoltAnimInstance->AM_GrizzBolt_Skill;
            UE_LOG(LogTemp, Log, TEXT("Successfully initialized BoltAnimInstance and AM_GrizzBolt_Skill"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to cast to UBoltAnimInstance in BeginPlay"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Me->GetMesh() returned null in BeginPlay"));
    }

    // 플래그 초기화
    bMontageActive = false;

    // Arrow Components 배열 초기화
    for (int32 i = 3; i <= 9; i++)
    {
        if (Me->ArrowComponents.IsValidIndex(i))
        {
            ArrowComponents.Add(Me->ArrowComponents[i]);
            UE_LOG(LogTemp, Log, TEXT("ArrowComponent %d Location: %s"), i, *Me->ArrowComponents[i]->GetComponentLocation().ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ArrowComponent %d is not valid"), i);
        }
    }

    // 애니메이션 가져오기
    Anim = Cast<UBoltAnimInstance>(Me->GetMesh()->GetAnimInstance());
}

void UGrizzBoltActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!Target || !Me)
    {
        UE_LOG(LogTemp, Error, TEXT("Target or Me is Null in TickComponent"));
        return;
    }

    // 일렉판다 상태 글씨로 표시
    /*FString boltState = UEnum::GetValueAsString(CurrentState);
    DrawDebugString(GetWorld(), GetOwner()->GetActorLocation(), boltState, nullptr, FColor::Purple, 0);*/

    if (!Target || !Me)
    {
        UE_LOG(LogTemp, Error, TEXT("Target or Me is Null in TickComponent"));
        return;
    }

    // 캐릭터 상태에 따라 UI 가시성 조절
    if (Me->WidgetComponent)
    {
        if (CurrentState == EGrizzBoltState::FINDTARGET)
        {
            Me->WidgetComponent->SetVisibility(true);  // FINDTARGET 상태일 때 UI 표시
        }
        else
        {
            Me->WidgetComponent->SetVisibility(false);  // 다른 상태일 때 UI 숨김
        }
    }

    // 프로토 보스 상태
    switch (CurrentState)
    {
    case EGrizzBoltState::IDLE:
        TickIdle(DeltaTime);
        break;
    case EGrizzBoltState::FINDTARGET:
        TickFindTarget(DeltaTime);
        break;
    case EGrizzBoltState::MOVE:
        TickMove(DeltaTime);
        break;
    case EGrizzBoltState::SKILL1:
        TickSkill1(DeltaTime);
        break;
    case EGrizzBoltState::SKILL2:
        TickSkill2(DeltaTime);
        break;
    case EGrizzBoltState::SKILL3:
        UE_LOG(LogTemp, Log, TEXT("Calling TickSkill3"));
        TickSkill3(DeltaTime); // SKILL3 상태일 때 반복 호출 확인
        break;
    case EGrizzBoltState::SKILL4:
        TickSkill4(DeltaTime);
        break;
    case EGrizzBoltState::DAMAGE:
        TickDAMAGE(DeltaTime);
        break;
    case EGrizzBoltState::DIE:
        TickDIE(DeltaTime);
        break;
    }
}

void UGrizzBoltActorComponent::OnPalRetrieved(bool bIsSummon)
{
    if (!bIsSummon)
    {
        UE_LOG(LogTemp, Warning, TEXT("Pal has been Retrieved, finding new target in GrizzBolt FSMComp"));
        Target = Cast<ACharacter>(FindTarget());
    }
}

AActor* UGrizzBoltActorComponent::FindTarget()
{
    TArray<AActor*> Characters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), Characters);

    AActor* ClosestActor = nullptr; // 초기화
    float MinDist = FLT_MAX; // 초기화
    for (int i = 0; i < Characters.Num(); ++i)
    {
        if (Characters[i] == GetOwner()) continue;

        auto Status = Characters[i]->GetComponentByClass<UPStatusComponent>();
        
        if (Status)
        {
            if (!Status->IsInitialized())
            {
                ClosestActor = Characters[i];
                break;
            }
            else if(Status->IsDead()) continue;
        }

        FVector Start = GetOwner()->GetActorLocation();
        FVector End = Characters[i]->GetActorLocation();

        float Dist = (End - Start).Size();

        if (Dist < MinDist)
        {
            ClosestActor = Characters[i];
            MinDist = Dist;
        }
    }

    if (ClosestActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Closest Actor for GrizzBolt: %s"), *GetNameSafe(ClosestActor));
        return ClosestActor;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No closest actor found for GrizzBolt"));
    }
    return nullptr;
}

void UGrizzBoltActorComponent::TickIdle(const float& DeltaTime)
{
    if (Target && Me)
    {
        float DistanceToPlayer = FVector::Dist(Me->GetActorLocation(), Target->GetActorLocation());
        if (DistanceToPlayer <= DetectionRange)
        {
            SetHasFoundTarget(true);
            CurrentState = EGrizzBoltState::FINDTARGET;
            UE_LOG(LogTemp, Log, TEXT("State changed to FINDTARGET"));
            Skill1CurrentTime = 0.0f;  // 타이머 초기화
            Skill2CurrentTime = 0.0f;  // 타이머 초기화
        }
        else
        {
            SetHasFoundTarget(false);
        }
    }
}

void UGrizzBoltActorComponent::TickFindTarget(const float& DeltaTime)
{
    // 타이머 업데이트
    Skill1CurrentTime += DeltaTime;
    Skill2CurrentTime += DeltaTime;
    if (Skill1CurrentTime >= 2.0f || Skill2CurrentTime >= 2.0f)
    {
        CurrentState = EGrizzBoltState::MOVE;
        UE_LOG(LogTemp, Log, TEXT("State changed to MOVE from FINDTARGET"));
        Skill1CurrentTime = 0.0f;
        Skill2CurrentTime = 0.0f;
    }
}

void UGrizzBoltActorComponent::TransitionToMoveState()
{
    if (CurrentState == EGrizzBoltState::FINDTARGET) // 상태가 FindTarget인지 확인
    {
        CurrentState = EGrizzBoltState::MOVE;
        UE_LOG(LogTemp, Log, TEXT("State changed to MOVE from FINDTARGET"));
    }
}

void UGrizzBoltActorComponent::TransitionToSkillState()
{
    if (CurrentState == EGrizzBoltState::MOVE && !bIsSkillCooldown) // 상태가 Move이고 쿨다운이 아닌 경우
    {
        if (CurrentState == EGrizzBoltState::SKILL3 && CachedDistanceToTarget > MiddleDistance)
        {
            UE_LOG(LogTemp, Warning, TEXT("Attempted to use Skill3 outside of MiddleDistance. Cancelling transition."));
            return;
        }

        int32 RandomSkill = FMath::RandRange(0, 3); // 0, 1, 2, 3 중 랜덤 선택
        if (RandomSkill == 0)
        {
            CurrentState = EGrizzBoltState::SKILL1;
            UE_LOG(LogTemp, Log, TEXT("State changed to SKILL1"));
        }
        else if (RandomSkill == 1)
        {
            CurrentState = EGrizzBoltState::SKILL2;
            UE_LOG(LogTemp, Log, TEXT("State changed to SKILL2"));
        }
        else if (RandomSkill == 2)
        {
            // SKILL3이 선택되면 MiddleDistance에서만 발동하도록 확인
            if (CachedDistanceToTarget <= MiddleDistance)
            {
                CurrentState = EGrizzBoltState::SKILL3;
                UE_LOG(LogTemp, Log, TEXT("State changed to SKILL3, within MiddleDistance."));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Attempted to use Skill3 outside of MiddleDistance. Cancelling transition."));
                return;
            }
        }
        else
        {
            CurrentState = EGrizzBoltState::SKILL4;
            UE_LOG(LogTemp, Log, TEXT("State changed to SKILL4"));
        }

        // 스킬 쿨다운 시작
        bIsSkillCooldown = true;
        GetWorld()->GetTimerManager().SetTimer(SkillCooldownTimerHandle, this, &UGrizzBoltActorComponent::ResetSkillCooldown, CooldownTime, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TransitionToSkillState called, but CurrentState is not MOVE or is in cooldown"));
    }
}


void UGrizzBoltActorComponent::ReturnToMoveStateFromSkill1()
{
    AActor* NewTarget = FindTarget();
    if (NewTarget) Target = Cast<ACharacter>(NewTarget);

    CurrentState = EGrizzBoltState::MOVE;
    UE_LOG(LogTemp, Log, TEXT("State changed back to MOVE from SKILL1"));
}

void UGrizzBoltActorComponent::ReturnToMoveStateFromSkill2()
{
    AActor* NewTarget = FindTarget();
    if (NewTarget) Target = Cast<ACharacter>(NewTarget);

    CurrentState = EGrizzBoltState::MOVE;
    UE_LOG(LogTemp, Log, TEXT("State changed back to MOVE from SKILL2"));
}

void UGrizzBoltActorComponent::ReturnToMoveStateFromSkill3()
{
    AActor* NewTarget = FindTarget();
    if (NewTarget) Target = Cast<ACharacter>(NewTarget);

    CurrentState = EGrizzBoltState::MOVE;
    bSkill3Executed = false; // SKILL3 상태가 완료된 후 플래그를 리셋
    UE_LOG(LogTemp, Log, TEXT("State changed back to MOVE from SKILL3"));
}

void UGrizzBoltActorComponent::ReturnToMoveStateFromSkill4()
{
    AActor* NewTarget = FindTarget();
    if (NewTarget) Target = Cast<ACharacter>(NewTarget);

    CurrentState = EGrizzBoltState::MOVE;
    bSkill4Executed = false; // SKILL4 상태가 완료된 후 플래그를 리셋
    UE_LOG(LogTemp, Log, TEXT("State changed back to MOVE from SKILL4"));
}


void UGrizzBoltActorComponent::TickMove(const float& DeltaTime)
{
    if (Target && Me)
    {
        FVector DirToTarget = Target->GetActorLocation() - Me->GetActorLocation();
        CachedDistanceToTarget = DirToTarget.Size(); // 거리를 캐싱합니다.

        // 만약 거리가 DetectionRange를 벗어나면 IDLE 상태로 전환
        if (CachedDistanceToTarget > DetectionRange)
        {
            CurrentState = EGrizzBoltState::IDLE;
            UE_LOG(LogTemp, Log, TEXT("Target is out of DetectionRange. State changed to IDLE."));
            return;
        }

        static bool bHasUsedSkill = false;   // 스킬을 사용했는지 여부
        static bool bIsBackingAway = false;  // 타겟에서 뒤로 물러나는지 여부
        static bool bIsMovingToMiddleDistance = false; // MiddleDistance로 이동할지 여부

        // 타겟에 접근하는 중 스킬 사용 (Skill3 제외)
        if (!bHasUsedSkill && CachedDistanceToTarget <= SkillDistance && !bIsBackingAway)
        {
            // Skill3를 제외한 스킬을 무작위로 선택하여 사용
            int32 RandomSkill = FMath::RandRange(0, 2);
            switch (RandomSkill)
            {
            case 0:
                CurrentState = EGrizzBoltState::SKILL1;
                break;
            case 1:
                CurrentState = EGrizzBoltState::SKILL2;
                break;
            case 2:
                CurrentState = EGrizzBoltState::SKILL4;
                break;
            }
            TransitionToSkillState(); // 선택된 스킬 발동
            bHasUsedSkill = true;
            UE_LOG(LogTemp, Log, TEXT("Used skill (not Skill3) at SkillDistance while approaching."));
            return;  // 스킬 사용 후 상태가 변경되었으므로 함수를 종료합니다.
        }

        // NearDistance에 도달했을 때
        if (CachedDistanceToTarget <= NearDistance)
        {
            if (!bIsBackingAway)
            {
                bIsMovingToMiddleDistance = FMath::RandRange(0, 4) == 0; // 1/5 확률로 MiddleDistance로 이동
                bIsBackingAway = true;

                if (bIsMovingToMiddleDistance)
                {
                    UE_LOG(LogTemp, Log, TEXT("Moving to MiddleDistance for Skill3."));
                }
                else
                {
                    UE_LOG(LogTemp, Log, TEXT("Moving to SkillDistance for other skills."));
                }
            }
        }

        if (bIsBackingAway)
        {
            FVector MoveBackDirection = -DirToTarget.GetSafeNormal();

            FRotator BackRotation = MoveBackDirection.Rotation();
            BackRotation.Pitch = FMath::Clamp(BackRotation.Pitch, -10.f, 25.f);
            Me->SetActorRotation(BackRotation);

            Me->AddMovementInput(MoveBackDirection);

            if (bIsMovingToMiddleDistance)
            {
                if (CachedDistanceToTarget >= MiddleDistance)
                {
                    bIsBackingAway = false;
                    bHasUsedSkill = true;

                    CurrentState = EGrizzBoltState::SKILL3;

                    FRotator NewRotation = DirToTarget.Rotation();
                    NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch, -10.f, 25.f);
                    Me->SetActorRotation(NewRotation);

                    TransitionToSkillState(); // Skill3 발동

                    UE_LOG(LogTemp, Log, TEXT("Reached MiddleDistance and used Skill3."));
                    return; // 상태 전환 후 함수 종료
                }
            }
            else
            {
                if (CachedDistanceToTarget >= SkillDistance)
                {
                    bIsBackingAway = false;
                    bHasUsedSkill = false;

                    FRotator NewRotation = DirToTarget.Rotation();
                    NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch, -10.f, 25.f);
                    Me->SetActorRotation(NewRotation);

                    TransitionToSkillState(); // Skill1, Skill2, Skill4 중 하나 발동

                    UE_LOG(LogTemp, Log, TEXT("Backed away to SkillDistance. Ready for next approach."));
                    return; // 상태 전환 후 함수 종료
                }
            }
        }
        else
        {
            Me->AddMovementInput(DirToTarget.GetSafeNormal());

            FRotator NewRotation = DirToTarget.Rotation();
            NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch, -10.f, 25.f);
            Me->SetActorRotation(NewRotation);

            UE_LOG(LogTemp, Log, TEXT("Approaching target."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Target or Me is Nullptr!"));
        CurrentState = EGrizzBoltState::IDLE;
    }
}





void UGrizzBoltActorComponent::TickSkill1(const float& DeltaTime)
{
    GetOwner()->GetComponentByClass<UGrizzBoltSkillComponent>()->TickLineThunder(DeltaTime);
}

void UGrizzBoltActorComponent::TickSkill2(const float& DeltaTime)
{
    GetOwner()->GetComponentByClass<UGrizzBoltSkillComponent>()->TickTripleSpark(DeltaTime);
}

void UGrizzBoltActorComponent::TickSkill3(const float& DeltaTime)
{
    GetOwner()->GetComponentByClass<UGrizzBoltSkillComponent>()->TickElectricBomb(DeltaTime);
}

void UGrizzBoltActorComponent::TickSkill4(const float& DeltaTime)
{
    GetOwner()->GetComponentByClass<UGrizzBoltSkillComponent>()->TickElectricMinigun(DeltaTime);
}

void UGrizzBoltActorComponent::TickDAMAGE(const float& DeltaTime)
{
    // DAMAGE 로직 추가
}

void UGrizzBoltActorComponent::TickDIE(const float& DeltaTime)
{
    // DIE 로직 추가
	auto Bolt = GetOwner<AGrizzBolt>();
	if (!bIsDead && Bolt)
	{
		auto StatusComp = Bolt->GetComponentByClass<UPStatusComponent>();
		if (StatusComp && StatusComp->IsDead())
		{
			bIsDead = true;
			Bolt->GetMesh()->SetCollisionProfileName("RagDoll");
			Bolt->GetMesh()->SetSimulatePhysics(true);
			Bolt->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			auto SkillComp = Bolt->GetComponentByClass<UGrizzBoltSkillComponent>();
			if (SkillComp) SkillComp->SetComponentTickEnabled(false);
			SetComponentTickEnabled(false);
		}
	}
}

void UGrizzBoltActorComponent::SpawnThunderWithDelay(int32 Index, float Delay)
{
    if (ThunderClass != nullptr && ArrowComponents.IsValidIndex(Index))
    {
        FVector SpawnLocation = ArrowComponents[Index]->GetComponentLocation();
        FRotator SpawnRotation = ArrowComponents[Index]->GetComponentRotation();
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Me;

        FTimerHandle TimerHandle;

        FTimerDelegate Delegate;
        Delegate.BindUFunction(this, "Test", SpawnLocation, SpawnRotation);
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, Delegate, Delay, false);

        /*GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, SpawnLocation, SpawnRotation, SpawnParams, Index]()
            {
                if (ThunderClass)
                {
                    AThunder* SpawnedThunder = GetWorld()->SpawnActor<AThunder>(ThunderClass, SpawnLocation, SpawnRotation, SpawnParams);
                    if (SpawnedThunder == nullptr)
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to spawn Thunder at index %d"), Index);
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("ThunderClass is not set"));
                }
            }, Delay, false);*/

        TimerHandles.Add(TimerHandle);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid ArrowComponent or ThunderClass not set"));
    }
}

void UGrizzBoltActorComponent::Test(FVector SpawnLocation, FRotator SpawnRotation)
{
    if (ThunderClass)
    {
        AThunder* SpawnedThunder = GetWorld()->SpawnActor<AThunder>(ThunderClass, SpawnLocation, SpawnRotation);
        if (SpawnedThunder == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn Thunder"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ThunderClass is not set"));
    }
}

void UGrizzBoltActorComponent::SetHasFoundTarget(bool bFound)
{
    bHasFoundTarget = bFound;
}

bool UGrizzBoltActorComponent::HasFoundTarget() const
{
    return bHasFoundTarget;
}

void UGrizzBoltActorComponent::ResetSkillCooldown()
{
    bIsSkillCooldown = false;
}