// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PalWorld_GameMode.generated.h"

/**
 * 
 */

class UMatchOverWidget;
class APCharacter;
class AGrizzBolt;
class UFadeWidget;

UENUM(Blueprintable)
enum class EBGM : uint8
{
	MAINMENU UMETA(DisplayName = "MainMenu"),
	LOADED UMETA(DisplayName = "Loaded"),
	BOSSFIGHT UMETA(DisplayName = "BossFight"),
	DEFEATED UMETA(DisplayName = "Defeated"),
	BOSSELIMINATED UMETA(DisplayName = "BossEliminated"),	
	THUNDER UMETA(DisplayName = "Thunder")
};

UENUM()
enum class EMatchOverState
{
	DEFEATED UMETA(DisplayName = "Defeated"),
	BOSSELIMINATED UMETA(DisplayName = "BossEliminated"),
	TIMEDOUT UMETA(DisplayName = "TimedOut")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReturnPressedSignature);

UCLASS()
class PAL_WORLD_API APalWorld_GameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	APalWorld_GameMode();

	bool IsMatchOver() const;
	UPROPERTY(BlueprintAssignable)
	FOnReturnPressedSignature OnReturnPressed;

	UFUNCTION(BlueprintCallable)
	void RemoveWidgets();

protected:
	UFUNCTION(BlueprintCallable)
	USoundBase* GetSound(EBGM BGMSound) const;

	UFUNCTION(BlueprintCallable)
	void SetupPlayerAndBoss();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Sound")
	TMap<EBGM, USoundBase*> BGMSounds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Sound")
	TObjectPtr<UAudioComponent> AudioComp;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Actors")
	TSubclassOf<APCharacter> PlayerActorClass;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Actors")
	TSubclassOf<AGrizzBolt> BossActorClass;

	UPROPERTY(EditAnywhere, Category = "Settings|MatchOver")
	TSubclassOf<UMatchOverWidget> DefeatedWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Settings|MatchOver")
	TSubclassOf<UMatchOverWidget> BossEliminatedWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Settings|MatchOver")
	TSubclassOf<UFadeWidget> FadeWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Settings|MatchOver")
	float DefeatedWidgetDelay;
	UPROPERTY(EditAnywhere, Category = "Settings|MatchOver")
	float BossEliminatedWidgetDelay;

private:
	void SetupPlayer();
	void SetupBoss();

	bool bMatchOver;

	UFUNCTION()
	void OnPlayerDead();
	UFUNCTION()
	void OnCountDownEnd();
	UFUNCTION()
	void OnDefeated();
	UFUNCTION()
	void OnBossDead();
	UFUNCTION()
	void OnBossEliminated();

	UPROPERTY()
	TObjectPtr<UMatchOverWidget> DefeatedWidget;
	UPROPERTY()
	TObjectPtr<UMatchOverWidget> BossEliminatedWidget;

	void ButtonPressed();

	TObjectPtr<APCharacter> Player;
	TObjectPtr<AGrizzBolt> Boss;
};
