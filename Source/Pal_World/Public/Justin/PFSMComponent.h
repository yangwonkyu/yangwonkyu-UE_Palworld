// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PFSMComponent.generated.h"


UENUM(Blueprintable)
enum class EPalState : uint8
{
	NONE UMETA(DisplayName = "None"),
	IDLE UMETA(DisplayName = "Idle"),
	MOVEMENT UMETA(DisplayName = "Movement"),
	SETUPATTACK UMETA(DisplayName = "SetupAttack"),
	ATTACK UMETA(DisplayName = "Attack"),
	STUN  UMETA(DisplayName = "Stun")
};

class UPAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAL_WORLD_API UPFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPFSMComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	void Enable();
	void Disable();

	void ExecuteAction(EPalState NewState);
	void SetCurrentState(EPalState _CurrentState);
	EPalState GetCurrentState() const;
	
	void UpdateAction(EPalState StateToUpdate);

	bool GetStepBack() const;
	void SetStepBack(bool _bIsSteppingBack);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TMap<EPalState, TSubclassOf<UPAction>> MapActionClasses;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	EPalState StartingState;

private:
	UPROPERTY()
	TMap<EPalState, UPAction*> MapActions;
	EPalState CurrentState;
	UPROPERTY()
	TObjectPtr<UPAction> CurrentAction;
	bool bIsAttacking;

	bool bIsSteppingBack;
};
