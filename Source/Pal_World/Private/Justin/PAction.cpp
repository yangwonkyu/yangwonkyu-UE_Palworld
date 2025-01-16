// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/PAction.h"
#include "Justin/PFSMComponent.h"
#include "Justin/PPalCharacter.h"
#include "Justin/PPalSkillComponent.h"

UPAction::UPAction()
{

}

void UPAction::Initialize(APPalCharacter* _PalCharacter)
{
	if (_PalCharacter)
	{
		PalCharacter = _PalCharacter;
		
		auto _FSMComp = PalCharacter->GetComponentByClass<UPFSMComponent>();
		if (_FSMComp) FSMComp = _FSMComp;
		
		auto _SkillComp = PalCharacter->GetComponentByClass<UPPalSkillComponent>();
		if (_SkillComp) SkillComp = _SkillComp;
	}
}

void UPAction::Start()
{
	if (FSMComp)
	{
		FSMComp->SetCurrentState(GetPalState());
		UE_LOG(LogTemp, Warning, TEXT("Start : %s"), *UEnum::GetValueAsString(GetPalState()));
	}
}

EPalState UPAction::Update(float DeltaTime)
{
	return EPalState::NONE;
}

void UPAction::Stop()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop : %s"), *UEnum::GetValueAsString(GetPalState()));
}

void UPAction::UpdateAction()
{
	//
}

UWorld* UPAction::GetWorld() const
{
	auto Actor = Cast<AActor>(GetOuter());
	if (Actor)
	{
		return Actor->GetWorld();
	}
	else return nullptr;
}

EPalState UPAction::GetPalState() const
{
	return PalState;
}
