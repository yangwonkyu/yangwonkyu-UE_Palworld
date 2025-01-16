// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "GrizzBolt.generated.h"

class UGrizzBoltActorComponent;

DECLARE_DELEGATE(FOnCountDownEndSignature)

UCLASS()
class PAL_WORLD_API AGrizzBolt : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGrizzBolt();
	FOnCountDownEndSignature OnCountDownEnd;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnGrizzBoltTakeDamage(float Damage = 100.f);

	// 몸체 스켈레톤
	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* SkeletalMeshComp;

	// 에로우도 생성해야지 갯수가 많으니까 배열로 관리하기
    UPROPERTY(EditAnywhere)
    TArray<class UArrowComponent*> ArrowComponents;

	// 액터 붙여넣기
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UGrizzBoltActorComponent* FSMComp;

	//Radial Force 넣기
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class URadialForceComponent* RadialComp;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComp;

	//----------------------------------------------------------------------------
	// 체력 관련
	UPROPERTY(EditAnywhere)
	class UPStatusComponent* StatusComponent;
	UFUNCTION()
	void OnDead();

	// 카운트다운 타이머 =================================================================
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> TimerUIClass;
	UUserWidget* TimerWidget;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MinuteText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SecondText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ZeroText;
	UPROPERTY(EditAnywhere, Category = "UI")
	float Second;
	UPROPERTY(EditAnywhere, Category = "UI")
	float Minute;
	FTimerHandle CountDownTimerHandle;
	float SecondNum = 59.f;
	int32 MinuteNum = 0;
	bool bTimeOver;
	void CountDown();

	// UI 관련
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> HPUIClass;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HP;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MaxHP;

	UPROPERTY(EditDefaultsOnly)
	float HealthPoint;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealthPoint = 4000.0f;

	UPROPERTY(EditDefaultsOnly)
	float Radius = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Minigun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AZoe> Zoe;

	UFUNCTION()
	void SetHealthPercent(float IHP, float IMaxHP);

	UFUNCTION(BlueprintImplementableEvent)
    void SpawnGAP();

	// findtarget 느낌표 뜨게 하기 시작
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UWidgetComponent* WidgetComponent;
};


