// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Widgets/PalWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"

void UPalWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Display(false);
}

void UPalWidget::StartInitialize(AActor* Owner, UTexture2D* Image)
{
	if (Owner)
	{
		auto WidgetLocationComp = Owner->FindComponentByTag<USceneComponent>("WidgetLocation");
		AttachTo = WidgetLocationComp;

		if (Image_PalUI)
		{
			Image_PalUI->SetBrushFromTexture(Image);
		}
	}
}

void UPalWidget::Display(bool bDisplay)
{
	if (bDisplay)
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}
