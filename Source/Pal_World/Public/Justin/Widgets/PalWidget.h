// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalWidget.generated.h"

/**
 * 
 */

class UTexture2D;
class UImage;

UCLASS()
class PAL_WORLD_API UPalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	void StartInitialize(AActor* Owner, UTexture2D* Image);
	void Display(bool bDisplay);
protected:	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UImage> Image_PalUI;

	UPROPERTY(BlueprintReadOnly)
	FVector2D Offset;
	UPROPERTY(BlueprintReadOnly)
	USceneComponent* AttachTo;
};
