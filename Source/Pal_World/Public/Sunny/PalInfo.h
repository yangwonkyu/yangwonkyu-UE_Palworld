// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PalInfo.generated.h"

class APPalCharacter;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UPalSlotUI;
class UParticleSystem;

USTRUCT(BlueprintType)
struct FPalData
{
	GENERATED_BODY()

	FPalData() = default;

	FPalData(FString _Name, TSubclassOf<AActor> _PalClass, UTexture2D* _SlotImage, UTexture2D* _PalInfoImage);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	TSubclassOf<AActor> PalClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	class UTexture2D* SlotImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	class UTexture2D* PalInfoImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	TArray<UMaterialInterface*> PalMaterials;

	APPalCharacter* Pal = nullptr;
};

UCLASS(Blueprintable)
class PAL_WORLD_API UPalInfo : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
public:
	UPalInfo();
	virtual bool IsAllowedToTick() const;
	virtual void Tick(float DeltaTime) override;
	/** return the stat id to use for this tickable **/
	virtual TStatId GetStatId() const override;

	virtual UWorld* GetWorld() const override;

	// 몬스터 정보 초기화
	void InitializePalInfos(ACharacter* Owner);
	void InitializePalSlotUI(UPalSlotUI* _SlotUI);
	bool IsPalSummoned() const;
	bool IsRetrievingPal() const;
	void DeclareDead(APPalCharacter* DeadPal);

	//Testing Function
	APPalCharacter* GetSummonedPal() const;

	UFUNCTION()
	void StartRetrieve();
	void SummonPal(AActor* Initiator, AActor* Target, FVector SpawnLocation, int32 Index);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	TArray<FPalData> PalInfos;

	UFUNCTION(BlueprintCallable)
	void KillSwitch();


	class APCharacter* Player;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Summon")
	TObjectPtr<UParticleSystem> SpawnParticleEffects_Light;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Summon")
	TObjectPtr<UParticleSystem> SpawnParticleEffects_Sphere;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Summon")
	TObjectPtr< UMaterialInterface > SummonMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Summon")
	float SummonDuration;
	//Loss of emissiveness per second
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Summon")
	float SummonEmissiveRate;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Retrieve")
	TObjectPtr< UMaterialInterface > RetrieveMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Retrieve")
	float RetrievalDuration;
	//Emissiveness gain per second
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Summon")
	float RetrieveEmissiveRate;

private:
	TObjectPtr<UMaterialInstanceDynamic> SummonDynamicMaterial;
	TObjectPtr<UMaterialInstanceDynamic> RetrieveDynamicMaterial;
	float DefaultSummonEmissiveVal;
	float DefaultRetrieveEmissiveVal;

	TObjectPtr<UPalSlotUI> SlotUI;
	APPalCharacter* PalSummoned;
	int32 SummonedIndex;
	
	FTimerHandle DeadRetrieveHandle;
	
	FTimerHandle StartSummonHandle;
	void FinishSummon();
	FTimerHandle RetrievingHandle;
	void RetrievePal();
};
