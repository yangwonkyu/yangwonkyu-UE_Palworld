// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Justin/PFSMComponent.h"
#include "Justin/PPalSkillComponent.h"
#include "PAction.generated.h"

/**
 * 
 */

class APPalCharacter;
class UPPalSkillComponent;
enum class EPalState : uint8;

UCLASS(Blueprintable)
class PAL_WORLD_API UPAction : public UObject
{
	GENERATED_BODY()
	
public: 
	UPAction();
	void Initialize(APPalCharacter* _PalCharacter);
	virtual void Start();
	virtual EPalState Update(float DeltaTime);
	virtual void Stop();

	virtual void UpdateAction();

protected:
	virtual UWorld* GetWorld() const override;
	TObjectPtr<UPFSMComponent> FSMComp;
	TObjectPtr<UPPalSkillComponent> SkillComp;
	TObjectPtr<APPalCharacter> PalCharacter;
	EPalState GetPalState() const; 
private:
	UPROPERTY(EditDefaultsOnly, Category="Settings", meta=(AllowPrivateAccess))
	EPalState PalState;
};
