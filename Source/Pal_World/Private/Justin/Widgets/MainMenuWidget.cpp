// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Widgets/MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Justin/Widgets/FadeWidget.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Start->OnHovered.AddDynamic(this, &UMainMenuWidget::Hovered);
	Button_Start->OnPressed.AddDynamic(this, &UMainMenuWidget::StartPressed);
	Button_Quit->OnPressed.AddDynamic(this, &UMainMenuWidget::QuitPressed);
	Button_Quit->OnHovered.AddDynamic(this, &UMainMenuWidget::QuitHovered);

	auto Widget = CreateWidget<UUserWidget>(GetWorld(), FadeWidgetClass);
	auto FadeWidget = Cast<UFadeWidget>(Widget);
	if (FadeWidget)
	{
		FadeWidget->Init(false, true, "", FadeInRate);
		FadeWidget->OnFadeFinished.AddDynamic(this, &UMainMenuWidget::OnFadeFinished);
		bFading = true;
		FadeWidget->AddToViewport();
	}
}

void UMainMenuWidget::OnFadeFinished(bool bFadeIn)
{
	bFading = false;
}

void UMainMenuWidget::Hovered()
{
	if (bFading) return;

	if (HoverButtonSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), HoverButtonSound);
	}
}

void UMainMenuWidget::StartPressed()
{
	if (bFading) return;

	if (StartButtonSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), StartButtonSound);
	}

	Button_Start->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Button_Quit->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	Button_Start->OnHovered.RemoveAll(this);
	Button_Start->OnPressed.RemoveAll(this);
	Button_Quit->OnPressed.RemoveAll(this);
	Button_Quit->OnHovered.RemoveAll(this);
}

void UMainMenuWidget::QuitPressed()
{
	if (bFading) return;

	if (EndButtonSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EndButtonSound);
		auto Widget = CreateWidget<UUserWidget>(GetWorld(), FadeWidgetClass);
		auto FadeWidget = Cast<UFadeWidget>(Widget);
		if (FadeWidget)
		{
			FadeWidget->Init(true, false, "", FadeOutRate);
			FadeWidget->OnFadeFinished.AddDynamic(this, &UMainMenuWidget::OnFadeFinished);
			FadeWidget->AddToViewport();
		}
	}

	Button_Start->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Button_Quit->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	Button_Start->OnHovered.RemoveAll(this);
	Button_Start->OnPressed.RemoveAll(this);
	Button_Quit->OnPressed.RemoveAll(this);
	Button_Quit->OnHovered.RemoveAll(this);

}

void UMainMenuWidget::QuitHovered()
{
	if (bFading) return;
	
	if (HoverButtonSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), HoverButtonSound);
	}
}