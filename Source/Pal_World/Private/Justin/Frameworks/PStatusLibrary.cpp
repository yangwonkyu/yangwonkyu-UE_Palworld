// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Frameworks/PStatusLibrary.h"
#include "GameFramework/Character.h"
#include "Justin/Components/PStatusComponent.h"

void UPStatusLibrary::ApplyDamage(AActor* Initiator, AActor* Target, FVector ImpactLocation, float Damage)
{
	if (Target)
	{
		auto Status = Target->GetComponentByClass<UPStatusComponent>();
		if (Status)
		{
			Status->TakeDamage(Initiator, ImpactLocation, Damage);
		}
	}


}
