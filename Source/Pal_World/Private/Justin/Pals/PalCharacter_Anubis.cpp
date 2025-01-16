// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Pals/PalCharacter_Anubis.h"
#include "Justin/Skills/AnubisSkill_SideStep.h"
#include "Justin/Components/PalStatusComp.h"
#include "Justin/PFSMComponent.h"
#include "Components/ArrowComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstance.h"

APalCharacter_Anubis::APalCharacter_Anubis()
{
	ArrowComp_SkillEffect = CreateDefaultSubobject<UArrowComponent>("ArrowComp_SkillEffect");
	ArrowComp_SkillEffect->SetupAttachment(RootComponent);

	NiagaraComp_SkillEffect = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComp_SkillEffect");
	NiagaraComp_SkillEffect->SetupAttachment(ArrowComp_SkillEffect);

	NiagaraComp_DashEffect = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComp_DashEffect");
	NiagaraComp_DashEffect->SetupAttachment(GetMesh());
	NiagaraComp_DashEffect->SetAutoActivate(false);

	EffectScale = 2.f;
}

void APalCharacter_Anubis::BeginPlay()
{
	Super::BeginPlay();

	if (AnubisSkillClass)
	{
		AnubisSkill = NewObject<UAnubisSkill_SideStep>(this, AnubisSkillClass);

		if (AnubisSkill)
		{
			AnubisSkill->StartInit(this);
			AnubisSkill->OnSideStepFinished.BindUObject(this, &APalCharacter_Anubis::OnSideStepCompleted);
		}
	}	
}

void APalCharacter_Anubis::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (NiagaraComp_SkillEffect && NiagaraComp_SkillEffect->GetSystemInstance())
	{
		if (NiagaraComp_SkillEffect && NiagaraComp_SkillEffect->GetSystemInstance()->IsComplete())
		{
			NiagaraComp_SkillEffect->SetAsset(nullptr);
		}
	}
}

void APalCharacter_Anubis::OnTakenDamage(float Health, float MaxHealth)
{
	if (StatusComponent->IsDead()) return;

	if (AnubisSkill)
	{
		if (FSMComponent && FSMComponent->GetCurrentState() == EPalState::MOVEMENT ||
			(FSMComponent->GetCurrentState() == EPalState::SETUPATTACK && !FSMComponent->GetStepBack()))
		{
			if (AnubisSkill->StartSideStep(GetTarget()))
			{
				StatusComponent->SetInvincibility(true);
				NiagaraComp_DashEffect->SetActive(true, true);
			}
			else Super::OnTakenDamage(Health, MaxHealth);
		}
	}
}

void APalCharacter_Anubis::OnSideStepCompleted()
{
	StatusComponent->SetInvincibility(false);
	if (FSMComponent)
	{
		FSMComponent->UpdateAction(EPalState::MOVEMENT);
		NiagaraComp_DashEffect->SetActive(false, false);
	}
}

void APalCharacter_Anubis::SpawnForcefulChargeEffect()
{
	SpawnEffect();
	//GetWorld()->GetTimerManager().SetTimer(EffectHandle, this, &APalCharacter_Anubis::SpawnEffect, .4f, true);
}

void APalCharacter_Anubis::DeactivateForcefulChargeEffect()
{
	//NiagaraComp_SkillEffect->Deactivate();

	NiagaraComp_SkillEffect->GetSystemInstance()->Deactivate();
	//NiagaraComp_SkillEffect->SetAsset(nullptr);
	/*if (EffectHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(EffectHandle);
	}*/
}

void APalCharacter_Anubis::SpawnEffect()
{
	if (NS_ForcefulCharge)
	{
		NiagaraComp_SkillEffect->SetAsset(NS_ForcefulCharge);
		NiagaraComp_SkillEffect->SetWorldScale3D(FVector(EffectScale));
	}
}