// Fill out your copyright notice in the Description page of Project Settings.


#include "Eddie/GrizzBolt.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimInstance.h"
#include "Eddie/GrizzBoltActorComponent.h"
#include "Eddie/GrizzBoltSkillComponent.h"
#include "Blueprint/UserWidget.h"
#include "Justin/Components/PStatusComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/WidgetComponent.h"
#include "Zoe.h"

// Sets default values
AGrizzBolt::AGrizzBolt()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;


    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/YWK/UI/WBP_FindTarget.WBP_FindTarget'"));
    WidgetComponent->SetupAttachment(RootComponent);

    // 몸체를 초기화하고 붙이기
    SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMeshComp->SetupAttachment(RootComponent);

    // 미니건 붙이기
    Minigun = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MinigunComp"));
    Minigun->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ConstructorHelpers::FObjectFinder<UStaticMesh>TempGunMesh(TEXT("/Script/Engine.StaticMesh'/Game/YWK/GrizzBolt/minigun/source/minigun.minigun'"));
    Minigun->SetupAttachment(GetMesh(), TEXT("hand_rMinigun"));
    if (TempGunMesh.Succeeded())
    {
        //Minigun->SetStaticMesh(TempGunMesh.Object);
        
    }
    
   
    
    //Radial 몸에 붙이기
    RadialComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialComp"));
    RadialComp->SetupAttachment(RootComponent);
    // 범위
    RadialComp->Radius = 500.0f;
    // 파워
    RadialComp->ImpulseStrength = 1000.0f;
    RadialComp->bImpulseVelChange = true;

    //SphereComp 초기화 및 설정
    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    SphereComp->SetupAttachment(RootComponent);
    SphereComp->SetSphereRadius(3000.0f); // 방금 Radial이랑 범위 똑같게 늘리면 이것도 늘리기
    SphereComp->SetVisibility(true);
    SphereComp->SetHiddenInGame(true); // 실제 게임플레이중에는 숨기기

    // 애로우가 너무 많으니까..
    for (int i = 0; i < 9; ++i)
    {
        FString ArrowName = FString::Printf(TEXT("ArrowComp%d"), i + 1);
        UArrowComponent* ArrowComp = CreateDefaultSubobject<UArrowComponent>(*ArrowName);
        ArrowComp->SetupAttachment(RootComponent);
        ArrowComponents.Add(ArrowComp);
    }

    // 스켈레탈 메시 가져와야지
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshObj(TEXT("/Script/Engine.SkeletalMesh'/Game/YWK/Asset/fbx_file/GrizzBolt_SkeletalMesh.GrizzBolt_SkeletalMesh'"));
    if (SkeletalMeshObj.Succeeded())
    {
        SkeletalMeshComp->SetSkeletalMesh(SkeletalMeshObj.Object);
    }

    static ConstructorHelpers::FClassFinder<UAnimInstance>AnimBPClass(TEXT("/Script/Engine.AnimBlueprint'/Game/YWK/BP/animation/GrizzBolt_AnimationBP.GrizzBolt_AnimationBP'"));
    if (AnimBPClass.Succeeded())
    {
        SkeletalMeshComp->SetAnimInstanceClass(AnimBPClass.Class);
    }
    // 일렉 판다 위치, 회전 및 스케일 설정
    SkeletalMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -78.0f)); // Z축으로 -90
    SkeletalMeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f)); // Yaw -90
    SkeletalMeshComp->SetRelativeScale3D(FVector(1.6f, 1.6f, 1.6f)); // 스케일 2배

    // FSM 컴포넌트 초기화
    FSMComp = CreateDefaultSubobject<UGrizzBoltActorComponent>(TEXT("FSMComp"));

    // 초기 체력 설정
    HealthPoint = MaxHealthPoint;
    UE_LOG(LogTemp, Warning, TEXT("GrizzBolt::HP init : %.f"), HealthPoint);
    StatusComponent = CreateDefaultSubobject<UPStatusComponent>(TEXT("StatusComponent"));

    //스폰해도 AIController possess 하게 하기
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AGrizzBolt::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // Arrow Components 위치 및 회전 설정
    if (ArrowComponents.Num() >= 10)
    {
        ArrowComponents[0]->SetRelativeLocation(FVector(100.0f, 0.0f, 110.0f));
        ArrowComponents[1]->SetRelativeLocation(FVector(100.0f, 60.0f, 110.0f));
        ArrowComponents[1]->SetRelativeRotation(FRotator(0.0f, 50.0f, 0.0f));
        ArrowComponents[2]->SetRelativeLocation(FVector(100.0f, -60.0f, 110.0f));
        ArrowComponents[2]->SetRelativeRotation(FRotator(0.0f, -50.0f, 0.0f));
        ArrowComponents[3]->SetRelativeLocation(FVector(50.0f, 0.0f, 500.0f));
        ArrowComponents[4]->SetRelativeLocation(FVector(200.0f, 0.0f, 500.0f));
        ArrowComponents[5]->SetRelativeLocation(FVector(350.0f, 0.0f, 500.0f));
        ArrowComponents[6]->SetRelativeLocation(FVector(500.0f, 0.0f, 500.0f));
        ArrowComponents[7]->SetRelativeLocation(FVector(650.0f, 0.0f, 500.0f));
        ArrowComponents[8]->SetRelativeLocation(FVector(800.0f, 0.0f, 500.0f));
        ArrowComponents[9]->SetRelativeLocation(FVector(950.0f, 0.0f, 500.0f));
    }
}

// Called when the game starts or when spawned
void AGrizzBolt::BeginPlay()
{
    Super::BeginPlay();



    //// 조이 붙이기
    AActor* Actor = GetWorld()->SpawnActor(Zoe);
    if (Actor)
    {
         auto SpawnZoe = Cast<AZoe>(Actor);
         if (SpawnZoe)
         {
             SpawnZoe->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("Zoe")));

             // 스케일을 0.01로 설정
             SpawnZoe->SetActorScale3D(FVector(0.01f));
         }

    }

    //OnConstruction(FTransform());

    // 처음에는 안맞고 있는 상태니까 false
    // 미니건 안보이게
    Minigun->SetVisibility(false);

    SecondNum = Second;
    MinuteNum = Minute;

    // 체력바 UI
    if (HPUIClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::HPUIClass OK"));
        UUserWidget* HPWidget = CreateWidget<UUserWidget>(GetWorld(), HPUIClass);
        if (HPWidget)
        {
            UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::HPWidget OK"));
            HPWidget->AddToViewport();
            UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::AddToViewport OK"));

            HPBar = Cast<UProgressBar>(HPWidget->GetWidgetFromName(TEXT("HPbar")));
            HP = Cast<UTextBlock>(HPWidget->GetWidgetFromName(TEXT("HP")));
            MaxHP = Cast<UTextBlock>(HPWidget->GetWidgetFromName(TEXT("MaxHP")));

            if (HPBar && HP && MaxHP)
            {
                UE_LOG(LogTemp, Warning, TEXT("HPBar, HP, MaxHP is successfully cast"));
                SetHealthPercent(HealthPoint, MaxHealthPoint);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("HPBar, HP, MaxHP is null - Could not cast the widget"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("HPWidget is not valid in GrizzBolt::BeginPlay"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HPUIClass is not valid in GrizzBolt::BeginPlay"));
    }


    if (StatusComponent)
    {
        StatusComponent->OnDied.AddUObject(this, &AGrizzBolt::OnDead);
    }

    // 카운트 다운 타이머 설정
    GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &AGrizzBolt::CountDown, 1.0f, true, 1.0f);

    // 카운트다운 타이머 설정
    if (TimerUIClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::TimerUIClass OK"));
        TimerWidget = CreateWidget<UUserWidget>(GetWorld(), TimerUIClass);
        if (TimerWidget)
        {
            UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::TimerWidget OK"));
            TimerWidget->AddToViewport();
            UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::AddToViewport() OK"));

            MinuteText = Cast<UTextBlock>(TimerWidget->GetWidgetFromName(TEXT("minutes")));
            SecondText = Cast<UTextBlock>(TimerWidget->GetWidgetFromName(TEXT("Seconds")));
            ZeroText = Cast<UTextBlock>(TimerWidget->GetWidgetFromName(TEXT("ZeroText")));

            ZeroText->SetVisibility(ESlateVisibility::Hidden);

            if (MinuteText && SecondText)
            {
                UE_LOG(LogTemp, Warning, TEXT("MinuteText, SecondText is successfully cast"));
                CountDown();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("MinuteText, SecondText is null - Could not cast the widget"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("TimerWidget is not valid in APCharacter::BeginPlay"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("TimerUIClass is not valid in APCharacter::BeginPlay"));
    }
}

void AGrizzBolt::OnDead()
{
    if (FSMComp)
    {
        FSMComp->CurrentState = EGrizzBoltState::DIE;
        StatusComponent->HideWidget();
        TimerWidget->RemoveFromParent();
    }  
}

void AGrizzBolt::CountDown()
{
    if (MinuteText && SecondText)
    {
        if (SecondNum > 0)
        {
            --SecondNum;
            UE_LOG(LogTemp, Warning, TEXT("Seconds %.f"), SecondNum);

            if (SecondNum < 10)
            {
                ZeroText->SetVisibility(ESlateVisibility::Visible);
            }
        }
        else
        {
            --MinuteNum;
            SecondNum = 59.f;
            ZeroText->SetVisibility(ESlateVisibility::Hidden);
            UE_LOG(LogTemp, Warning, TEXT("Minutes %d"), MinuteNum);

            if (MinuteNum < 0)
            {
                GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
                SecondNum = 0.0f;
                MinuteNum = 0.0f;
                UE_LOG(LogTemp, Warning, TEXT("Timer Over"));
            }
        }

        FNumberFormattingOptions SecOpts;
        SecOpts.SetMaximumFractionalDigits(0);
        SecondText->SetText(FText::AsNumber(SecondNum, &SecOpts));

        FNumberFormattingOptions MinOpts;
        MinOpts.SetMaximumFractionalDigits(0);
        MinuteText->SetText(FText::AsNumber(MinuteNum, &MinOpts));

        if (MinuteNum <= 0 && SecondNum <= 0)
        {
            bTimeOver = true;
            OnCountDownEnd.ExecuteIfBound();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("APCharacter::CountDown()::NO MinuteText && SecondText"));
    }
}

// Called every frame
void AGrizzBolt::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (RadialComp && SphereComp)
    {
        SphereComp->SetSphereRadius(RadialComp->Radius);
    }
}

// Called to bind functionality to input
void AGrizzBolt::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGrizzBolt::OnGrizzBoltTakeDamage(float Damage /*= 100.f*/)
{
    UE_LOG(LogTemp, Warning, TEXT("Enemy::OnMyTakeDamage()"));
   

    // 총에 맞으면 체력을 100 감소시키고 싶다.
    HealthPoint -= Damage;
    UE_LOG(LogTemp, Warning, TEXT("Enemy::HealthPoint : %.f"), HealthPoint);
    //GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, FString::Printf(TEXT("HP : %.d"), Hp));

    // 만약 체력이 0보다 크다면 데미지 상태로 전이하고 싶다
    //if (Hp <= 0)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("Enemy::Hp == 0"));
    //    bEnemyDead = true;
    //    ClearFireTimer();
    //    OnDie();
    //}
    
}


void AGrizzBolt::SetHealthPercent(float IHP, float IMaxHP)
{
    if (HPBar)
    {
        HPBar->SetPercent(IHP / IMaxHP);
    }

    if (HP)
    {
        HP->SetText(FText::AsNumber(IHP));
    }

    if (MaxHP)
    {
        MaxHP->SetText(FText::AsNumber(IMaxHP));
    }
}