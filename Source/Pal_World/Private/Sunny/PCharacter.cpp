// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/PCharacter.h"
#include "Sunny/PBullet.h"
#include "Sunny/Enemy.h"
#include "Sunny/PalSlotUI.h"
#include "Sunny/PalInfo.h"
#include "Sunny/GunPlayerAnimInstance.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sunny/GunPlayerAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "TimerManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"


#include "Eddie/GrizzBolt.h"
#include "Justin/PalSphereActor.h"
#include "Justin/Components/PStatusComponent.h"
#include "Justin/Frameworks/PStatusLibrary.h"

//Testing Stuff
#include "Justin/PPalCharacter.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/ArrowComponent.h"


// Sets default values
APCharacter::APCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 카메라 설정
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SocketOffset = FVector(0, 70, 0);  // 카메라 오프셋 설정  

	// 카메라의 거리와 회전 중심 설정
	SpringArmComp->TargetArmLength = MaxArmLength;
	SpringArmComp->bUsePawnControlRotation = true; // 카메라가 캐릭터의 회전을 따르도록 설정
	//SpringArmComp->bEnableCameraLag = true; // 카메라 랙 활성화
	//SpringArmComp->CameraLagSpeed = 3.0f; // 카메라 랙 속도

	// 카메라 컴포넌트 생성 및 초기 설정
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	//CameraComp->SetRelativeRotation(FRotator(5, 5, 0));
	//CameraComp->SetRelativeRotation(FRotator(0, 0, 0));
	CameraComp->bUsePawnControlRotation = false;  // 카메라가 자체 회전을 하지 않도록 설정
	CameraComp->SetFieldOfView(OriginFieldView);

	// 총 컴포넌트 처리
	GunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	GunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_r"));
	GunMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Stamina Bar Widget 컴포넌트 추가
	StaminaWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("StaminaBar"));
	StaminaWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// FireNiaPos 컴포넌트 생성 및 GunMeshComp의 자식으로 설정
	FireNiaPos = CreateDefaultSubobject<UArrowComponent>(TEXT("FireNiaPos"));
	FireNiaPos->SetupAttachment(GunMeshComp); // GunMeshComp의 자식으로 설정


	// 캐릭터 메시 설정
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Sunny/Anim/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
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
		//GunMeshComp->SetRelativeLocationAndRotation(FVector(7.904235, 2.701636, 0.000000), FRotator(0.000000, 110.000000, 0.000000));
		GunMeshComp->SetRelativeLocationAndRotation(FVector(0.200000, -7.000000, -1.000000), FRotator(79.000000, 118.000000, 292.000000));
		GunMeshComp->SetRelativeScale3D(FVector(0.8f));;
	}
	//  메시에 애님 인스터스를 추가
	//ConstructorHelpers::FClassFinder<UGunPlayerAnimInstance> TempAnimInst(TEXT("/Script/Engine.AnimBlueprint'/Game/Sunny/Blueprints/Anim/ABP_GunPlayer.ABP_GunPlayer_C'"));
	ConstructorHelpers::FClassFinder<UGunPlayerAnimInstance> TempAnimInst(TEXT("/Script/Engine.AnimBlueprint'/Game/Sunny/Blueprints/Anim/ABP_GunKachujin.ABP_GunKachujin'_C'"));
	if (TempAnimInst.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(TempAnimInst.Class);
	}
	// Stamina Bar Widget Add
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempSteminaBar(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Sunny/UI/WBP_Stamina.WBP_Stamina_C'"));
	if (TempSteminaBar.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempSteminaBar.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 50, 0), FRotator(0, 0, 0));
	}

	// 체력 초기 설정
	Hp = MaxHp;
	StatusComponent = CreateDefaultSubobject<UPStatusComponent>(TEXT("StatusComponent"));
	//UE_LOG(LogTemp, Warning, TEXT("PCharacter::Hp init : %.f"), Hp);

	// 스테미나 초기 설정
	Stamina = MaxStamina;
	//UE_LOG(LogTemp, Warning, TEXT("PCharacter::Stemina init : %.f"), Stamina);

	// 총알 개수 설정
	SetLeftBullet(ReloadBullet);
	SetLeftMaxBullet(MaxBullet - ReloadBullet);
	//UE_LOG(LogTemp, Warning, TEXT("APCharacter()::SetLeftBullet(): %d"), GetLeftBullet());
	//UE_LOG(LogTemp, Warning, TEXT("APCharacter()::SetLeftMaxBullet(): %d"), GetLeftMaxBullet());
	//UE_LOG(LogTemp, Warning, TEXT("PCharacter::Stemina init : %.f"), Stamina);


	// 팰
	bCanInteractPal = true;
	CanInteractDelay = .6f;


	// Fire 이펙트 나이아가라 컴포넌트 추가
	FireNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireNiagara"));
	FireNiagara->SetupAttachment(GunMeshComp, TEXT("FirePosition"));
	FireNiagara->bAutoActivate = false; // 처음에는 비활성 상태로 설정
	FireNiagara->SetRelativeScale3D(FVector(0.5f)); // 필요에 따라 크기 조정

}

void APCharacter::SetTarget(AActor* Target)
{
	if (PalInfo)
	{
		PalInfo->GetSummonedPal()->SetTarget(Target);
	}
}

void APCharacter::KillSwitchPal()
{
	if (PalInfo)
	{
		PalInfo->KillSwitch();
	}
}


void APCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 카운트 다운 타이머 설정
	//GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &APCharacter::CountDown, 1.0f, true, 1.0f);

	// 컨트롤러를 가져와서 PlayerController인지 캐스팅
	PlayerController = Cast<APlayerController>(Controller);

	// 2. 캐스팅에 성공했다면
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		ULocalPlayer* localPlayer = PlayerController->GetLocalPlayer();
		if (localPlayer)
		{
			UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(localPlayer);
			if (subSys)
			{
				subSys->AddMappingContext(IMC_Player, 0);
			}
		}

		// 카메라의 피치(Pitch) 각도 제한 설정
		//PlayerController->PlayerCameraManager->ViewPitchMin = -65.0f; // 위로 -65도
		//PlayerController->PlayerCameraManager->ViewPitchMax = 50.f;  // 아래로 70도
	}

	// 태어날 때 조준선UI와 줌UI를 생성하고 싶다. 
	CrosshairUI = CreateWidget<UUserWidget>(GetWorld(), CrosshairUIClass);
	if (CrosshairUI)
	{
		CrosshairUI->AddToViewport();

		// CrosshairImage를 UI 위젯에서 가져오기
		CrosshairImage = Cast<UImage>(CrosshairUI->GetWidgetFromName(TEXT("Crosshair")));
		if (CrosshairImage)
		{
			InitCrosshairSize();
			CalcCrosshairAlpha(CrosshairOriginAlpha);
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("Failed to find Crosshair image in UI!"));
		}

		PalCircleImage = Cast<UImage>(CrosshairUI->GetWidgetFromName(TEXT("PalCircle")));
		PalAimImage = Cast<UImage>(CrosshairUI->GetWidgetFromName(TEXT("PalArrow")));
		if (PalCircleImage && PalAimImage)
		{
			HidePalAimUI();
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("Failed to find PalCircleImage & PalAimImage in UI!"));
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("Failed to create Crosshair UI!"));
	}

	// Show Gun
	//OnMyActionChoooseGun(FInputActionValue());
	OnMyActionChoooseGun();


	// Stamina widget setting
	if (StaminaWidgetComp)
	{
		//UE_LOG(LogTemp, Warning, TEXT("StaminaWidgetComp is valid"));

		UUserWidget* StaminaWidget = StaminaWidgetComp->GetUserWidgetObject();

		if (StaminaWidget)
		{
			//UE_LOG(LogTemp, Warning, TEXT("StaminaWidget is valid"));

			StaminaBar = Cast<UProgressBar>(StaminaWidget->GetWidgetFromName(TEXT("SteminaBar")));
			DamageBar = Cast<UProgressBar>(StaminaWidget->GetWidgetFromName(TEXT("DamageBar")));

			if (StaminaBar && DamageBar)
			{
				//UE_LOG(LogTemp, Warning, TEXT("StaminaBar is successfully cast"));
				SetStaminaPercent(Stamina, MaxStamina);
				HideDamageBar();
			}
			else
			{
				//UE_LOG(LogTemp, Error, TEXT("StaminaBar, DamageBar are null - Could not cast the widget"));
			}
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("StaminaWidget is not valid in APCharacter::BeginPlay"));
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("StaminaWidgetComp is not valid in APCharacter::BeginPlay"));
	}
	StaminaWidgetComp->SetVisibility(false);


	// Health widget setting
	if (HealthUIClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::HealthUIClass OK"));
		UUserWidget* HealthWidget = CreateWidget<UUserWidget>(GetWorld(), HealthUIClass);
		if (HealthWidget)
		{
			//UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::HealthWidget OK"));
			HealthWidget->AddToViewport();
			//UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::AddToViewport() OK"));

			HealthBar = Cast<UProgressBar>(HealthWidget->GetWidgetFromName(TEXT("HealthBar")));
			CurrentHealthLabel = Cast<UTextBlock>(HealthWidget->GetWidgetFromName(TEXT("CurrentHealthLabel")));
			MaxHealthLabel = Cast<UTextBlock>(HealthWidget->GetWidgetFromName(TEXT("MaxHealthLabel")));

			if (HealthBar && CurrentHealthLabel && MaxHealthLabel)
			{
				//UE_LOG(LogTemp, Warning, TEXT("HealthBar, CurrentHealthLabel, MaxHealthLabel is successfully cast"));
				SetHealthPercent(Hp, MaxHp);
			}
			else
			{
				//UE_LOG(LogTemp, Error, TEXT("HealthBar,CurrentHealthLabel, MaxHealthLabel is null - Could not cast the widget"));
			}
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("HealthWidget is not valid in APCharacter::BeginPlay"));
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("HealthUIClass is not valid in APCharacter::BeginPlay"));
	}


	// 무기 슬롯 설정
	if (WeaponUIClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::WeaponUIClass OK"));
		UUserWidget* WeaponWidget = CreateWidget<UUserWidget>(GetWorld(), WeaponUIClass);
		if (WeaponWidget)
		{
			//UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::WeaponWidget OK"));
			WeaponWidget->AddToViewport();
			//UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::AddToViewport() OK"));

			CurrentNumLabel = Cast<UTextBlock>(WeaponWidget->GetWidgetFromName(TEXT("CurNum")));
			MaxNumLabel = Cast<UTextBlock>(WeaponWidget->GetWidgetFromName(TEXT("MaxNum")));

			if (CurrentNumLabel && MaxNumLabel)
			{
				//UE_LOG(LogTemp, Warning, TEXT("CurrentNumLabel, MaxNumLabel is successfully cast"));
				SetBulletNum(GetLeftBullet(), GetLeftMaxBullet());
				//UE_LOG(LogTemp, Warning, TEXT("BeginPlay()::SetLeftBullet(): %d"), GetLeftBullet());
				//UE_LOG(LogTemp, Warning, TEXT("BeginPlay()::SetLeftMaxBullet(): %d"), GetLeftMaxBullet());
			}
			else
			{
				//UE_LOG(LogTemp, Error, TEXT("CurrentNumLabel, MaxNumLabel is null - Could not cast the widget"));
			}
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("WeaponWidget is not valid in APCharacter::BeginPlay"));
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("WeaponUIClass is not valid in APCharacter::BeginPlay"));
	}


	// 카운트다운 타이머 설정
	//if (TimerUIClass)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::TimerUIClass OK"));
	//	UUserWidget* TimerWidget = CreateWidget<UUserWidget>(GetWorld(), TimerUIClass);
	//	if (TimerWidget)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::TimerWidget OK"));
	//		TimerWidget->AddToViewport();
	//		UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::AddToViewport() OK"));

	//		MinuteText = Cast<UTextBlock>(TimerWidget->GetWidgetFromName(TEXT("minutes")));
	//		SecondText = Cast<UTextBlock>(TimerWidget->GetWidgetFromName(TEXT("Seconds")));
	//		ZeroText = Cast<UTextBlock>(TimerWidget->GetWidgetFromName(TEXT("ZeroText")));  // <---------------------------- 추가

	//		if (MinuteText && SecondText && ZeroText)
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("MinuteText, SecondText is successfully cast"));
	//			ZeroText->SetVisibility(ESlateVisibility::Hidden);  // <---------------------------- 추가
	//			CountDown();
	//		}
	//		else
	//		{
	//			UE_LOG(LogTemp, Error, TEXT("MinuteText, SecondText is null - Could not cast the widget"));
	//		}
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("TimerWidget is not valid in APCharacter::BeginPlay"));
	//	}
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("TimerUIClass is not valid in APCharacter::BeginPlay"));
	//}


	// 팰 슬롯 설정
	if (!PalInfo && PalInfoClass)
	{
		// PalInfo 인스턴스가 할당되지 않은 경우 인스턴스 생성
		PalInfo = NewObject<UPalInfo>(this, PalInfoClass);
	}

	if (PalInfo)
	{
		//UE_LOG(LogTemp, Warning, TEXT("PCharacter::PalInfo OK"));
		PalInfo->InitializePalInfos(this);
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("PCharacter::PalInfo NOOOOOOOO"));
	}

	//  팰 인포 설정
	if (PalSlotUIClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("PCharacter::PalSlotUIClass OK"));
		//UUserWidget* pData = CreateWidget<UUserWidget>(GetWorld(), PalSlotUIClass);
		//if (pData)
		//{
		//	pData->AddToViewport();
		//}
		PalSlotUI = CreateWidget<UPalSlotUI>(GetWorld(), PalSlotUIClass);
		if (PalSlotUI)
		{
			//UE_LOG(LogTemp, Warning, TEXT("PCharacter::PalSlotUI OK"));
			PalSlotUI->AddToViewport();
			PalSlotUI->InitializeSlots(PalInfo);
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("Failed to create PalSlotUI in APCharacter::BeginPlay"));
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("PCharacter::PalSlotUIClass NOOOOOOOOO"));

	}

	// AnimInstance Setting
	Anim = Cast<UGunPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (!Anim)
	{
		UE_LOG(LogTemp, Error, TEXT("Anim is null in APCharacter::BeginPlay"));
	}


	SpringArmComp->SocketOffset = FVector(0, 70, 0);
	SpringArmComp->TargetArmLength = MaxArmLength;
	CameraComp->SetFieldOfView(OriginFieldView);

	// SpringArmComp 초기화 시 원래 SocketOffset 저장
	OriginSocketOffset = SpringArmComp->SocketOffset;
	ZoomedSocketOffset = OriginSocketOffset; // 초기화

}

// Called every frame
void APCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 이동
	FTransform dir = FTransform(GetControlRotation());
	Direction = dir.TransformVector(Direction);
	Direction.Z = 0;
	Direction.Normalize();
	AddMovementInput(Direction);
	Direction = FVector::ZeroVector;

	// Stamina Value Update
	
	if (bStaminaBarOn)
	{
		if (Stamina >= MaxStamina)
		{
			StopStaminaRecovery();

			if (!bUsedStaminaShift && !bUsedStaminaCtrl)
			{
				//HideStaminaBar();
				bStaminaBarOn = false;
				StaminaWidgetComp->SetVisibility(false);
				/*FTimerHandle Handle;
				GetWorld()->GetTimerManager().SetTimer(HideStaminaBarHandle, this, &APCharacter::HideStaminaBar, 0.2f, false);*/
			}
		}
		else
		{
			if (!bUsedStaminaCtrl && !bUsedStaminaShift)
			{
				StartStaminaRecovery();
			}
		}
	}
	
	// Health Value Update
	if (Hp == MaxHp)
	{
		StopHealthRecovery();
	}

	// 카메라 줌 처리 (마우스 우클릭)
	if (bZoomOn)
	{	
		if (bIsZooming)
		{
			float CurrentFOV = CameraComp->FieldOfView;
			float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, InterpSpeed);
			CameraComp->SetFieldOfView(NewFOV);
			// FieldOfView 로그 출력
			//UE_LOG(LogTemp, Log, TEXT("Current FOV: %f"), CameraComp->FieldOfView);

			// SpringArm의 SocketOffset도 부드럽게 보간
			FVector CurrentSocketOffset = SpringArmComp->SocketOffset;
			FVector NewSocketOffset = FMath::VInterpTo(CurrentSocketOffset, ZoomedSocketOffset, DeltaTime, InterpSpeed);
			SpringArmComp->SocketOffset = NewSocketOffset;


			// 목표 FOV에 거의 도달했으면 멈춤
			if (FMath::IsNearlyEqual(NewFOV, TargetFOV, 0.1f))
			{
				bIsZooming = false;
			}
		}
	}
	else
	{
		float CurrentFOV = CameraComp->FieldOfView;
		float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, InterpSpeed);
		CameraComp->SetFieldOfView(NewFOV);

		// SpringArm의 SocketOffset도 부드럽게 보간
		FVector CurrentSocketOffset = SpringArmComp->SocketOffset;
		FVector NewSocketOffset = FMath::VInterpTo(CurrentSocketOffset, ZoomedSocketOffset, DeltaTime, InterpSpeed);
		SpringArmComp->SocketOffset = NewSocketOffset;

		// FieldOfView 로그 출력
		//UE_LOG(LogTemp, Log, TEXT("Current FOV: %f"), CameraComp->FieldOfView);
	}

}

// Called to bind functionality to input
void APCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (input)
	{
		// 이동
		input->BindAction(IA_Move, ETriggerEvent::Triggered, this, &APCharacter::OnMyActionMove);
		input->BindAction(IA_Move, ETriggerEvent::Completed, this, &APCharacter::OnMyActionMoveStop);
		input->BindAction(IA_Rotate, ETriggerEvent::Triggered, this, &APCharacter::OnMyActionRotate);
		input->BindAction(IA_Jump, ETriggerEvent::Started, this, &APCharacter::OnMyActionJump);
		input->BindAction(IA_Run, ETriggerEvent::Started, this, &APCharacter::OnMyActionRun);
		input->BindAction(IA_Run, ETriggerEvent::Completed, this, &APCharacter::OnMyActionWalk);
		input->BindAction(IA_DiveRoll, ETriggerEvent::Started, this, &APCharacter::OnMyActionDiveRoll);
		input->BindAction(IA_DiveRoll, ETriggerEvent::Completed, this, &APCharacter::OnMyActionDiveRollDone);

		// 발사
		input->BindAction(IA_Fire, ETriggerEvent::Started, this, &APCharacter::OnMyActionFireOn);
		input->BindAction(IA_Fire, ETriggerEvent::Completed, this, &APCharacter::OnMyActionFireOff);

		// 장전
		input->BindAction(IA_Reload, ETriggerEvent::Started, this, &APCharacter::OnMyActionReload);

		// 카메라 줌
		input->BindAction(IA_Zoom, ETriggerEvent::Triggered, this, &APCharacter::OnMyActionZoom);
		input->BindAction(IA_Zoom, ETriggerEvent::Completed, this, &APCharacter::OnMyActionZoomRelease);

		// 무기 선택 (총)
		//input->BindAction(IA_ChooseGun, ETriggerEvent::Started, this, &APCharacter::OnMyActionChoooseGun);

		// 슬롯
		input->BindAction(IA_PalSlotMoveRigth, ETriggerEvent::Started, this, &APCharacter::PalSlotMoveRigth);
		input->BindAction(IA_PalSlotMoveLeft, ETriggerEvent::Started, this, &APCharacter::PalSlotMoveLeft);

		// 팰 소환
		input->BindAction(IA_PalSlotSelect, ETriggerEvent::Triggered, this, &APCharacter::SpawnSelectPal);
		input->BindAction(IA_PalSlotSelect, ETriggerEvent::Completed, this, &APCharacter::ThrowSelectPal);
	}

}


void APCharacter::OnMyActionMove(const FInputActionValue& Value)
{
	// bWalking를 true로 바꾸기
	CheckPlayerMovement();

	CalcCrosshairSize(CrosshairMoveSize);
	//bMoveCrosshair = true;

	FVector2D v = Value.Get<FVector2D>();

	// 상하 입력 이벤트 처리
	Direction.X = v.X;

	// 좌우 입력 이벤트 처리
	Direction.Y = v.Y;
	Direction.Normalize();

	// Direction 값을 로그로 출력
	//UE_LOG(LogTemp, Log, TEXT("Direction.X: %f, Direction.Y: %f"), Direction.X, Direction.Y);

	// bWalking를 false로 바꾸기
	CheckPlayerMovement();
}

void APCharacter::OnMyActionMoveStop(const struct FInputActionValue& Value)
{
	//bMoveCrosshair = false;
	InitCrosshairSize();
}

void APCharacter::CheckPlayerMovement()
{
	//UE_LOG(LogTemp, Log, TEXT("before bWalking: %s"), bWalking ? TEXT("true") : TEXT("false"));

	bWalking = !bWalking;
	//UE_LOG(LogTemp, Log, TEXT("after bWalking: %s"), bWalking ? TEXT("true") : TEXT("false"));
}

void APCharacter::OnMyActionJump(const FInputActionValue& Value)
{
	Jump();
}
void APCharacter::OnMyActionRun(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

	bUsedStaminaShift = true;
	if (bUsedStaminaShift)
	{
		bStaminaBarOn = true;
		StaminaWidgetComp->SetVisibility(true);
		StartStaminaDrainTimer();
	}

}
void APCharacter::OnMyActionWalk(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	if (bUsedStaminaShift)
	{
		bUsedStaminaShift = false;
		//StaminaWidgetComp->SetVisibility(false);
		StopStaminaDrainTimer();
	}
}
void APCharacter::OnMyActionDiveRoll(const FInputActionValue& Value)
{
	check(Anim);
	if (Anim)
	{
		Anim->PlayDiveRollMontage();
	}

	if (!bUsedStaminaCtrl)
	{
		bUsedStaminaCtrl = true;
		bStaminaBarOn = true;
		StaminaWidgetComp->SetVisibility(true);
		DrainStamina(StaminaDrainRateCtrl);
	}

}

void APCharacter::OnMyActionDiveRollDone(const FInputActionValue& Value)
{
	if (bUsedStaminaCtrl)
	{
		bUsedStaminaCtrl = false;
		//StaminaWidgetComp->SetVisibility(false);
	}
}

// Mouse Left Click
void APCharacter::OnMyActionFireOn(const FInputActionValue& Value)
{
	if (bPlayerDead)
	{
		return;
	}
	// 총알 수 갱신
	CalcBulletNum(GetLeftBullet(), GetLeftMaxBullet());


	// AnimInstance의 PlayFrieMontage함수를 호출하고 싶다.
	check(Anim);  // <-- 버그 찾을 때 유용
	if (Anim)
	{
		Anim->PlayFireMontage();
	}

	// 발사 소리
	check(FireSFX);
	if (FireSFX)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), FireSFX);

		// 총알 공장에 총알 생성 및 배치
		FTransform firePos = GunMeshComp->GetSocketTransform(TEXT("FirePosition"));

		// Fire 이펙트 나이아가라 컴포넌트 비활성화 후 활성화
		if (FireNiagara && FireNiagara->IsActive())
		{
			DeactivateNiagaraEffect();
		}
		FVector Location = FireNiaPos->GetComponentLocation();
		FRotator Rotation = FireNiaPos->GetComponentRotation();
		if (FireNiagara) {
			// RootComponent의 위치와 회전에 Niagara 이펙트 재생
			FireNiagara->SetWorldLocationAndRotation(Location, Rotation);
			FireNiagara->Activate(); // Niagara 이펙트 활성화

			// 일정 시간 후에 이펙트 비활성화
			FTimerHandle NiagaraTimerHandle;
			GetWorldTimerManager().SetTimer(NiagaraTimerHandle, this, &APCharacter::DeactivateNiagaraEffect, 0.1f, false);
		}

		// Line Trace 
		FHitResult OutHit;
		FVector Start = firePos.GetLocation() + firePos.GetRotation().RotateVector(FVector::ForwardVector * 100);
		FVector End = Start + CameraComp->GetForwardVector() * 100000.f;
		ECollisionChannel TraceChannel = ECC_GameTraceChannel1;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);


		bool bHit = GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, TraceChannel, Params);

		if (bHit)
		{
			// 바라본 곳에 뭔가 있다
			//DrawDebugLine(GetWorld(), Start, OutHit.ImpactPoint, FColor::Red, false, 3.f);

			// 폭발VFX를 표현하고 싶다.
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletImpactVFXFactory, OutHit.ImpactPoint);

			UPStatusLibrary::ApplyDamage(this, OutHit.GetActor(), OutHit.ImpactPoint, Damege);
			// 총알 생성
			//APBullet* bullet = GetWorld()->SpawnActor<APBullet>(BulletFatory, firePos);

			// 총알 발사 방향 설정
			/*if (bullet)
			{
				FVector FireDirection = CameraComp->GetForwardVector();
				bullet->FireInDirection(FireDirection);
			}*/

			check(CrosshairUI);
			if (CrosshairUI)
			{
				//UE_LOG(LogTemp, Warning, TEXT("APCharacter::OnMyActionFireOn::CrosshairUI OK"));
				CalcCrosshairSize(CrosshairFireSize);
				CalcCrosshairAlpha(CrosshairFireAlpha);
			}
			else
			{
				//UE_LOG(LogTemp, Error, TEXT("APCharacter::OnMyActionFireOn::CrosshairUI No"));
			}
		}
	}
}

void APCharacter::OnMyActionFireOff(const FInputActionValue& Value)
{
	check(CrosshairUI);
	if (CrosshairUI)
	{
		//UE_LOG(LogTemp, Warning, TEXT("APCharacter::OnMyActionFireOff::CrosshairUI OK"));

		if (CrosshairImage)
		{
			//UE_LOG(LogTemp, Warning, TEXT("APCharacter::OnMyActionFireOff::CrosshairUI::CrosshairImage OK"));
			InitCrosshairSize();
			CalcCrosshairAlpha(CrosshairOriginAlpha);
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("Failed to find Crosshair image in UI!"));
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("Failed to create Crosshair UI!"));
	}
}


void APCharacter::ShowGunMesh()
{
	GunMeshComp->SetVisibility(true);
}
void APCharacter::HideGunMesh()
{
	GunMeshComp->SetVisibility(false);
}


void APCharacter::DeactivateNiagaraEffect()
{
	if (FireNiagara && FireNiagara->IsActive())
	{
		FireNiagara->Deactivate();
	}
}

void APCharacter::OnMyActionChoooseGun()
{
	//CameraComp->SetFieldOfView(100.f);
	CrosshairUI->SetVisibility(ESlateVisibility::Visible);
}

// Mouse Right Click
void APCharacter::OnMyActionZoom(const FInputActionValue& Value) 
{
	//UE_LOG(LogTemp, Log, TEXT("APCharacter::OnMyActionZoom()"));

	// 인풋 값으로 BS_Anim의 Horizontal Axis Value 조절하기
	float aimValue = Value.Get<float>(); 
	//UE_LOG(LogTemp, Log, TEXT("APCharacter::OnMyActionZoom() with AimValue: %f"), aimValue);

	// AnimInstance의 값을 업데이트
	check(Anim)
	if (Anim)
	{
		Anim->SetAimValue(aimValue); // AnimInstance에서 조준 애니메이션을 제어하는 변수를 업데이트
	}

	CameraZoomIn();
	UpdateCharaterRotation();
	CalcCrosshairSize(CrosshairZoomSize);
}

void APCharacter::OnMyActionZoomRelease(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Log, TEXT("APCharacter::OnMyActionZoomRelease()"));

	float aimValue = Value.Get<float>(); // Input Action의 값을 float로 가져옴
	//UE_LOG(LogTemp, Log, TEXT("APCharacter::OnMyActionZoomRelease() with AimValue: %f"), aimValue);

	// 애니메이션의 값을 초기화
	check(Anim)
	if (Anim)
	{
		Anim->SetAimValue(0.0f); // 다시 서 있는 상태로 돌아가도록 설정
	}

	CameraZoomOut();
	InitCrosshairSize();	
}

// 조준, 구르기 시, 플레이어 몸 방향 회전
void APCharacter::UpdateCharaterRotation()
{
	if (bZoomOn || bSummonPal) // 조준 중일 때만 회전하도록 조건을 설정
	{
		FRotator ControlRotation = GetControlRotation();
		ControlRotation.Pitch = 0.0f; // Pitch를 0으로 설정해 몸의 회전이 상하로 움직이지 않게 함
		ControlRotation.Roll = 0.0f; // Roll을 0으로 설정해 회전을 안정적으로 유지
		SetActorRotation(ControlRotation);
	}
}

// 카메라 줌 인/아웃
void APCharacter::CameraZoomIn()
{
	if (!bZoomOn && !bIsZooming)
	{
		TargetFOV = ZoomFieldView;
		InterpSpeed = 5.f;  // 2초 내에 100에서 40으로 줄어들기 위해 속도 조정
		bZoomOn = true;

		// SpringArm의 SocketOffset을 줌인 값으로 변경
		OriginSocketOffset = SpringArmComp->SocketOffset;
		ZoomedSocketOffset = FVector(0.0f, 40.0f, 0.0f);

		bIsZooming = true;
	}
}
void APCharacter::CameraZoomOut()
{
	if (bZoomOn)
	{
		TargetFOV = OriginFieldView;
		InterpSpeed = 10.f;  // 빠르게 100으로 돌아가기 위해 더 빠른 속도로 설정
		bZoomOn = false;

		// SpringArm의 SocketOffset을 원래 값으로 변경
		ZoomedSocketOffset = OriginSocketOffset; // 원래의 SocketOffset 값

		bIsZooming = false;	
	}
}

// Bullet Reload
void APCharacter::OnMyActionReload(const struct FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("PCharacter::OnMyActionReload()"));
	if (GetLeftMaxBullet() <= 0 || GetLeftBullet() <= 0)
	{
		return;
	}

	// LeftBullet > 0   ReloadBullet - LeftBullet = InputReloadBullet 만큼만 추가
	int32 inputReloadBullet = ReloadBullet - GetLeftBullet();
	SetLeftMaxBullet(GetLeftMaxBullet() - inputReloadBullet);
	SetLeftBullet(GetLeftBullet() + inputReloadBullet);

	//UE_LOG(LogTemp, Warning, TEXT("OnMyActionReload()::SetLeftBullet(): %d"), GetLeftBullet());
	//UE_LOG(LogTemp, Warning, TEXT("OnMyActionReload()::SetLeftMaxBullet(): %d"), GetLeftMaxBullet());
	SetBulletNum(GetLeftBullet(), GetLeftMaxBullet());
}

// Weapon Slot Value Update
void APCharacter::SetBulletNum(int32 InLeftBullet, int32 InLeftMaxBullet)
{
	//UE_LOG(LogTemp, Warning, TEXT("PCharacter::SetBulletNum()"));
	if (CurrentNumLabel)
	{
		if (InLeftBullet <= 0)
		{
			InLeftBullet = 0;
		}
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0);
		CurrentNumLabel->SetText(FText::AsNumber(InLeftBullet, &Opts));
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("CurrentNumLabel is null"));
	}

	if (MaxNumLabel)
	{
		if (InLeftMaxBullet <= 0)
		{
			InLeftMaxBullet = 0;
		}
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0);
		MaxNumLabel->SetText(FText::AsNumber(InLeftMaxBullet, &Opts));
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("MaxNumLabel is null"));
	}
}

void APCharacter::CalcBulletNum(int32 InLeftBullet, int32 InLeftMaxBullet)
{
	//UE_LOG(LogTemp, Warning, TEXT("PCharacter::CalcBulletNum()"));
	if (GetLeftBullet() == 0 && GetLeftMaxBullet() == 0)
	{
		return;
	}

	// LeftMaxBullet Setting
	if (InLeftMaxBullet > 0)
	{
		SetLeftMaxBullet(InLeftMaxBullet);
	}
	else
	{
		SetLeftMaxBullet(0);
		// ActionFireOn() 발사 안 되고, 틱틱 소리만 나도록 추가하기  <===============================  베타 추가 작업 
	}

	// LeftBullet setting
	if (InLeftBullet > 1)
	{
		// 1보다 크면 1 감소
		int32 value = InLeftBullet - 1;
		SetLeftBullet(value);
	}
	else if (InLeftBullet == 1)
	{
		// 1이면 1 감소
		int32 value = InLeftBullet - 1;
		// 0이 되니까 재장전
		SetLeftBullet(value);
	}
	//UE_LOG(LogTemp, Warning, TEXT("CalcBulletNum()::InLeftBullet >= 1  --> SetLeftBullet(): %d"), GetLeftBullet());
	//UE_LOG(LogTemp, Warning, TEXT("CalcBulletNum()::InLeftBullet >= 1  --> SetLeftMaxBullet(): %d"), GetLeftMaxBullet());
	SetBulletNum(GetLeftBullet(), GetLeftMaxBullet());


	if (GetLeftBullet() == 0)
	{
		// 1초 대기
		GetWorldTimerManager().SetTimer(AutoReloadTimerHandle, this, &APCharacter::AutoReloadDelayTimer, 0.2f, false);
	}

}

void APCharacter::AutoReloadDelayTimer()
{
	/*if (GetWorldTimerManager().IsTimerActive(AutoReloadTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(AutoReloadTimerHandle);
	}*/

	if (GetLeftBullet() == 0)
	{
		// LeftBullet 0이면
		if (GetLeftMaxBullet() >= 30)
		{
			// LeftMaxBullet >= 30이면 자동으로 장전되고
			SetLeftBullet(ReloadBullet);
			// LeftMax 에서 장전만큼 빼기
			int32 value = GetLeftMaxBullet() - ReloadBullet;
			SetLeftMaxBullet(value);
		}
		else if (GetLeftMaxBullet() > 0)
		{
			// 0 < LeftMaxBullet < 30 이면, LeftMaxBullet만큼 자동 장전
			int32 value = GetLeftMaxBullet();
			SetLeftBullet(value);
			// LeftMaxBullet은 0 넣기
			SetLeftMaxBullet(0);
		}
		else
		{
			// LeftMaxBullet == 0이면 장전 불가
			SetLeftBullet(0);
			SetLeftMaxBullet(0);
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("CalcBulletNum()::InLeftBullet == 0  --> SetLeftBullet(): %d"), GetLeftBullet());
	//UE_LOG(LogTemp, Warning, TEXT("CalcBulletNum()::InLeftBullet == 0  --> SetLeftMaxBullet(): %d"), GetLeftMaxBullet());
	SetBulletNum(GetLeftBullet(), GetLeftMaxBullet());
}

// 마우스 방향으로 회전
void APCharacter::OnMyActionRotate(const FInputActionValue& Value)
{
	//FVector2D v = Value.Get<FVector2D>();
	FVector2D LookValue = Value.Get<FVector2D>();

	if (bPlayerDead) return;

	// 멈췄을 때와 움직일 때 카메라 움직이 제어     
	if (!bWalking)
	{
		// 현재 컨트롤러의 피치 값을 가져옴
		FRotator ControlRotation = GetControlRotation();
		float NewPitch = ControlRotation.Pitch + LookValue.Y;
		//UE_LOG(LogTemp, Log, TEXT("NewPitch: %f"), NewPitch);

		// 위로는 플레이어 기준 90도 꺾어짐
		if (NewPitch > DownMinAngle)
		{
			NewPitch = DownMinAngle;
		}

		// 아래로는 허벅지 정도까지 내려가고 각도가 -60도 정도 꺾이며, 플레이어 쪽으로 점점 가까워짐
		if (NewPitch < -1 * UpMinAngle)
		{
			NewPitch = -1 * UpMinAngle;
		}

		// 피치 값 설정
		ControlRotation.Pitch = NewPitch;

		// 위쪽 피치와 아래쪽 피치를 나눠서 계산
		if (NewPitch >= 0)
		{
			float NormalizedPitch = NewPitch / DownMinAngle;
			TargetArmLength = FMath::Lerp(MaxArmLength, DownMinArmLength, NormalizedPitch);
			SpringArmComp->TargetArmLength = TargetArmLength;
			//UE_LOG(LogTemp, Log, TEXT("ReNewPitch: %f, DownMinArmLength: %f"), NewPitch, TargetArmLength);

			// 자연스러운 SocketOffset 전환
			FVector TargetOffset = FMath::Lerp(FVector(0, 70, 0), FVector(0, 20, 0), NormalizedPitch);
			SpringArmComp->SocketOffset = TargetOffset;
			//UE_LOG(LogTemp, Log, TEXT("Down SocketOffset: X=%f, Y=%f, Z=%f"), TargetOffset.X, TargetOffset.Y, TargetOffset.Z);
		}
		else
		{
			float NormalizedPitch = FMath::Abs(NewPitch) / UpMinAngle;
			TargetArmLength = FMath::Lerp(MaxArmLength, UpMinArmLength, NormalizedPitch);
			SpringArmComp->TargetArmLength = TargetArmLength;
			//UE_LOG(LogTemp, Log, TEXT("ReNewPitch: %f, UpMinArmLength: %f"), NewPitch, TargetArmLength);

			// 자연스러운 SocketOffset 전환
			FVector TargetOffset = FMath::Lerp(FVector(0, 70, 0), FVector(0, 70, 60), NormalizedPitch);
			SpringArmComp->SocketOffset = TargetOffset;
			//HUE_LOG(LogTemp, Log, TEXT("Up SocketOffset: X=%f, Y=%f, Z=%f"), TargetOffset.X, TargetOffset.Y, TargetOffset.Z);
		}

		// Yaw는 기존 로직을 그대로 사용
		ControlRotation.Yaw += LookValue.X;
		// 새로운 컨트롤러 회전 값 설정
		GetController()->SetControlRotation(ControlRotation);

	}
	else
	{
		// 마우스 X 입력을 Yaw에 반영 (좌우 회전)
		AddControllerYawInput(LookValue.X);
		// 마우스 Y 입력을 Pitch에 반영 (상하 회전)
		AddControllerPitchInput(-LookValue.Y);


		// 플레이어의 현재 속도를 가져옴
		float CurrentSpeed = GetVelocity().Size();

		// 걷기와 뛰기 속도에 따른 카메라 거리 조정
		float SpeedRatio = FMath::Clamp((CurrentSpeed - WalkSpeed) / (RunSpeed - WalkSpeed), 0.0f, 1.0f);

		// 타겟 암 길이를 걷기와 뛰기 속도에 따라 보간
		float AdjustedArmLength = FMath::Lerp(200.0f, 250.0f, SpeedRatio); // 200과 250은 예시 값으로 조정 가능
		SpringArmComp->TargetArmLength = AdjustedArmLength;

		// 카메라의 오프셋도 속도에 따라 보간 가능 (필요한 경우)
		//FVector TargetOffset = FMath::Lerp(FVector(-2, 40, 70), FVector(-2, 60, 90), SpeedRatio);
		//SpringArmComp->SocketOffset = TargetOffset;

		// 카메라를 플레이어의 뒤쪽으로 유지
		FRotator PlayerRotation = GetActorRotation();
		PlayerRotation.Pitch = 0;  // 플레이어의 피치는 보통 0으로 유지
		SpringArmComp->SetRelativeRotation(PlayerRotation);
	}
}

void APCharacter::CalcCrosshairSize(float Insize)
{
	//UE_LOG(LogTemp, Warning, TEXT("APCharacter::CalcCrosshairSize()"));
	// 현재 CrosshairImage의 크기 가져오기
	if (CrosshairImage)
	{
		//GetWorldTimerManager().SetTimer(AutoReloadTimerHandle, this, &APCharacter::AutoReloadDelayTimer, 0.f, false);
		//const FSlateBrush& Brush = CrosshairImage->GetBrush(); // GetBrush()를 통해 참조를 가져옴
		//InitialSize = Brush.ImageSize;
		//UE_LOG(LogTemp, Warning, TEXT("Current Crosshair Size: X = %f, Y = %f"), InitialSize.X, InitialSize.Y);

		//float ChangedSizeX = InitialSize.X * Insize;
		//float ChangedSizeY = InitialSize.Y * Insize;

		//TargetSize = FVector2D(ChangedSizeX, ChangedSizeY);
		//CrosshairImage->SetDesiredSizeOverride(TargetSize);
		//UE_LOG(LogTemp, Warning, TEXT("Changed Crosshair Size: X = %f, Y = %f"), ChangedSizeX, ChangedSizeY);

		// 크기 변화 시작
		//bIsCrosshairResizing = true;


		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CrosshairImage->Slot))
		{
			FVector2D currentSize = CanvasSlot->GetSize();
			FVector2D targetSize = InitialSize * Insize;
			if (currentSize == targetSize)
			{
				return;
			}
			else
			{
				//FVector2D newSize = currentSize * Insize;
				CanvasSlot->SetSize(targetSize);
			}

		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("CrosshairImage is null! Cannot get size."));
	}
}
void APCharacter::InitCrosshairSize()
{
	//UE_LOG(LogTemp, Warning, TEXT("APCharacter::InitCrosshairSize()"));

	//// 목표 크기를 원래 크기로 설정
	//InitialSize = FVector2D(CrosshairSize, CrosshairSize);
	//TargetSize = InitialSize;
	//CrosshairImage->SetDesiredSizeOverride(TargetSize);
	//UE_LOG(LogTemp, Warning, TEXT("Init Crosshair Size: X = %f, Y = %f"), TargetSize.X, TargetSize.Y);

	// 크기 복구 시작
	//bIsCrosshairResizing = true;


	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CrosshairImage->Slot))
	{
		CanvasSlot->SetSize(FVector2D(CrosshairOriginSize, CrosshairOriginSize));
	}
}

void APCharacter::ShowPalAimUI()
{
	PalCircleImage->SetVisibility(ESlateVisibility::Visible);
	PalAimImage->SetVisibility(ESlateVisibility::Visible);
	CrosshairImage->SetVisibility(ESlateVisibility::Hidden);
}

void APCharacter::HidePalAimUI()
{
	PalCircleImage->SetVisibility(ESlateVisibility::Hidden);
	PalAimImage->SetVisibility(ESlateVisibility::Hidden);
	CrosshairImage->SetVisibility(ESlateVisibility::Visible);
}

void APCharacter::CalcCrosshairAlpha(float InAlpha)
{
	// 투명도 변경
	FLinearColor NewColor = CrosshairImage->ColorAndOpacity;
	NewColor.A = InAlpha;
	CrosshairImage->SetColorAndOpacity(NewColor);
	//UE_LOG(LogTemp, Warning, TEXT("Changed Crosshair Alpha: %f"), NewColor.A);
}



// 피격 시 체력 감소
void APCharacter::OnPlayerTakeDamage(float Damage, AEnemy* Enemy /*= nullptr*/)
{
	//UE_LOG(LogTemp, Warning, TEXT("Player::OnMyTakeDamage()"));
	// 총에 맞으면 체력을 100 감소
	Hp -= Damage;
	//UE_LOG(LogTemp, Warning, TEXT("Player::HP : %.1f"), Hp);
	SetHealthPercent(Hp, MaxHp);
	StartHealthRecovery();

	//GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, FString::Printf(TEXT("HP : %.d"), Hp));

	// 만약 체력이 0보다 작으면 GameOver() 호출
	if (Hp <= 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Player::Hp == 0"));
		bPlayerDead = true;
		StopHealthRecovery();

		check(Anim);
		if (Anim)
		{
			Anim->PlayDeathMontage();
		}

		if (Enemy)
		{
			Enemy->ClearFireTimer();
		}
		OnGameOver();
	}

}


// 게임 오버 시
void APCharacter::OnGameOver()
{
	//UE_LOG(LogTemp, Warning, TEXT("pCharacter::OnGameOver()"));
	//if (bPlayerDead || bTimeOver)
	//{
	//	//this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//	//UGameplayStatics::SetGamePaused(GetWorld(), true);
	//	ShowDefeatUI();
	//}

	/*StaminaWidgetComp->SetVisibility(false);
	GetMesh()->SetCollisionProfileName("NoCollision");

	PrimaryActorTick.SetTickFunctionEnable(false);
	SetActorTickEnabled(false);*/

	//UE_LOG(LogTemp, Warning, TEXT("End Game!!!"));
}


// 체력 값 설정
void APCharacter::SetHealthPercent(float InHealth, float InMaxHealth)
{
	//UE_LOG(LogTemp, Warning, TEXT("PCharacter::SetHealthPercent()"));

	if (HealthBar)
	{
		float Percent = InHealth / InMaxHealth;
		HealthBar->SetPercent(Percent);
		//UE_LOG(LogTemp, Warning, TEXT("HealthBar->SetPercent called with Percent: %f"), Percent);
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("HealthBar is null"));
	}

	if (CurrentHealthLabel)
	{
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0);
		if (Hp <= 0)
		{
			Hp = 0;
		}
		CurrentHealthLabel->SetText(FText::AsNumber(Hp, &Opts));
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("CurrentHealthLabel is null"));
	}

	if (MaxHealthLabel)
	{
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0);
		MaxHealthLabel->SetText(FText::AsNumber(MaxHp, &Opts));
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("MaxHealthLabel is null"));
	}
}

// 체력 회복 타이머 설정
void APCharacter::StartHealthRecovery()
{
	//UE_LOG(LogTemp, Log, TEXT("PCharacter::StartHealthRecovery()"));
	if (!GetWorldTimerManager().IsTimerActive(HealthRecoveryTimerHandle))
	{
		// RecoverStamina(float)를 호출하는 람다 함수 생성
		auto RecoveryLambda = [this]() { RecoverHealth(HealthRecoveryRate); };

		// 람다 함수를 인수로 전달
		GetWorldTimerManager().SetTimer(HealthRecoveryTimerHandle, RecoveryLambda, 1.0f, true);
	}
}
void APCharacter::StopHealthRecovery()
{
	//UE_LOG(LogTemp, Log, TEXT("PCharacter::StopHealthRecovery()"));
	if (GetWorldTimerManager().IsTimerActive(HealthRecoveryTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(HealthRecoveryTimerHandle);
	}
}
void APCharacter::RecoverHealth(float Value)
{
	//UE_LOG(LogTemp, Log, TEXT("PCharacter::RecoverHealth()"));
	Hp = FMath::Clamp(Hp + Value, 0.0f, MaxHp);
	SetHealthPercent(Hp, MaxHp);
}

// 패배 UI
void APCharacter::ShowDefeatUI()
{
	if (DefeatUIClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("PCharacter::OnGameOver()::DefeatUIClass OK"));
		//UUserWidget* DefeatWidget = CreateWidget<UUserWidget>(GetWorld(), DefeatUIClass);
		//if (DefeatWidget)
		{
			//UE_LOG(LogTemp, Warning, TEXT("PCharacter::OnGameOver()::DefeatWidget OK"));
			//DefeatWidget->AddToViewport();
		}
		//else
		{
			//UE_LOG(LogTemp, Warning, TEXT("PCharacter::OnGameOver()::DefeatWidget Nooooooooo"));
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("PCharacter::OnGameOver()::DefeatUIClass Noooooooooooooo"));
	}
}

// 스테미나 값 설정
void APCharacter::SetStaminaPercent(float InStamina, float InMaxStamina)
{
	//UE_LOG(LogTemp, Warning, TEXT("PCharacter::SetStaminaPercent()"));
	if (StaminaBar)
	{
		float Percent = InStamina / InMaxStamina;
		StaminaBar->SetPercent(Percent);
		//UE_LOG(LogTemp, Warning, TEXT("StaminaBar->SetPercent called with Percent: %f"), Percent);

		// 스테미나에 따른 색깔 변경
		FLinearColor BarColor = FLinearColor::White; // 기본 색상: 녹색

		if (Percent <= 0.4f)
		{
			BarColor = FLinearColor::Red; // 체력이 40% 이하일 때 빨간색으로 변경
		}
		else if (Percent <= 0.7f)
		{
			BarColor = FLinearColor::Yellow; // 체력이 40~70%일 때 노란색으로 변경
		}

		StaminaBar->SetFillColorAndOpacity(BarColor);
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("StaminaBar is null"));
	}
}

void APCharacter::StartStaminaRecovery()
{
	//UE_LOG(LogTemp, Log, TEXT("PCharacter::StartStaminaRecovery()"));
	if (!GetWorldTimerManager().IsTimerActive(StaminaRecoveryTimerHandle))
	{
		// RecoverStamina(float)를 호출하는 람다 함수 생성
		auto RecoveryLambda = [this]() { RecoverStamina(StaminaRecoveryRate); };

		// 람다 함수를 인수로 전달
		GetWorldTimerManager().SetTimer(StaminaRecoveryTimerHandle, RecoveryLambda, 0.5f, true);
	}
}

void APCharacter::StopStaminaRecovery()
{
	//UE_LOG(LogTemp, Log, TEXT("PCharacter::StopStaminaRecovery()"));
	if (GetWorldTimerManager().IsTimerActive(StaminaRecoveryTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(StaminaRecoveryTimerHandle);
	}
	SetStaminaPercent(Stamina, MaxStamina);
}

void APCharacter::RecoverStamina(float Value)
{
	//UE_LOG(LogTemp, Log, TEXT("PCharacter::RecoverStamina()"));
	Stamina = FMath::Clamp(Stamina + Value, 0.0f, MaxStamina);
	SetStaminaPercent(Stamina, MaxStamina);
	//UE_LOG(LogTemp, Log, TEXT("Stamina recovered: %f, current stamina: %f"), Value, Stamina);
}

void APCharacter::DrainStamina(float Value)
{
	//UE_LOG(LogTemp, Log, TEXT("PCharacter::DrainStamina()"));
	if (bUsedStaminaCtrl || bUsedStaminaShift)
	{
		ShowDamageBar(Stamina, MaxStamina);
		float SetStamina = FMath::Clamp(Stamina - Value, 0.0f, MaxStamina);
		Stamina = SetStamina;
		//UE_LOG(LogTemp, Log, TEXT("Stamina drained: %f, current stamina: %f"), Value, Stamina);
		SetStaminaPercent(Stamina, MaxStamina);
	}
}

void APCharacter::StartStaminaDrainTimer()
{
	if (!GetWorldTimerManager().IsTimerActive(StaminaDrainTimerHandle))
	{
		// RecoverStamina(float)를 호출하는 람다 함수 생성
		auto DrainLambda = [this]() { DrainStamina(StaminaDrainRateShift); };
		// 람다 함수를 인수로 전달
		GetWorldTimerManager().SetTimer(StaminaDrainTimerHandle, DrainLambda, 1.0f, true);
	}
}

void APCharacter::StopStaminaDrainTimer()
{
	if (GetWorldTimerManager().IsTimerActive(StaminaDrainTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(StaminaDrainTimerHandle);
	}
}


void APCharacter::ShowDamageBar(float InStamina, float InMaxStamina)
{

	DamageBar->SetVisibility(ESlateVisibility::Visible);
	if (DamageBar)
	{
		float Percent = InStamina / InMaxStamina;
		DamageBar->SetPercent(Percent);
		//UE_LOG(LogTemp, Log, TEXT("ShowDamageBar() ::  Stamina: %f, MaxStamina: %f, Percent: %f"), InStamina, InMaxStamina, Percent);
	}
	else
	{
		//UE_LOG(LogTemp, Log, TEXT("PCharacter::ShowDamageBar()::DamageBar NOoooooooo"));
	}

	FTimerHandle HideDanageBarTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(HideDanageBarTimerHandle, this, &APCharacter::HideDamageBar, 0.2f, false);
}

void APCharacter::HideDamageBar()
{
	DamageBar->SetVisibility(ESlateVisibility::Hidden);
}

void APCharacter::Destroyed()
{
	if(StatusComponent && StatusComponent->LowHealthWidget)
		StatusComponent->LowHealthWidget->RemoveFromParent();
	Super::Destroyed();
}


// 카운트 다운
//void APCharacter::CountDown()
//{
//	if (MinuteText && SecondText)
//	{
//		if (SecondNum > 0)
//		{
//			--SecondNum;
//			UE_LOG(LogTemp, Warning, TEXT("Seconds %.f"), SecondNum);
//
//			if (SecondNum < 10) 
//			{
//				ZeroText->SetVisibility(ESlateVisibility::Visible);
//			}
//			
//		}
//		else
//		{
//			--MinuteNum;
//			SecondNum = 11.f;
//			ZeroText->SetVisibility(ESlateVisibility::Hidden);
//			UE_LOG(LogTemp, Warning, TEXT("Minutes %d"), MinuteNum);
//
//			if (MinuteNum <= 0)
//			{
//				GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
//				SecondNum = 0.0f;
//				UE_LOG(LogTemp, Warning, TEXT("Timer Over"));
//			}
//		}
//
//		FNumberFormattingOptions SecOpts;
//		SecOpts.SetMaximumFractionalDigits(0);
//		SecondText->SetText(FText::AsNumber(SecondNum, &SecOpts));
//
//		FNumberFormattingOptions MinOpts;
//		MinOpts.SetMaximumFractionalDigits(0);
//		MinuteText->SetText(FText::AsNumber(MinuteNum, &MinOpts));
//
//		if (MinuteNum <= 0 && SecondNum <= 0)
//		{
//			bTimeOver = true;
//			OnGameOver();
//		}
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("APCharacter::CountDown()::NO MinuteText && SecondText"));
//	}
//
//}

// 팰 슬롯 좌우 이동
void APCharacter::PalSlotMoveRigth()
{
	if (PalSlotUI)
	{
		PalSlotUI->MoveRight();
	}
}
void APCharacter::PalSlotMoveLeft()
{
	if (PalSlotUI)
	{
		PalSlotUI->MoveLeft();
	}
}

void APCharacter::StartCanInteractTimer()
{
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &APCharacter::CanInteractElapsed, CanInteractDelay, false);
}

void APCharacter::CanInteractElapsed()
{
	bCanInteractPal = true;
}

// 팰 소환
void APCharacter::SpawnSelectPal(const FInputActionValue& Value)
{
	if (!PalSlotUI || !PalInfo)
	{
		return;
	}

	if (!bCanInteractPal) return;

	if (PalInfo->IsRetrievingPal())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Pal is already retrieving! Sparkle kira kira all the effects are active!"));
		return;
	}

	if (!PalInfo->IsPalSummoned() && PalSlotUI->IsCurrentPalDead())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Stop it!! He's already dead!"));
		return;
	}

	if (PalInfo->IsPalSummoned())
	{
		PalSlotUI->RetrievePal();

		// 여기 돌아오는 애니메이션
		if (bSummonPal)
		{
			bSummonPal = false;

			check(Anim)
				if (Anim)
				{
					Anim->PlayRetrieveAnim();
				}
		}
		bCanInteractPal = false;
		OnPalInteract.Broadcast(false);
		StartCanInteractTimer();
	}
	else
	{
		// 던지기
		if (!bSummonPal)
		{
			bSummonPal = true;
			UpdateCharaterRotation();
			ShowPalAimUI();
			HideGunMesh();


			check(Anim)
			if (Anim)
			{
				float holdValue = Value.Get<float>();
				Anim->PlayHoldAnim(holdValue);
			}
		}
	
	}
}

void APCharacter::ThrowSelectPal()
{
	if (bSummonPal)
	{
		check(Anim)
		if (Anim)
		{
			Anim->PlayThrowAnim();
		}
	}
}

void APCharacter::ThrowPal()
{
	if (PalSphereClass)
	{
		FTransform Trans;
		Trans.SetLocation(GetActorLocation());
		Trans.SetRotation(GetActorForwardVector().Rotation().Quaternion());
		auto PalSphere = GetWorld()->SpawnActorDeferred<APalSphereActor>(PalSphereClass, Trans, this);

		if (PalSphere)
		{
			PalSphere->Initialize(PalSlotUI);
			UGameplayStatics::FinishSpawningActor(PalSphere, Trans);
			PalSphere->BeginThrow();
			bCanInteractPal = false;
		}
	}

	FTimerHandle HideTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, this, &APCharacter::HidePalAimUI, 0.5f, false);
}

