// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Widgets/GrizzBoltHealthWidget.h"
#include "Animation/WidgetAnimation.h"

void UGrizzBoltHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (FadeOutAnim)
	{
		FWidgetAnimationDynamicEvent Event;
		Event.BindUFunction(this, "RemoveWidget");
		BindToAnimationFinished(FadeOutAnim, Event);

		PlayAnimationForward(FadeOutAnim);
	}
}

void UGrizzBoltHealthWidget::RemoveWidget()
{
	RemoveFromParent();
}
