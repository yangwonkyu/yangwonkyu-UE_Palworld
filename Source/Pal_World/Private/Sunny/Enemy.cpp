// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/Enemy.h"
#include "Sunny/PlayerBullet.h"
#include "Sunny/PCharacter.h"

#include "Engine/SkeletalMesh.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 총 컴포넌트 처리
	GunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	GunMeshComp->SetupAttachment(GetMesh());
	GunMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	// 캐릭터 메시 설정
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
	// 총 메시 설정
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Sunny/Assets/Gun/Mesh/SK_FPGun.SK_FPGun'"));
	if (TempGunMesh.Succeeded())
	{
		GunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		GunMeshComp->SetRelativeLocation(FVector(-14, 52, 120));
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// 체력 초기 설정
	Hp = MaxHp;
	//UE_LOG(LogTemp, Warning, TEXT("AEnemy::Hp init : % d"), Hp);

	Player = Cast<APCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	SetFireTimer();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Player)
	{
		RotateTarget(Player->GetActorLocation());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AEnemy::Tick - Player is nullptr"));
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


//  발사 타이머 설정
void AEnemy::SetFireTimer()
{
	//UE_LOG(LogTemp, Warning, TEXT("SetFireTimer()"));
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &AEnemy::CheckFireCondition, FireRate, true);
}

// 발사 타이머 해제
void AEnemy::ClearFireTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("ClearFireTimer()"));
	if (GetWorldTimerManager().IsTimerActive(FireRateTimerHandle))
	{

		GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
	}
}



// Rotate along the Player
void AEnemy::RotateTarget(FVector LookAtTarget)
{
	FVector ToTarget = LookAtTarget - this->GetActorLocation();
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);

	this->SetActorRotation(
		FMath::RInterpTo(
			this->GetActorRotation(),
			LookAtRotation,
			UGameplayStatics::GetWorldDeltaSeconds(this),
			10.f)
	);
}

// Check Fire Condition
void AEnemy::CheckFireCondition()
{
	//UE_LOG(LogTemp, Warning, TEXT("CheckFireCondition()"));
	FVector dir = Player->GetActorLocation() - this->GetActorLocation();
	float dist = dir.Size();

	if (dist < AttackDistance)
	{
		//UE_LOG(LogTemp, Warning, TEXT("AttackDistance!!!"));
		Fire();
	}

}


void AEnemy::Fire()
{
	//UE_LOG(LogTemp, Warning, TEXT("Fire()"));
	FTransform firePos = GunMeshComp->GetSocketTransform(TEXT("FirePosition"));

	// Line Trace 
	FHitResult OutHit;
	FVector Start = firePos.GetLocation();
	FVector End = Player->GetActorLocation();
	//ECollisionChannel TraceChannel = ECC_Visibility;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	//bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, TraceChannel, Params);
	bool bHit = GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, ECC_Pawn, Params);
	//GetWorld()->LineTraceSingleByProfile("Pawn", OutHit, Start, End, ECC_Pawn, Params);
	
	if (bHit)
	{
		// 바라본 곳에 뭔가 있다
		//DrawDebugLine(GetWorld(), Start, OutHit.ImpactPoint, FColor::Red, false, 3.f);

		// 폭발VFX를 표현하고 싶다.
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletImpactVFXFactory, OutHit.ImpactPoint);

		// 총알 생성 및 발사
		APlayerBullet* bullet = GetWorld()->SpawnActor<APlayerBullet>(BulletFatory, firePos);
		if (bullet)
		{
			bullet->MovementComp->Velocity = (End - Start).GetSafeNormal() * bullet->MovementComp->InitialSpeed;
		}

		APCharacter* player = Cast<APCharacter>(OutHit.GetActor());
		if (player)
		{
			UE_LOG(LogTemp, Warning, TEXT("hit player"));
			// OnMyTakeDamege()를 호출하고 싶다
			player->OnPlayerTakeDamage(Damage, this);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NO player"));
		}
		
	}
	else
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 3.f);
	}


	


	// 만약 부딪힌 액터가 APCharacter라면
	/*AActor* HitActor = OutHit.GetActor();
	if (HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Actor Hit"));
	}

	APCharacter* player = Cast<APCharacter>(HitActor);
	if (player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit player: %s"), *Player->GetName());
		player->OnMyTakeDamage();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor is not a Player"));
	}*/


	//UE_LOG(LogTemp, Warning, TEXT("------------"));
	
}

void AEnemy::OnEnemyTakeDamage(float InDamage)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy::OnMyTakeDamage()"));

	// 총에 맞으면 체력을 100 감소시키고 싶다.
	Hp -= InDamage;
	UE_LOG(LogTemp, Warning, TEXT("Enemy::HP : %.f"), Hp);
	//GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, FString::Printf(TEXT("HP : %.d"), Hp));

	// 만약 체력이 0보다 크다면 데미지 상태로 전이하고 싶다
	if (Hp <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy::Hp == 0"));
		bEnemyDead = true;
		ClearFireTimer();
		OnDie();
	}
}

void AEnemy::OnDie()
{
	if (bEnemyDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy::OnDie()"));
		
		this->Destroy();
	}
}


