// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FadeWidget.generated.h"

/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFadeFinishedSignature, bool, bFadeIn);

class UWidgetAnimation;

UCLASS()
class PAL_WORLD_API UFadeWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(bool _bQuitGame, bool _bFadeIn, FName _NextLevel, float _FadeRate = 1.0f);

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnFadeFinishedSignature OnFadeFinished;
protected:

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	bool bQuitGame;
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	bool bFadeIn;
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FName NextLevel;
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	float FadeRate;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnAnimFinished();

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeAnim;
};
