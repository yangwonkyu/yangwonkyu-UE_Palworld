// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchOverWidget.generated.h"

/**
 * 
 */

class UButton;

DECLARE_DELEGATE(FOnButtonPressedSignature);

UCLASS()
class PAL_WORLD_API UMatchOverWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void ButtonPressed();

	UFUNCTION()
	void ButtonHovered();

	FOnButtonPressedSignature Pressed;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	bool bRemoveAtEndOfAnimation;
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TObjectPtr<USoundBase> ButtonHoveredSound;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<USoundBase> ButtonPressedSound;

	UPROPERTY(meta=(BindWidget))
	UButton* Button_Return;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeIn;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOut;

private:
	UFUNCTION()
	void OnFadeInFinished();
	UFUNCTION()
	void OnFadeOutFinished();
};
