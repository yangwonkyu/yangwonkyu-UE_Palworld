// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Sunny/PalInfo.h"
#include "PalSlotUI.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class PAL_WORLD_API UPalSlotUI : public UUserWidget
{
	GENERATED_BODY()

public:
	// 팰 슬롯 이미지 배열
	//UPROPERTY(meta = (BindWidget))
	//TArray<UImage*> PalSlots;

	UPROPERTY(EditAnywhere)
	class UPalInfo* PalInfoSlotUI = nullptr;

	UPROPERTY(meta = (BindWidget))
	UImage* Slot1;

	UPROPERTY(meta = (BindWidget))
	UImage* Slot2;

	UPROPERTY(meta = (BindWidget))
	UImage* Slot3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_PalName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_PalUI;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr < UProgressBar> ProgressBar_Health;

	bool IsCurrentPalDead() const;

	// 총 팰 배열
	TArray<UTexture2D*> PalTextures;

	// 초기화 함수
	void InitializeSlots(UPalInfo* PalInfo);

	// 슬롯 갱신 함수
	void UpdateSlots();

	// 팰 선택 함수 (1번 키)
	void MoveRight();

	// 몬스터 선택 함수 (3번 키)
	void MoveLeft();

	// 중앙 슬롯의  팰 번호 반환하는 함수
	FPalData GetSelectedPalData() const;

	// 선택된 팰 소환
	void SpawnSelectedPal(FVector SpawnLocation, AActor* Target = nullptr);
	void RetrievePal();
	
	// 현재 선택된 팰 인덱스
	int32 CurrentPalIndex;
	int32 GetCurrentIndex() const;

protected:
	virtual void NativeOnInitialized();

	void RemoveHealthBind();
	void OnPalHealthChanged(float Health, float MaxHealth);
};
