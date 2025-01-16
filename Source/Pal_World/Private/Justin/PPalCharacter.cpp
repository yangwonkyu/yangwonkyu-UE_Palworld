// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/PPalCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Justin/PFSMComponent.h"
#include "Justin/PPalSkillComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Eddie/GrizzBolt.h"
#include "Sunny/PCharacter.h"
#include "Sunny/PalInfo.h"
#include "Justin/Components/PalStatusComp.h"
#include "Components/SceneComponent.h"
#include "PalWorld_GameMode.h"

APPalCharacter::APPalCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	BaseHeight = 0.f;
	bIsInAir = false;

	FSMComponent = CreateDefaultSubobject<UPFSMComponent>("PalFSMComponent");
	SkillComponent = CreateDefaultSubobject<UPPalSkillComponent>("PalSkillComponent");
	StatusComponent = CreateDefaultSubobject<UPalStatusComp>("StatusComponent");
	WidgetLocationComp = CreateDefaultSubobject<USceneComponent>("WidgetLocationComp");
	WidgetLocationComp->SetupAttachment(GetMesh(), "headSocket");
	WidgetLocationComp->ComponentTags.Add(FName("WidgetLocation"));

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	bUseControllerRotationYaw = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bIsSummoning = false;
	MaxSummonTime = .1f;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
}

void APPalCharacter::BeginPlay()
{
	Super::BeginPlay();
	BaseHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	StatusComponent->OnDied.AddUObject(this, &APPalCharacter::DeclareDead);
	StatusComponent->OnHealthChanged.AddUObject(this, &APPalCharacter::OnTakenDamage);
}

void APPalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsSummoning)
	{
		TotalSummonTime += DeltaTime;
		if (TotalSummonTime < MaxSummonTime)
		{
			FVector Scale = FMath::Lerp(StartSummonScale, FVector::One(), TotalSummonTime / MaxSummonTime);
			SetActorScale3D(Scale);
		}
		else
		{
			TotalSummonTime = 0.f;
			SetActorScale3D(FVector::One());
			bIsSummoning = false;
		}		
	}

	if (bIsInAir)
	{
		FHitResult Hit;
		FCollisionShape Shape;
		Shape.SetSphere(1.f);
		FVector Start = GetActorLocation() + GetActorUpVector() * (BaseHeight * -1.f);
		//DrawDebugSphere(GetWorld(), Start, 1.f, 10, FColor::Blue, false);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		if (GetWorld()->SweepSingleByChannel(Hit, Start, Start, FQuat::Identity, ECollisionChannel::ECC_Visibility,
			Shape, QueryParams))
		{
			bIsInAir = false;
			GetCapsuleComponent()->SetSimulatePhysics(false);
			FVector Loc = GetActorLocation();
			SetActorLocation(FVector(Loc.X, Loc.Y, Hit.ImpactPoint.Z + BaseHeight));
		}
	}
}

void APPalCharacter::Initialize(FVector SpawnLoc, AActor* _Player, UPalInfo* _Info, AActor* NewTarget)
{
	Player = Cast<APCharacter>(_Player);
	Info = _Info;
	SetTarget(NewTarget);

	if (!Info) //if pal is free from the shakles of the pal sphere
	{
		//Place Pal on ground
		FVector Loc = GetActorLocation();
		Loc.Z = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		SetActorLocation(Loc);
	}
	else Retrieve(SpawnLoc);
}

const UPalInfo* APPalCharacter::GetPalInfo() const
{
	return Info;
}

void APPalCharacter::StartPalBounce(FVector Impulse, bool VelocityChange)
{
	auto CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetSimulatePhysics(true);
	CapsuleComp->AddImpulse(Impulse, FName(), VelocityChange);
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &APPalCharacter::StartIsInAir, .2f, false);
}

void APPalCharacter::MoveToLocation(float Speed, FVector GoalLocation)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
	auto AIController = Cast<AAIController>(GetController());
	if (AIController)
		AIController->MoveToLocation(GoalLocation, 50.f);
}

void APPalCharacter::SetIsInAir(bool _bIsInAir)
{
	bIsInAir = _bIsInAir;
}

bool APPalCharacter::GetIsInAir() const
{
	return bIsInAir;
}

float APPalCharacter::GetBaseHeight() const
{
	return BaseHeight;
}

AActor* APPalCharacter::GetTarget()
{
	return Target;
}

void APPalCharacter::SetTarget(AActor* _Target)
{
	Target = _Target;
	UE_LOG(LogTemp, Warning, TEXT("INSIDE!"));
}

void APPalCharacter::Retrieve(FVector RetrieveLoc)
{
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);

	FSMComponent->Disable();
	SkillComponent->Disable();
	StatusComponent->DisplayWidget(false);

	SetTarget(nullptr);
	SetActorLocation(RetrieveLoc);
	GetCharacterMovement()->GravityScale = 0.f;

	UE_LOG(LogTemp, Warning, TEXT("Pal Retrieved! %s"), *GetNameSafe(this));
}

void APPalCharacter::Summon(FVector SpawnLoc, AActor* _Target)
{
	SpawnLoc.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SetActorLocation(SpawnLoc);

	GetCharacterMovement()->GravityScale = 1.f;

	if (_Target)
	{
		SetTarget(_Target);
	}
	else
	{
		auto GrizzBolt = UGameplayStatics::GetActorOfClass(GetWorld(), AGrizzBolt::StaticClass());
		if (GrizzBolt)
		{
			auto Status = GrizzBolt->GetComponentByClass<UPStatusComponent>();
			if (Status && !Status->IsDead())
			{
				if (!Status->OnDied.IsBoundToObject(this))
				{
					Status->OnDied.AddUObject(this, &APPalCharacter::OnBossDead);
				}
				SetTarget(GrizzBolt);
			}
			else SetTarget(nullptr);
		}
	}

	FSMComponent->Enable();
	SkillComponent->Enable();
	StatusComponent->DisplayWidget(true);

	if (Player)
	{
		Player->StartCanInteractTimer();
	}

	bIsSummoning = true;
	UE_LOG(LogTemp, Warning, TEXT("Pal summoned! %s"), *GetNameSafe(this));
}

void APPalCharacter::OnBossDead()
{
	SetTarget(nullptr);
	FSMComponent->ExecuteAction(EPalState::IDLE);
}

void APPalCharacter::OnTakenDamage(float Health, float MaxHealth)
{
	auto Anim = GetMesh()->GetAnimInstance();
	if (Anim && FSMComponent)
	{
		EPalState State = FSMComponent->GetCurrentState();
		Anim->Montage_Play(DamagedMontage);
	}
}

void APPalCharacter::DeclareDead()
{
	FSMComponent->Disable();
	SkillComponent->Disable();
	StatusComponent->DisplayWidget(false);
		
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Info)
	{
		Info->DeclareDead(this);
	}
}

bool APPalCharacter::HasTarget() const
{
	return Target != nullptr;
}

FVector APPalCharacter::GetTargetLocation(AActor* RequestedBy) const
{
	if (Target) return Target->GetActorLocation();
	return FVector::Zero();
}

APCharacter* APPalCharacter::GetPlayer() const
{
	return Player;
}

void APPalCharacter::StartIsInAir()
{
	bIsInAir = true;
}

void APPalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
