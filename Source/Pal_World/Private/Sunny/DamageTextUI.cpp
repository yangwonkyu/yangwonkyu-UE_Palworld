// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/DamageTextUI.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

void UDamageTextUI::ShowDamageTextUI(const FVector2D& ScreenLocation, float Damage)
{
	// �ؽ�Ʈ ��ġ�� �����ϰ� ȭ�鿡 �߰�
	SetPositionInViewport(ScreenLocation);
	AddToViewport();

	// �ؽ�Ʈ ����
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

	// FadeOut �ִϸ��̼� ���
	if (FadeOutAnim)
	{
		// �ִϸ��̼� �Ϸ� �� RemoveFromParent ȣ��
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
		// �ִϸ��̼��� ������ �θ𿡼� ����
		RemoveFromParent();
	}
}
