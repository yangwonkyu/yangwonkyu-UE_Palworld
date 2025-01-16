// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/DamageTextUI.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

void UDamageTextUI::ShowDamageTextUI(const FVector2D& ScreenLocation, float Damage)
{
	// 텍스트 위치를 설정하고 화면에 추가
	SetPositionInViewport(ScreenLocation);
	AddToViewport();

	// 텍스트 설정
	if (DamageText)
	{
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0);
		DamageText->SetText(FText::AsNumber(Damage, &Opts));
		UE_LOG(LogTemp, Warning, TEXT("DamageText is OK"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DamageText is null in ShowDamageTextUI"));
	}
}

void UDamageTextUI::HideDamageTextUI()
{
	UE_LOG(LogTemp, Warning, TEXT("UDamageTextUI::HideDamageTextUI()"));

	// FadeOut 애니메이션 재생
	if (FadeOutAnim)
	{
		// 애니메이션 완료 시 RemoveFromParent 호출
		FWidgetAnimationDynamicEvent AnimationFinishedDelegate;
		AnimationFinishedDelegate.BindUFunction(this, FName("OnFadeOutFinished"));
		BindToAnimationFinished(FadeOutAnim, AnimationFinishedDelegate);

		PlayAnimation(FadeOutAnim);
	}
	else
	{
		RemoveFromParent();
	}
}

void UDamageTextUI::PlayFadeOutAnimation()
{
	if (FadeOutAnim)
	{
		UE_LOG(LogTemp, Warning, TEXT("inin2"));
		PlayAnimation(FadeOutAnim);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NOOOOO FadeOutAnim!!!! ever~~~ "));
	}
	
}

void UDamageTextUI::OnFadeOutFinished()
{
	if (IsValid(this))
	{
		// 애니메이션이 끝나면 부모에서 제거
		RemoveFromParent();
	}
}
