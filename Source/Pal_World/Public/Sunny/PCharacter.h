// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PCharacter.generated.h"

class APalSphereActor;

DECLARE_MULTICAST_DELEGATE_OneParam(FPalInteractDelegate, bool /*bIsSummoned*/);

UCLASS()
class PAL_WORLD_API APCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APCharacter();
	
	//Testing Function
	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* Target);

	FPalInteractDelegate OnPalInteract;

	UFUNCTION(BlueprintCallable)
	void KillSwitchPal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



	// 카메라 ========================================================================
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* CameraComp;

	UPROPERTY(EditDefaultsOnly)
	class APlayerController* PlayerController;

	// 카메라와 플레이어의 거리 조정
	UPROPERTY(EditAnywhere)
	float UpMinArmLength = 190.0f;
	UPROPERTY(EditAnywhere)
	float UpMinAngle = 65.0f;

	UPROPERTY(EditAnywhere)
	float DownMinArmLength = 150.0f;
	UPROPERTY(EditAnywhere)
	float DownMinAngle = 50.0f;

	UPROPERTY(EditAnywhere)
	float MaxArmLength = 200.0f;
	UPROPERTY(EditDefaultsOnly)
	float TargetArmLength;

	UPROPERTY(EditAnywhere)
	float OriginFieldView = 90.f;
	UPROPERTY(EditAnywhere)
	float ZoomFieldView = 40.f;

	FVector ZoomedSocketOffset;
	FVector OriginSocketOffset;


	// 애니메이션 ====================================================================
	UPROPERTY()
	class UGunPlayerAnimInstance* Anim;



	// 키보드 입력 (이동) ============================================================
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_Player;

    // W, S, A, D (앞, 뒤, 좌, 우)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Move;
	FVector Direction;
	bool bWalking;
	bool bMoveCrosshair;

	void OnMyActionMove(const struct FInputActionValue& Value);
	void OnMyActionMoveStop(const struct FInputActionValue& Value);
	void CheckPlayerMovement();

	// Space (점프)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Jump;
	void OnMyActionJump(const FInputActionValue& Value);

	// Shift (뛰기)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Run;

	UPROPERTY(EditDefaultsOnly)
	float WalkSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float RunSpeed = 800.f;

	void OnMyActionRun(const FInputActionValue& Value);
	void OnMyActionWalk(const FInputActionValue& Value);

	// DiveRoll (구르기)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_DiveRoll;
	void OnMyActionDiveRoll(const FInputActionValue& Value);
	void OnMyActionDiveRollDone(const FInputActionValue& Value);



	// Mouse Left Click (총 쏘기) ==============================================================
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class USkeletalMeshComponent* GunMeshComp;

	//UPROPERTY(EditDefaultsOnly)
	//class UStaticMeshComponent* SniperMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Fire;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class APBullet> BulletFatory;

	void OnMyActionFireOn(const FInputActionValue& Value);
	void OnMyActionFireOff(const FInputActionValue& Value);

	void ShowGunMesh();
	void HideGunMesh();

	// 총알 자국 VFX
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	class UParticleSystem* BulletImpactVFXFactory;
	// 발사 VFX
	UPROPERTY(EditAnyWhere, Category = "FX")
	class UNiagaraComponent* FireNiagara;
	UPROPERTY(EditAnyWhere)
	class UArrowComponent* FireNiaPos;


	// 발사 소리
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	class USoundBase* FireSFX;

	void DeactivateNiagaraEffect();

	//UPROPERTY(EditDefaultsOnly)
	//class UInputAction* IA_ChooseGun;
	//void OnMyActionChoooseGun(const FInputActionValue& Value);
	void OnMyActionChoooseGun();

	// 마우스 입력 (카메라 회전)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Rotate;
	void OnMyActionRotate(const struct FInputActionValue& Value);



	// Zoom =============================================================================
	// Factory, 생성된 UI를 기억할 변수
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> CrosshairUIClass;

	UPROPERTY()
	class UUserWidget* CrosshairUI;
	UPROPERTY(meta = (BindWidget))
    class UImage* CrosshairImage;
	UPROPERTY(meta = (BindWidget))
    class UImage* PalCircleImage;
	UPROPERTY(meta = (BindWidget))
    class UImage* PalAimImage;

	FVector2D InitialSize = FVector2D(50, 50);  // 크기 변화 시작 시의 초기 크기
	float CrosshairOriginSize = 50.f;
	float CrosshairFireSize = 1.2f;
	float CrosshairMoveSize = 2.f;
	float CrosshairZoomSize = 0.7f;
	float CrosshairFireAlpha = 0.2f;
	float CrosshairOriginAlpha = 1.f;

	void CalcCrosshairSize(float InSize);
	void CalcCrosshairAlpha(float InAlpha);
	void InitCrosshairSize();

	void ShowPalAimUI();
	void HidePalAimUI();

	// Key, Functions
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Zoom;
	float TargetFOV = 90.f;
	float InterpSpeed;
	bool bZoomOn;
	bool bIsZooming;

	void OnMyActionZoom(const FInputActionValue& Value);
	void OnMyActionZoomRelease(const FInputActionValue& Value);

	void UpdateCharaterRotation();

	void CameraZoomIn();
	void CameraZoomOut();
	

	// Weapon Slot =====================================================================
	// Widget
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> WeaponUIClass;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentNumLabel;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MaxNumLabel;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Reload;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 LeftBullet;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 LeftMaxBullet;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 InputReloadBullet;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 ReloadBullet = 10;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 MaxBullet = 300;

	FTimerHandle AutoReloadTimerHandle;
	
	void OnMyActionReload(const struct FInputActionValue& Value);
	
	UFUNCTION()
	void SetBulletNum(int32 InLeftBullet, int32 InLeftMaxBullet);
	UFUNCTION()
	void CalcBulletNum(int32 InLeftBullet, int32 InLeftMaxBullet);
	UFUNCTION()
	void AutoReloadDelayTimer();

	int32 GetLeftBullet() const { return LeftBullet; }
	void SetLeftBullet(int32 val) { LeftBullet = val; }
	int32 GetLeftMaxBullet() const { return LeftMaxBullet; }
	void SetLeftMaxBullet(int32 val) { LeftMaxBullet = val; }

	
	
	// 체력 관리 ========================================================================
	// Widget
	UPROPERTY(EditAnywhere, Category = "UI")
	class UPStatusComponent* StatusComponent;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> DefeatUIClass;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> HealthUIClass;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentHealthLabel;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MaxHealthLabel;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float Hp;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHp = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float Damege = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float HealthRecoveryRate = 3.f;

	bool bPlayerDead = false;

	FTimerHandle HealthRecoveryTimerHandle;

	// 피격 당했을 때 처리
	UFUNCTION(BlueprintCallable, Category = "Health")
	void OnPlayerTakeDamage(float Damage = 100.f, AEnemy* Enemy = nullptr);
	// 게임 오버될 떄 호출되는 함수
	UFUNCTION(BlueprintCallable, Category = "Health")
	void OnGameOver();
	// Health Bar Setting Function
	UFUNCTION()
	void SetHealthPercent(float InHealth, float InMaxHealth);
	UFUNCTION()
	void StartHealthRecovery();
	UFUNCTION()
	void StopHealthRecovery();
	UFUNCTION()
	void RecoverHealth(float Value);
	UFUNCTION()
	void ShowDefeatUI();


	// 스테미나 바 ======================================================================
	// Widget
	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* StaminaWidgetComp;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* DamageBar;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float Stamina;
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float MaxStamina = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float StaminaRecoveryRate = 8.f;
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float StaminaDrainRateCtrl = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float StaminaDrainRateShift = 2.f;

	bool bUsedStaminaCtrl;
	bool bUsedStaminaShift;
	bool bStaminaBarOn;
	bool bShowStaminaValue;

	FTimerHandle StaminaRecoveryTimerHandle;
	FTimerHandle StaminaDrainTimerHandle;
	//FTimerHandle HideStaminaBarHandle;

	UFUNCTION()
	//float GetHealthPercent(float health, float maxHealth);
	void SetStaminaPercent(float InStamina, float InMaxStamina); 
	UFUNCTION()
	void StartStaminaRecovery();
	UFUNCTION()
	void StopStaminaRecovery();
	UFUNCTION()
	void RecoverStamina(float Value);
	UFUNCTION()
	void DrainStamina(float Value);
	UFUNCTION()
	void StartStaminaDrainTimer();
	UFUNCTION()
	void StopStaminaDrainTimer();
	UFUNCTION()
	void ShowDamageBar(float InStamina, float InMaxStamina);
	UFUNCTION()
	void HideDamageBar();
	


	

	 
	// 카운트다운 타이머 =================================================================
	/*UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> TimerUIClass;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MinuteText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SecondText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ZeroText;

	FTimerHandle CountDownTimerHandle;
	float SecondNum = 11.f;
	int32 MinuteNum = 4;
	bool bTimeOver;

	void CountDown();*/


	// 팰 슬롯 ==========================================================================
	// 소환할 팰의 클래스
    //UPROPERTY(EditAnywhere, Category = "PalSlot")
    //UPROPERTY()
    class UPalInfo* PalInfo;

    // 팰 슬롯 UI
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UUserWidget> PalSlotUIClass;

	// 실제 생성된 팰 슬롯 UI 인스턴스
	class UPalSlotUI* PalSlotUI;

	UPROPERTY(EditDefaultsOnly, Category = "PalSlot")
	class UInputAction* IA_PalSlotMoveRigth;
	UPROPERTY(EditDefaultsOnly, Category = "PalSlot")
	class UInputAction* IA_PalSlotMoveLeft;
	UPROPERTY(EditDefaultsOnly, Category = "PalSlot")
	class UInputAction* IA_PalSlotSelect;

	UFUNCTION()
	void PalSlotMoveRigth();
	UFUNCTION()
	void PalSlotMoveLeft();


	//팰 소환 로직 ==========================================================================
	UPROPERTY(EditDefaultsOnly, Category = "PalSlot")
	TSubclassOf<APalSphereActor> PalSphereClass;
	bool bCanInteractPal;
	bool bSummonPal;
	
	void StartCanInteractTimer();
	UFUNCTION()
	void CanInteractElapsed();
	//Delay for declaring summoned or retrieved done
	UPROPERTY(EditDefaultsOnly, Category = "PalSlot")
	float CanInteractDelay;
	UFUNCTION()
	void SpawnSelectPal(const struct FInputActionValue& Value);
	UFUNCTION()
	void ThrowSelectPal();
	UFUNCTION()
	void ThrowPal();
	
	UPROPERTY(EditDefaultsOnly, Category="PalSlot")
	TSubclassOf<UPalInfo> PalInfoClass;

	UPROPERTY(EditDefaultsOnly, Category = "PalSlot")
	TObjectPtr<USoundBase> PalRetrieveSound;
};
