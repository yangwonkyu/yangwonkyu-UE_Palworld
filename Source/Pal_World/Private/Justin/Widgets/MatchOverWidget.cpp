// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Widgets/MatchOverWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"

void UMatchOverWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Button_Return)
	{
		Button_Return->OnPressed.AddDynamic(this, &UMatchOverWidget::ButtonPressed);

		Button_Return->OnHovered.AddDynamic(this, &UMatchOverWidget::ButtonHovered);

		Button_Return->SetIsEnabled(false);
	}

	if (FadeIn)
	{
		FWidgetAnimationDynamicEvent Event;
		Event.BindUFunction(this, "OnFadeInFinished");
		BindToAnimationFinished(FadeIn, Event);
		PlayAnimationForward(FadeIn);
	}
}

void UMatchOverWidget::ButtonPressed()
{
	if (ButtonPressedSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ButtonPressedSound);
	}

	if (Button_Return)
	{
		Button_Return->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		Button_Return->OnHovered.RemoveAll(this);
		Button_Return->OnPressed.RemoveAll(this);
		
		Pressed.ExecuteIfBound();
	}

}

void UMatchOverWidget::ButtonHovered()
{
	if (ButtonHoveredSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ButtonHoveredSound);
	}
}

void UMatchOverWidget::OnFadeInFinished()
{
	if (bRemoveAtEndOfAnimation)
	{
		PlayAnimationForward(FadeOut);
	}
	else {
		Button_Return->SetIsEnabled(true);
	}
}

void UMatchOverWidget::OnFadeOutFinished()
{
	RemoveFromParent();
}