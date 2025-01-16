// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/PalSlotUI.h"
#include "Sunny/PalInfo.h"
#include "Justin/PPalCharacter.h"
#include "Justin/Components/PStatusComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPalSlotUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UPalSlotUI::InitializeSlots(UPalInfo* PalInfo)
{
	PalInfoSlotUI = PalInfo;
	// UE_LOG(LogTemp, Warning, TEXT("PalSlotUI::InitializeSlots()"));
	if (PalInfoSlotUI && PalInfoSlotUI->PalInfos.Num() > 0)
	{
		CurrentPalIndex = 0;
		//UE_LOG(LogTemp, Warning, TEXT("InitializeSlots()::CurrentPalIndex = 0"));
		UpdateSlots();
		PalInfoSlotUI->InitializePalSlotUI(this);
	}
}

void UPalSlotUI::RemoveHealthBind()
{
	FPalData Data = GetSelectedPalData();
	if (Data.Pal)
	{
		auto Status = Data.Pal->GetComponentByClass<UPStatusComponent>();
		if (Status && Status->OnHealthChanged.IsBoundToObject(this))
			Status->OnHealthChanged.RemoveAll(this);
	}
}

void UPalSlotUI::OnPalHealthChanged(float Health, float MaxHealth)
{
	ProgressBar_Health->SetPercent(Health / MaxHealth);
}

void UPalSlotUI::UpdateSlots()
{
	//UE_LOG(LogTemp, Warning, TEXT("PalSlotUI::UpdateSlots()"));
	if (!PalInfoSlotUI || PalInfoSlotUI->PalInfos.Num() == 0)
	{
		return;
	}

	int32 PalCount = PalInfoSlotUI->PalInfos.Num();
	UTexture2D* Slot1Texture = PalInfoSlotUI->PalInfos[(CurrentPalIndex) % PalCount].SlotImage;
	UTexture2D* Slot2Texture = PalInfoSlotUI->PalInfos[(CurrentPalIndex + 1) % PalCount].SlotImage;
	UTexture2D* Slot3Texture = PalInfoSlotUI->PalInfos[(CurrentPalIndex + 2) % PalCount].SlotImage;

	if (Slot1) Slot1->SetBrushFromTexture(Slot1Texture);
	if (Slot2) Slot2->SetBrushFromTexture(Slot2Texture);
	if (Slot3) Slot3->SetBrushFromTexture(Slot3Texture);

	if (Image_PalUI)
	{
		Image_PalUI->SetBrushFromTexture(nullptr);
		Image_PalUI->SetBrushFromTexture(PalInfoSlotUI->PalInfos[(CurrentPalIndex) % PalCount].PalInfoImage);
	}

	FPalData Data = GetSelectedPalData();
	Text_PalName->SetText(FText::FromString(Data.Name));
	if (Data.Pal)
	{
		auto Status = Data.Pal->GetComponentByClass<UPStatusComponent>();
		if (Status)
		{
			ProgressBar_Health->SetPercent(Status->GetHealth() / Status->GetMaxHealth());

			if (ensureAlways(!Status->OnHealthChanged.IsBoundToObject(this)))
				Status->OnHealthChanged.AddUObject(this, &UPalSlotUI::OnPalHealthChanged);
		}
	}
}

void UPalSlotUI::MoveRight()
{
	RemoveHealthBind();

	CurrentPalIndex = (CurrentPalIndex + 1) % PalInfoSlotUI->PalInfos.Num();
	UpdateSlots();
}

void UPalSlotUI::MoveLeft()
{
	RemoveHealthBind();

	CurrentPalIndex = (CurrentPalIndex - 1 + PalInfoSlotUI->PalInfos.Num()) % PalInfoSlotUI->PalInfos.Num();
	UpdateSlots();
}

FPalData UPalSlotUI::GetSelectedPalData() const
{
	int32 SelectedIndex = GetCurrentIndex();
	return PalInfoSlotUI->PalInfos[SelectedIndex];
}

void UPalSlotUI::SpawnSelectedPal(FVector SpawnLocation, AActor* Target)
{
	//UE_LOG(LogTemp, Warning, TEXT("PalSlotUI::SpawnSelectedPal()  -=========================="));
	FPalData SelectedPalData = GetSelectedPalData();
	if (SelectedPalData.Pal)
	{
		PalInfoSlotUI->SummonPal(nullptr, Target, SpawnLocation, GetCurrentIndex());
	}

	/*if (SelectedPalData.PalClass)
	{
		FActorSpawnParameters SpawnParams;
		UWorld* World = GetWorld();
		if (World)
		{
			FVector Location = World->GetFirstPlayerController()->GetPawn()->GetActorLocation() + FVector(200.0f, 0.0f, 0.0f);
			World->SpawnActor<AActor>(SelectedPalData.PalClass, Location, FRotator::ZeroRotator, SpawnParams);
		}
	}*/
}

void UPalSlotUI::RetrievePal()
{
	PalInfoSlotUI->StartRetrieve();
}

bool UPalSlotUI::IsCurrentPalDead() const
{
	FPalData Pal = GetSelectedPalData();
	if (Pal.Pal)
	{
		auto Status = Pal.Pal->GetComponentByClass<UPStatusComponent>();
		if (Status) return Status->IsDead();
	}
	return false;
}

int32 UPalSlotUI::GetCurrentIndex() const
{
	return (CurrentPalIndex + 1) % PalInfoSlotUI->PalInfos.Num();
}
