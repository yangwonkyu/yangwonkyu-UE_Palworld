// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Components/PalStatusComp.h"
#include "Justin/Widgets/PalWidget.h"
#include "Justin/PPalCharacter.h"
#include "Sunny/PalInfo.h"

UPalStatusComp::UPalStatusComp()
{
	bInvincible = false;
}

void UPalStatusComp::BeginPlay()
{
	Super::BeginPlay();


	if (HealthWidget)
	{
		HealthWidget->RemoveFromParent();
		auto Widget = Cast<UPalWidget>(HealthWidget);
		if (Widget)
		{
			Widget->StartInitialize(GetOwner(), PalUIImage);
			HealthWidget = Widget;
			Widget->AddToViewport(-5);		
		}
	}
	Pal = GetOwner<APPalCharacter>();

}

void UPalStatusComp::DisplayWidget(bool bDisplay)
{
	auto Widget = Cast<UPalWidget>(HealthWidget);
	if (Widget)
	{
		Widget->Display(bDisplay);
	}
}

void UPalStatusComp::TakeDamage(AActor* Initiator, FVector ImpactLocation, float Damage)
{
	if (Pal)
	{
		const auto palInfo = Pal->GetPalInfo();

		if(palInfo)

		if (palInfo->IsRetrievingPal()) return;
	}
	
	if (bInvincible) return;

	Super::TakeDamage(Initiator, ImpactLocation, Damage);
}

void UPalStatusComp::SetInvincibility(bool bEnable)
{
	bInvincible = bEnable;
}
