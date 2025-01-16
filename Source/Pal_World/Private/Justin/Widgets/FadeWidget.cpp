// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Widgets/FadeWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Animation/WidgetAnimationEvents.h"
#include "Kismet/GameplayStatics.h"

void UFadeWidget::Init(bool _bQuitGame, bool _bFadeIn, FName _NextLevel, float _FadeRate)
{
	bQuitGame = _bQuitGame;
	bFadeIn = _bFadeIn;
	NextLevel = _NextLevel; 
	FadeRate = _FadeRate;
}

void UFadeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FWidgetAnimationDynamicEvent Event;
	Event.BindUFunction(this, "OnAnimFinished");
	BindToAnimationFinished(FadeAnim, Event);

	if (bFadeIn)
	{
		PlayAnimationForward(FadeAnim, FadeRate);
	}
	else
	{
		PlayAnimationReverse(FadeAnim, FadeRate);
	}
}

void UFadeWidget::OnAnimFinished()
{
	OnFadeFinished.Broadcast(bFadeIn);

	if (bQuitGame)
		UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, true);
	else if (!bFadeIn)
	{
		if (ensureAlways(!NextLevel.IsNone()))
			UGameplayStatics::OpenLevel(GetWorld(), NextLevel);
		else UE_LOG(LogTemp, Warning, TEXT("Invalid Level name. Can't move to next map"));
	}
	else
	{
		RemoveFromParent();
		Destruct();
	}
}