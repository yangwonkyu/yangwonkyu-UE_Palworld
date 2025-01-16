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
    // �÷��� �ʱ�ȭ
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
        // Ÿ���� ���� �ۿ� �ִ��� ��ų�� �ߵ��ϵ��� ����
        for (int32 i = 3; i < Me->ArrowComponents.Num(); i++)
        {
            float Delay = i * 0.2f;
            SpawnThunderWithDelay(i, Delay);
        }

        // ��ų �ߵ� �� MOVE ���·� ��ȯ
        float TotalDelay = Me->ArrowComponents.Num() * 0.2f;
        GetWorld()->GetTimerManager().SetTimer(Skill1TimerHandle, this, &UGrizzBoltSkillComponent::ReturnToMoveStateFromSkill1, TotalDelay + 0.5f, false);

        bSkill1Executed = true;
        UE_LOG(LogTemp, Log, TEXT("SKILL1 executed regardless of target distance, transitioning to MOVE after delay."));

        // ��ų �ð� �ʱ�ȭ
        Skill1CurrentTime = 0.0f;
    }
}



// ��ų2 (Ʈ���� ����ũ)
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
        FVector TargetLocation = FSM->Target->GetActorLocation(); // �÷��̾��� ��ġ�� ������
        if (!Audio->IsPlaying())
        {
            //LastBullet Sound
            Audio->SetSound(Sound_TripleBolt);
            Audio->Play();
        }
        // 1�� Arrow���� �߻� (Abolt)
        if (BoltClass != nullptr && Me->ArrowComponents.IsValidIndex(0))
        {
            FVector SpawnLocation = Me->ArrowComponents[0]->GetComponentLocation();
            FRotator SpawnRotation = (TargetLocation - SpawnLocation).Rotation(); // Ÿ�� �������� ȸ��
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
                // �̵� ���� �߰�
                FVector LaunchDirection = (TargetLocation - SpawnLocation).GetSafeNormal();
                SpawnedBolt->InitializeProjectile(LaunchDirection);
                UE_LOG(LogTemp, Log, TEXT("Bolt spawned and moving successfully towards player"));
            }
        }

        // 2�� Arrow���� �߻� (Abolt2)
        if (Bolt2Class != nullptr && Me->ArrowComponents.IsValidIndex(1))
        {
            FVector SpawnLocation = Me->ArrowComponents[1]->GetComponentLocation();
            FRotator SpawnRotation = Me->ArrowComponents[1]->GetComponentRotation(); // Ÿ�� �������� ȸ�� ArrowComponents[1]->GetComponentRotation(); (TargetLocation - SpawnLocation).Rotation();
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
                // �̵� ���� �߰�
                FVector LaunchDirection = (TargetLocation - SpawnLocation).GetSafeNormal(); //Me->ArrowComponents[1]->GetComponentLocation() (TargetLocation - SpawnLocation).GetSafeNormal();
                SpawnedBolt2->InitializeProjectile(LaunchDirection);
                UE_LOG(LogTemp, Log, TEXT("Bolt2 spawned and moving successfully towards player"));
            }
        }

        // 3�� Arrow���� �߻� (ABolt3)
        if (Bolt3Class != nullptr && Me->ArrowComponents.IsValidIndex(2))
        {
            FVector SpawnLocation = Me->ArrowComponents[2]->GetComponentLocation();
            FRotator SpawnRotation = Me->ArrowComponents[2]->GetComponentRotation(); // Ÿ�� �������� ȸ�� ArrowComponents[2]->GetComponentRotation();
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
                // �̵� ���� �߰�
                FVector LaunchDirection = (TargetLocation - SpawnLocation).GetSafeNormal();// (TargetLocation - SpawnLocation).GetSafeNormal();
                SpawnedBolt3->InitializeProjectile(LaunchDirection);
                UE_LOG(LogTemp, Log, TEXT("Bolt3 spawned and moving successfully towards player"));
            }
        }

        // ���� ��ȯ�� ���� Ÿ�̸� ���� (2�� �Ŀ� Move ���·� ��ȯ)
        GetWorld()->GetTimerManager().SetTimer(Skill2TimerHandle, this, &UGrizzBoltSkillComponent::ReturnToMoveStateFromSkill2, 2.0f, false);
        bSkill2Executed = true; // ��ų ��ٿ� ����

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

    // ĳ���Ͱ� �÷��̾ ���������� �ٶ󺸵��� ����
    FVector dir = FSM->Target->GetActorLocation() - Me->GetActorLocation();
    FRotator NewRotation = dir.Rotation();
    NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch, -10.f, 25.f); // �ʿ信 ���� Pitch ������ ����
    Me->SetActorRotation(NewRotation);

    // 5.3�� ������ ���� Bullet�� ��� �߻�
    if (Skill4CurrentTime <= 4.3f)
    {
        if (MinigunShootTimer >= 0.1f)
        {
            FTransform MinigunTransform = Me->Minigun->GetComponentTransform();
            FVector FireLocation = MinigunTransform.GetLocation();
            FVector FireDirection = MinigunTransform.GetUnitAxis(EAxis::Y); // Y�� ���� ���

            // �߻�ü ����
            GetWorld()->SpawnActor<AGrizzBullet>(BulletFactory, FireLocation, FireDirection.Rotation());

            // Ÿ�̸� �ʱ�ȭ
            MinigunShootTimer = 0.0f;

            if (!Audio->IsPlaying())
            {
                //LastBullet Sound
                Audio->SetSound(Sound_MachineGun);
                Audio->Play();
            }

        }
    }

    // 5.3�ʰ� ���� �Ŀ��� LastBullet �߻�
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
        FVector FireDirection = MinigunTransform.GetUnitAxis(EAxis::Y); // Y�� ���� ���
        FActorSpawnParameters params;
        params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        // LastBullet �߻�
       // auto* LastBullet = GetWorld()->SpawnActor<ALastBullet>(LastFactory, FireLocation, FireDirection.Rotation(),params);
        Me->SpawnGAP();
        // Ÿ�̸� �ʱ�ȭ �� ���� ��ȯ
        LastShootTimer = -FLT_MAX;

        // ���� ��ȯ �� ��ٿ� ����
        auto GrizzBoltActorComponent = GetOwner()->FindComponentByClass<UGrizzBoltActorComponent>();
        if (GrizzBoltActorComponent)
        {
            GrizzBoltActorComponent->CurrentState = EGrizzBoltState::MOVE;
            UE_LOG(LogTemp, Log, TEXT("State changed back to MOVE from SKILL4"));

            // ��ٿ� ����
            GrizzBoltActorComponent->bIsSkillCooldown = true;
            GrizzBoltActorComponent->GetWorld()->GetTimerManager().SetTimer(
                GrizzBoltActorComponent->SkillCooldownTimerHandle,
                GrizzBoltActorComponent,
                &UGrizzBoltActorComponent::ResetSkillCooldown,
                GrizzBoltActorComponent->CooldownTime + 0.5f,
                false
            );
        }

        // SKILL4 ���� ���� �� ��ȯ
        GetWorld()->GetTimerManager().SetTimer(
            Skill4TimerHandle,
            this,
            &UGrizzBoltSkillComponent::ReturnToMoveStateFromSkill4,
            1.0f,
            false
        );

        // SKILL4 �÷��� ������ ���������� �̵�
        bSkill4Executed = true;
        Skill4CurrentTime = 0.0f;

        // Ÿ�̸� �ʱ�ȭ
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
        UE_LOG(LogTemp, Log, TEXT("Skill3CurrentTime ���� ����, ���� ����"));

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

        // ���� ��ȯ �� ��ٿ� ����
        auto GrizzBoltActorComponent = GetOwner()->FindComponentByClass<UGrizzBoltActorComponent>();
        if (GrizzBoltActorComponent)
        {
            GrizzBoltActorComponent->CurrentState = EGrizzBoltState::MOVE;
            UE_LOG(LogTemp, Log, TEXT("State changed back to MOVE from SKILL3"));

            // ��ٿ� ����
            GrizzBoltActorComponent->bIsSkillCooldown = true;
            GrizzBoltActorComponent->GetWorld()->GetTimerManager().SetTimer(GrizzBoltActorComponent->SkillCooldownTimerHandle, GrizzBoltActorComponent, &UGrizzBoltActorComponent::ResetSkillCooldown, GrizzBoltActorComponent->CooldownTime + 0.5f, false); // ��ٿ� �ð� ����
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
    bSkill1Executed = false; // �÷��� ����
    UE_LOG(LogTemp, Log, TEXT("State changed back to MOVE from SKILL1"));
    Skill1CurrentTime = 0.0f; // Ÿ�̸� �ʱ�ȭ
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
    Skill2CurrentTime = 0.0f; // Ÿ�̸� �ʱ�ȭ
}

void UGrizzBoltSkillComponent::ReturnToMoveStateFromSkill3()
{
    GetOwner()->GetComponentByClass<UGrizzBoltActorComponent>()->ReturnToMoveStateFromSkill3();
    bSkill3Executed = false;
    UE_LOG(LogTemp, Log, TEXT("State changed back to MOVE from SKILL3"));
    Skill3CurrentTime = 0.0f; // Ÿ�̸� �ʱ�ȭ

}

void UGrizzBoltSkillComponent::ReturnToMoveStateFromSkill4()
{
    // ��ų�� ������ Minigun�� ����
    Me->Minigun->SetVisibility(false);
    UE_LOG(LogTemp, Log, TEXT("Minigun set to invisible."));

    // ���¸� MOVE�� ��ȯ
    auto GrizzBoltActorComponent = GetOwner()->FindComponentByClass<UGrizzBoltActorComponent>();
    if (GrizzBoltActorComponent)
    {
        GrizzBoltActorComponent->CurrentState = EGrizzBoltState::MOVE;
    }

    bSkill4Executed = false;
    Skill4CurrentTime = 0.0f; // Ÿ�̸� �ʱ�ȭ
}
