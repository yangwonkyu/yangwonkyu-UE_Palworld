// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PStatusComponent.generated.h"


DECLARE_MULTICAST_DELEGATE(FDiedDDelegate);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float /* Hp */, float /* MaxHp */);
class UUserWidget;

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAL_WORLD_API UPStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPStatusComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//float Health, float MaxHealth
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> HitCameraShakeClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> LowHealthUIClass;
	UPROPERTY()
	class UUserWidget* LowHealthWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> DamageTextUIClass;
	UPROPERTY()
	class UDamageTextUI* DamageTextUI;
	
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	class USoundBase* DamageSFX;



	bool bIsLowHealthUIVisible;

	void ShowLowHealthUI();
	void HideWidget();

	void ShowDamageTextUI(AActor* Initiator, FVector ImpactLocation, float Damage);
	void HideDamageTextUI();

	UFUNCTION(BlueprintCallable)
	virtual void TakeDamage(AActor* Initiator, FVector ImpactLocation, float Damage);

	float GetHealth() const;
	float GetMaxHealth() const;

	bool IsDead() const;
	bool IsInitialized() const;

	FDiedDDelegate OnDied;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TSubclassOf<UUserWidget> HealthWidgetClass;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ProgressBar_Health;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* DamageBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_CurrentHealth;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_MaxHealth;

protected:
	void SetHealthPercent(float InHealth, float InMaxHealth);

	UFUNCTION()
	void ShowDamageBar(float InHealth, float InMaxHealth);
	UFUNCTION()
	void HideDamageBar();
	bool bIsDamageBarVisible;

	TObjectPtr<UUserWidget> HealthWidget;
private:
	float Hp;
	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (AllowPrivateAccess))
	float MaxHp;
	bool Initialized;
};
