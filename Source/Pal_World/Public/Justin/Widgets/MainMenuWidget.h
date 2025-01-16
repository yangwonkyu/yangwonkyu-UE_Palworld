// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
class USoundBase;
class UFadeWidget;
class UButton;

UCLASS()
class PAL_WORLD_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Start;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Quit;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float FadeOutRate;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float FadeInRate;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TSubclassOf<UFadeWidget> FadeWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category ="Settings")
	TObjectPtr<USoundBase> StartButtonSound;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<USoundBase> EndButtonSound;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TObjectPtr<USoundBase> HoverButtonSound;

	UPROPERTY(BlueprintReadWrite)
	bool bFading;


private:
	UFUNCTION()
	void OnFadeFinished(bool bFadeIn);

	UFUNCTION()
	void Hovered();

	UFUNCTION()
	void StartPressed();

	UFUNCTION()
	void QuitPressed();

	UFUNCTION()
	void QuitHovered();

};
