// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GrizzBoltHealthWidget.generated.h"

/**
 * 
 */

class UWidgetAnimation;

UCLASS()
class PAL_WORLD_API UGrizzBoltHealthWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnim;

	UFUNCTION()
	void RemoveWidget();
};
