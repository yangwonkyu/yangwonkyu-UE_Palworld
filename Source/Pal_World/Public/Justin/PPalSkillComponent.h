// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Justin/PalSkillObject.h"
#include "PPalSkillComponent.generated.h"

class UPalSkillObject;

USTRUCT()
struct FAvailableSkill
{
	GENERATED_BODY()

	FAvailableSkill() = default;
	FAvailableSkill(int _Index, UPalSkillObject* _Skill)
		: Index(_Index), Skill(_Skill)
	{}

	int Index;
	UPROPERTY()
	TObjectPtr<UPalSkillObject> Skill;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PAL_WORLD_API UPPalSkillComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPPalSkillComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Enable();
	void Disable();

	bool IsSkillAvailable();
	float GetSkillDistance() const;

	ESkillRange GetSkillRangeType() const;
	float GetStepBackTriggerDistance() const;
	bool MustStepBack() const;
	int32 GetStepBackAmount() const;
	float GetStepBackDistance() const;
	float GetMinStepBackNeeded() const;

	bool IsWithinTarget(AActor* Target) const;
	//bool IsInProperRange(AActor* Target, float Offset) const;

	void ActivateSkill(UObject* Obj, FName FuncName);
	void PrepareAttack();
	void StartAttack();
	
	//Binds to owning Pal Skills
	UFUNCTION()
	void OnSkillDeactivated();
	UFUNCTION()
	void OnAttackExecuted(const UPalSkillObject* ActiveSkill);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TArray<TSubclassOf<UPalSkillObject>> SkillClasses;

private:
	void StartCooldown(const UPalSkillObject* ActiveSkill);
	UPalSkillObject* GetAvailableSkill() const;

	UPROPERTY(VisibleInstanceOnly)
	TArray<UPalSkillObject*> Skills;
	TQueue<FAvailableSkill> AvailableSkill;

	TArray<float> TotalTimes;
	TArray<bool> InCooldown;
};
