#include "Eddie/BoltAnimInstance.h"
#include "Eddie/GrizzBolt.h"
#include "Eddie/GrizzBoltActorComponent.h"
#include "Eddie/GrizzBoltSkillComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Justin/Frameworks/PStatusLibrary.h"
#include "Sunny/PCharacter.h"


void UBoltAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    APawn* OwnerPawn = TryGetPawnOwner();
    if (OwnerPawn)
    {
        FSM = OwnerPawn->FindComponentByClass<UGrizzBoltActorComponent>();
        Me = Cast<AGrizzBolt>(OwnerPawn);
        if (Me)
        {
            RadialComp = Me->RadialComp;
        }
    }

    if (!FSM)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find UGrizzBoltActorComponent"));
    }
    if (!Me)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast to AGrizzBolt"));
    }
    if (!RadialComp)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find URadialForceComponent"));
    }
}

void UBoltAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    AGrizzBolt* Grizzbolt = Cast<AGrizzBolt>(TryGetPawnOwner());
    if (nullptr == Grizzbolt)
    {
        return;
    }

    GrizzBoltActorComponent = Grizzbolt->FindComponentByClass<UGrizzBoltActorComponent>();
    if (nullptr == GrizzBoltActorComponent)
    {
        return;
    }

    // 내가 플레이어를 발견했는지?
    if (Grizzbolt->FSMComp)
    {
        SetFindTarget(Grizzbolt->FSMComp->HasFoundTarget());

        if (Grizzbolt->FSMComp->Target)
        {
            DistanceToPlayer = FVector::Dist(Grizzbolt->GetActorLocation(), Grizzbolt->FSMComp->Target->GetActorLocation());
        }

        IsSkillCooldown = Grizzbolt->FSMComp->bIsSkillCooldown;
    }

    // 상태 가져오기
    state = Grizzbolt->FSMComp->CurrentState;

    // 애니메이션 블루프린트에서 CurrentTime을 참조할 수 있도록 설정
    CurrentTime += DeltaSeconds;
}

void UBoltAnimInstance::AnimNotify_StartTime()
{
    CurrentTime = 0.f;
}

void UBoltAnimInstance::SetFindTarget(bool bTarget)
{
    FindTarget = bTarget;
}

void UBoltAnimInstance::AnimNotify_BeginPre()
{
    //UE_LOG(LogTemp, Log, TEXT("AnimNotify_BeginPre called, CurrentTime: %f"), CurrentTime);

    //// CurrentTime이 3초 이상일 때만 다음 섹션으로 이동
    //if (CurrentTime > 3.0f)
    //{
    //    Montage_JumpToSection(FName("2"));
    //}
}

void UBoltAnimInstance::AnimNotify_BeginAttack()
{
    //UE_LOG(LogTemp, Log, TEXT("AnimNotify_BeginAttack called, bIsElectricBomb: %s"), bIsElectricBomb ? TEXT("true") : TEXT("false"));

    //if (bIsElectricBomb)
    //{
    //    AGrizzBolt* Grizzbolt = Cast<AGrizzBolt>(TryGetPawnOwner());
    //    if (Grizzbolt)
    //    {
    //        UGrizzBoltSkillComponent* SkillComponent = Grizzbolt->FindComponentByClass<UGrizzBoltSkillComponent>();
    //        if (SkillComponent)
    //        {
    //            UE_LOG(LogTemp, Log, TEXT("SkillComponent found, calling TickElectricBomb"));
    //            if (!SkillComponent->bSkill3Executed)
    //            {
    //                SkillComponent->TickElectricBomb(CurrentTime);
    //            }
    //        }
    //    }
    //}
    //bIsElectricBomb = false;
}

void UBoltAnimInstance::AnimNotify_EndState()
{
//    UE_LOG(LogTemp, Log, TEXT("AnimNotify_EndState called"));
//
//    if (GrizzBoltActorComponent)
//    {
//        GrizzBoltActorComponent->ReturnToMoveStateFromSkill3();
//    }
}
