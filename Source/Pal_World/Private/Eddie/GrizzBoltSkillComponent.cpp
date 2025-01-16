// Fill out your copyright notice in the Description page of Project Settings.


#include "Eddie/GrizzBoltSkillComponent.h"
#include "Eddie/GrizzBolt.h"
#include "Eddie/BoltAnimInstance.h"
#include "Eddie/GrizzBoltActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Eddie/Thunder.h"
#include "Eddie/GrizzBolt.h"
#include "Eddie/bolt.h"
#include "Eddie/bolt2.h"
#include "Eddie/Bolt3.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Justin/Frameworks/PStatusLibrary.h"
#include "GrizzBullet.h"
#include "LastBullet.h"
#include "Components/AudioComponent.h"

// Sets default values for this component's properties
UGrizzBoltSkillComponent::UGrizzBoltSkillComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
    Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));


}


// Called when the game starts
void UGrizzBoltSkillComponent::BeginPlay()
{
	Super::BeginPlay();

    FSM = GetOwner()->GetComponentByClass<UGrizzBoltActorComponent>();
    Me = GetOwner< AGrizzBolt >();
    RadialComp = GetOwner<AGrizzBolt>();

    if (Me && Me->GetMesh())
    {
        UAnimInstance* AnimInstance = Me->GetMesh()->GetAnimInstance();
        BoltAnimInstance = Cast<UBoltAnimInstance>(AnimInstance);
        if (BoltAnimInstance)
        {
            AM_GrizzBolt_Skill = BoltAnimInstance->AM_GrizzBolt_Skill;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to cast to UBoltAnimInstance"));
        }    

    }
    // 플래그 초기화
    bAnimMontage = false;
}


// Called every frame
void UGrizzBoltSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
void UGrizzBoltSkillComponent::TickLineThunder(const float& DeltaTime)
{
    if (bSkill1Executed)
    {
        return;
    }

    Skill1CurrentTime += DeltaTime;

    if (Skill1CurrentTime >= 1.8f)
    {
        // 타겟이 범위 밖에 있더라도 스킬을 발동하도록 설정
        for (int32 i = 3; i < Me->ArrowComponents.Num(); i++)
        {
            float Delay = i * 0.2f;
            SpawnThunderWithDelay(i, Delay);
        }

        // 스킬 발동 후 MOVE 상태로 전환
        float TotalDelay = Me->ArrowComponents.Num() * 0.2f;
        GetWorld()->GetTimerManager().SetTimer(Skill1TimerHandle, this, &UGrizzBoltSkillComponent::ReturnToMoveStateFromSkill1, TotalDelay + 0.5f, false);

        bSkill1Executed = true;
        UE_LOG(LogTemp, Log, TEXT("SKILL1 executed regardless of target distance, transitioning to MOVE after delay."));

        // 스킬 시간 초기화
        Skill1CurrentTime = 0.0f;
    }
}



// 스킬2 (트리플 스파크)
void UGrizzBoltSkillComponent::TickTripleSpark(const float& DeltaTime)
{
    if (bSkill2Executed)
    {
        return;
    }

    Skill2CurrentTime += DeltaTime;
    if (Skill2CurrentTime >= 2.f)
    {
        if (!FSM->Target)
        {
            UE_LOG(LogTemp, Warning, TEXT("Target is null"));
            return;
        }
        FVector TargetLocation = FSM->Target->GetActorLocation(); // 플레이어의 위치를 가져옴
        if (!Audio->IsPlaying())
        {
            //LastBullet Sound
            Audio->SetSound(Sound_TripleBolt);
            Audio->Play();
        }
        // 1번 Arrow에서 발사 (Abolt)
        if (BoltClass != nullptr && Me->ArrowComponents.IsValidIndex(0))
        {
            FVector SpawnLocation = Me->ArrowComponents[0]->GetComponentLocation();
            FRotator SpawnRotation = (TargetLocation - SpawnLocation).Rotation(); // 타겟 방향으로 회전
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = Me;

            // Spawn bolt
            Abolt* SpawnedBolt = GetWorld()->SpawnActor<Abolt>(BoltClass, SpawnLocation, SpawnRotation, SpawnParams);
            if (SpawnedBolt == nullptr)
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn Bolt"));
            }
            else
            {
                // 이동 로직 추가
                FVector LaunchDirection = (TargetLocation - SpawnLocation).GetSafeNormal();
                SpawnedBolt->InitializeProjectile(LaunchDirection);
                UE_LOG(LogTemp, Log, TEXT("Bolt spawned and moving successfully towards player"));
            }
        }

        // 2번 Arrow에서 발사 (Abolt2)
        if (Bolt2Class != nullptr && Me->ArrowComponents.IsValidIndex(1))
        {
            FVector SpawnLocation = Me->ArrowComponents[1]->GetComponentLocation();
            FRotator SpawnRotation = Me->ArrowComponents[1]->GetComponentRotation(); // 타겟 방향으로 회전 ArrowComponents[1]->GetComponentRotation(); (TargetLocation - SpawnLocation).Rotation();
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = Me;

            // Spawn bolt2
            Abolt2* SpawnedBolt2 = GetWorld()->SpawnActor<Abolt2>(Bolt2Class, SpawnLocation, SpawnRotation, SpawnParams);
            if (SpawnedBolt2 == nullptr)
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn Bolt2"));
            }
            else
            {
                // 이동 로직 추가
                FVector LaunchDirection = (TargetLocation - SpawnLocation).GetSafeNormal(); //Me->ArrowComponents[1]->GetComponentLocation() (TargetLocation - SpawnLocation).GetSafeNormal();
                SpawnedBolt2->InitializeProjectile(LaunchDirection);
                UE_LOG(LogTemp, Log, TEXT("Bolt2 spawned and moving successfully towards player"));
            }
        }

        // 3번 Arrow에서 발사 (ABolt3)
        if (Bolt3Class != nullptr && Me->ArrowComponents.IsValidIndex(2))
        {
            FVector SpawnLocation = Me->ArrowComponents[2]->GetComponentLocation();
            FRotator SpawnRotation = Me->ArrowComponents[2]->GetComponentRotation(); // 타겟 방향으로 회전 ArrowComponents[2]->GetComponentRotation();
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = Me;

            // Spawn bolt3
            ABolt3* SpawnedBolt3 = GetWorld()->SpawnActor<ABolt3>(Bolt3Class, SpawnLocation, SpawnRotation, SpawnParams);
            if (SpawnedBolt3 == nullptr)
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn Bolt3"));
            }
            else
            {
                // 이동 로직 추가
                FVector LaunchDirection = (TargetLocation - SpawnLocation).GetSafeNormal();// (TargetLocation - SpawnLocation).GetSafeNormal();
                SpawnedBolt3->InitializeProjectile(LaunchDirection);
                UE_LOG(LogTemp, Log, TEXT("Bolt3 spawned and moving successfully towards player"));
            }
        }

        // 상태 전환을 위한 타이머 설정 (2초 후에 Move 상태로 전환)
        GetWorld()->GetTimerManager().SetTimer(Skill2TimerHandle, this, &UGrizzBoltSkillComponent::ReturnToMoveStateFromSkill2, 2.0f, false);
        bSkill2Executed = true; // 스킬 쿨다운 시작

        //GetWorld()->GetTimerManager().SetTimer(Skill2TimerHandle, this, &UGrizzBoltActorComponent::ResetSkillCooldown, , false);
        Skill2CurrentTime = 0.0f;
    }
}


void UGrizzBoltSkillComponent::TickElectricMinigun(const float& DeltaTime)
{
    Skill4CurrentTime += DeltaTime;
    MinigunShootTimer += DeltaTime;
    LastShootTimer += DeltaTime;

    UE_LOG(LogTemp, Log, TEXT("Skill4CurrentTime: %f"), Skill4CurrentTime);

    if (bSkill4Executed)
    {
        return;
    }

    // 캐릭터가 플레이어를 지속적으로 바라보도록 설정
    FVector dir = FSM->Target->GetActorLocation() - Me->GetActorLocation();
    FRotator NewRotation = dir.Rotation();
    NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch, -10.f, 25.f); // 필요에 따라 Pitch 제한을 조정
    Me->SetActorRotation(NewRotation);

    // 5.3초 이하일 때는 Bullet을 계속 발사
    if (Skill4CurrentTime <= 4.3f)
    {
        if (MinigunShootTimer >= 0.1f)
        {
            FTransform MinigunTransform = Me->Minigun->GetComponentTransform();
            FVector FireLocation = MinigunTransform.GetLocation();
            FVector FireDirection = MinigunTransform.GetUnitAxis(EAxis::Y); // Y축 방향 사용

            // 발사체 생성
            GetWorld()->SpawnActor<AGrizzBullet>(BulletFactory, FireLocation, FireDirection.Rotation());

            // 타이머 초기화
            MinigunShootTimer = 0.0f;

            if (!Audio->IsPlaying())
            {
                //LastBullet Sound
                Audio->SetSound(Sound_MachineGun);
                Audio->Play();
            }

        }
    }

    // 5.3초가 지난 후에는 LastBullet 발사
    if (Skill4CurrentTime > 4.9f && LastShootTimer >= 1.0f)
    {
        if (Audio->IsPlaying())
        {
            Audio->Stop();
        }
        if (!Audio->IsPlaying())
        {
            //LastBullet Sound
            Audio->SetSound(Sound_MachineGun_Last);
            Audio->Play();
        }
        FTransform MinigunTransform = Me->Minigun->GetComponentTransform();
        FVector FireLocation = MinigunTransform.GetLocation();
        FVector FireDirection = MinigunTransform.GetUnitAxis(EAxis::Y); // Y축 방향 사용
        FActorSpawnParameters params;
        params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        // LastBullet 발사
       // auto* LastBullet = GetWorld()->SpawnActor<ALastBullet>(LastFactory, FireLocation, FireDirection.Rotation(),params);
        Me->SpawnGAP();
        // 타이머 초기화 및 상태 전환
        LastShootTimer = -FLT_MAX;

        // 상태 전환 및 쿨다운 설정
        auto GrizzBoltActorComponent = GetOwner()->FindComponentByClass<UGrizzBoltActorComponent>();
        if (GrizzBoltActorComponent)
        {
            GrizzBoltActorComponent->CurrentState = EGrizzBoltState::MOVE;
            UE_LOG(LogTemp, Log, TEXT("State changed back to MOVE from SKILL4"));

            // 쿨다운 시작
            GrizzBoltActorComponent->bIsSkillCooldown = true;
            GrizzBoltActorComponent->GetWorld()->GetTimerManager().SetTimer(
                GrizzBoltActorComponent->SkillCooldownTimerHandle,
                GrizzBoltActorComponent,
                &UGrizzBoltActorComponent::ResetSkillCooldown,
                GrizzBoltActorComponent->CooldownTime + 0.5f,
                false
            );
        }

        // SKILL4 상태 종료 및 전환
        GetWorld()->GetTimerManager().SetTimer(
            Skill4TimerHandle,
            this,
            &UGrizzBoltSkillComponent::ReturnToMoveStateFromSkill4,
            1.0f,
            false
        );

        // SKILL4 플래그 설정을 마지막으로 이동
        bSkill4Executed = true;
        Skill4CurrentTime = 0.0f;

        // 타이머 초기화
        LastShootTimer = 0.0f;
        MinigunShootTimer = 0.0f;
    }
}







void UGrizzBoltSkillComponent::TickElectricBomb(const float& DeltaTime)
{
    Skill3CurrentTime += DeltaTime;
    UE_LOG(LogTemp, Log, TEXT("Skill3CurrentTime: %f"), Skill3CurrentTime);

    if (bSkill3Executed)
    {
        return;
    }

    if (Skill3CurrentTime > 4.8f)
    {
        UE_LOG(LogTemp, Log, TEXT("Skill3CurrentTime 조건 충족, 로직 실행"));

        if (Me && Me->RadialComp)
        {
            TArray<FHitResult >Hits;

            FCollisionShape Shape;
            Shape.SetSphere(RadialComp->Radius);
            FCollisionQueryParams QueryParams;

            auto Pal = Cast<AGrizzBolt>(GetOwner());
            if (Pal)
            {
                QueryParams.AddIgnoredActor(GetOwner());
            }

            if (GetWorld()->SweepMultiByObjectType(Hits, Me->GetActorLocation(), Me->GetActorLocation(), FQuat::Identity, ECollisionChannel::ECC_Pawn, Shape, QueryParams))
            {
                for (auto& hit : Hits)
                {
                    //DrawDebugSphere(GetWorld(), Hits[0].ImpactPoint, 60.f, 10, FColor::Red, false, 2.f);
                    UPStatusLibrary::ApplyDamage(GetOwner(), hit.GetActor(), Hits[0].ImpactPoint, 800.f);
                }
            }
            //DrawDebugSphere(GetWorld(), Me->GetActorLocation(), RadialComp->Radius*1.8f, 24, FColor::Yellow, false, 3.f);
        }

        // 상태 전환 및 쿨다운 설정
        auto GrizzBoltActorComponent = GetOwner()->FindComponentByClass<UGrizzBoltActorComponent>();
        if (GrizzBoltActorComponent)
        {
            GrizzBoltActorComponent->CurrentState = EGrizzBoltState::MOVE;
            UE_LOG(LogTemp, Log, TEXT("State changed back to MOVE from SKILL3"));

            // 쿨다운 시작
            GrizzBoltActorComponent->bIsSkillCooldown = true;
            GrizzBoltActorComponent->GetWorld()->GetTimerManager().SetTimer(GrizzBoltActorComponent->SkillCooldownTimerHandle, GrizzBoltActorComponent, &UGrizzBoltActorComponent::ResetSkillCooldown, GrizzBoltActorComponent->CooldownTime + 0.5f, false); // 쿨다운 시간 증가
        }

        GetWorld()->GetTimerManager().SetTimer(Skill3TimerHandle, this, &UGrizzBoltSkillComponent::ReturnToMoveStateFromSkill3, 1.0f, false);
        bSkill3Executed = true;
        Skill3CurrentTime = 0.0f;
    }
}





void UGrizzBoltSkillComponent::SpawnThunderWithDelay(int32 Index, float Delay)
{
    if (ThunderClass != nullptr && Me->ArrowComponents.IsValidIndex(Index))
    {
        FVector SpawnLocation = Me->ArrowComponents[Index]->GetComponentLocation();
        FRotator SpawnRotation = Me->ArrowComponents[Index]->GetComponentRotation();
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Me;

        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, SpawnLocation, SpawnRotation, SpawnParams, Index]()
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
            }, Delay, false);

        TimerHandles.Add(TimerHandle);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid ArrowComponent or ThunderClass not set"));
    }
}

void UGrizzBoltSkillComponent::ResetSkillCooldown()
{
    
}

void UGrizzBoltSkillComponent::ReturnToMoveStateFromSkill1()
{
    GetOwner()->GetComponentByClass<UGrizzBoltActorComponent>()->ReturnToMoveStateFromSkill1();
    bSkill1Executed = false; // 플래그 리셋
    UE_LOG(LogTemp, Log, TEXT("State changed back to MOVE from SKILL1"));
    Skill1CurrentTime = 0.0f; // 타이머 초기화
}

void UGrizzBoltSkillComponent::ReturnToMoveStateFromSkill2()
{
    if (Audio->IsPlaying())
    {
        Audio->Stop();
    }
    GetOwner()->GetComponentByClass<UGrizzBoltActorComponent>()->ReturnToMoveStateFromSkill2();
    bSkill2Executed = false;
    UE_LOG(LogTemp, Log, TEXT("State changed back to MOVE from SKILL2"));
    Skill2CurrentTime = 0.0f; // 타이머 초기화
}

void UGrizzBoltSkillComponent::ReturnToMoveStateFromSkill3()
{
    GetOwner()->GetComponentByClass<UGrizzBoltActorComponent>()->ReturnToMoveStateFromSkill3();
    bSkill3Executed = false;
    UE_LOG(LogTemp, Log, TEXT("State changed back to MOVE from SKILL3"));
    Skill3CurrentTime = 0.0f; // 타이머 초기화

}

void UGrizzBoltSkillComponent::ReturnToMoveStateFromSkill4()
{
    // 스킬이 끝나면 Minigun을 숨김
    Me->Minigun->SetVisibility(false);
    UE_LOG(LogTemp, Log, TEXT("Minigun set to invisible."));

    // 상태를 MOVE로 전환
    auto GrizzBoltActorComponent = GetOwner()->FindComponentByClass<UGrizzBoltActorComponent>();
    if (GrizzBoltActorComponent)
    {
        GrizzBoltActorComponent->CurrentState = EGrizzBoltState::MOVE;
    }

    bSkill4Executed = false;
    Skill4CurrentTime = 0.0f; // 타이머 초기화
}
