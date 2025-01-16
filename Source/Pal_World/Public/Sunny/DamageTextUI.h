// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageTextUI.generated.h"

/**
 * 
 */
UCLASS()
class PAL_WORLD_API UDamageTextUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* FadeOutAnim;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DamageText;

public:
	void ShowDamageTextUI(const FVector2D& ScreenLocation, float Damage);
	void HideDamageTextUI();

	void PlayFadeOutAnimation();
	void OnFadeOutFinished();
	
};
