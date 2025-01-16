// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/PalInfo.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "Justin/PPalCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Justin/Components/PalStatusComp.h"
#include "Justin/Frameworks/PStatusLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Sunny/PalSlotUI.h"
#include "Sunny/PCharacter.h"
#include "Particles/ParticleSystemComponent.h"

UPalInfo::UPalInfo()
{
	//// 블루프린트 경로를 사용하여 팰 클래스 로드
	//static ConstructorHelpers::FClassFinder<AActor> AnubisBP(TEXT("/Script/Engine.Blueprint'/Game/Justin/Anubis/BP_Anubis.BP_Anubis_C'"));
	//static ConstructorHelpers::FClassFinder<AActor> KatressBP(TEXT("/Script/Engine.Blueprint'/Game/Justin/Katress/BP_Katress.BP_Katress_C'"));
	////static ConstructorHelpers::FClassFinder<AActor> EgleBP(TEXT("/Script/Engine.Blueprint'/Game/Justin/Anubis/BP_Anubis.BP_Anubis_C'"));
	////static ConstructorHelpers::FClassFinder<AActor> DragonBP(TEXT("/Script/Engine.Blueprint'/Game/Justin/Katress/BP_Katress.BP_Katress_C'"));

	//// 슬롯 이미지 로드
	//UTexture2D* AnubisSlotImg = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Sunny/Sprite/Anubis.Anubis'"));
	//UTexture2D* KatressSlotImg = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Sunny/Sprite/Katress.Katress'"));
	//UTexture2D* EgleSlotImg = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Sunny/Sprite/Anubis.Anubis'"));
	//UTexture2D* DragonSlotImg = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Sunny/Sprite/Katress.Katress'"));


	//// PalInfos 배열에 데이터를 추가
	//if (AnubisBP.Succeeded())
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("PalInfo::AnubisBP.Succeeded()"));
	//	PalInfos.Add({ TEXT("Anubis"), AnubisBP.Class, AnubisSlotImg });
	//}
	//if (KatressBP.Succeeded())
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("PalInfo::KatressBP.Succeeded()"));
	//	PalInfos.Add({ TEXT("Katress"), KatressBP.Class, KatressSlotImg });
	//}
	//if (EgleBP.Succeeded())
	//{
	//    //UE_LOG(LogTemp, Warning, TEXT("PalInfo::EgleBP.Succeeded()"));
	//    PalInfos.Add({ TEXT("Egle"), EgleBP.Class, EgleSlotImg });
	//}
	//if (DragonBP.Succeeded())
	//{
	//    //UE_LOG(LogTemp, Warning, TEXT("PalInfo::DragonBP.Succeeded()"));
	//    PalInfos.Add({ TEXT("Dragon"), DragonBP.Class, DragonSlotImg });
	//}

	RetrievalDuration = 1.f;
	SummonDuration = 1.f;
	SummonEmissiveRate = 50.f;
	RetrieveEmissiveRate = 30.f;

}

bool UPalInfo::IsAllowedToTick() const
{
	return PalSummoned && (RetrievingHandle.IsValid() || StartSummonHandle.IsValid());
}

void UPalInfo::Tick(float DeltaTime)
{
	if (RetrievingHandle.IsValid())
	{
		FHashedMaterialParameterInfo Info("Emissive");
		float EmissiveValue;
		RetrieveDynamicMaterial->GetScalarParameterValue(Info, EmissiveValue);

		UE_LOG(LogTemp, Warning, TEXT("Retrieve Val: %f"), EmissiveValue);

		EmissiveValue = EmissiveValue - RetrieveEmissiveRate * DeltaTime;
		RetrieveDynamicMaterial->SetScalarParameterValue("Emissive", EmissiveValue);
	}
	else if (StartSummonHandle.IsValid())
	{
		FHashedMaterialParameterInfo Info("Emissive");
		float EmissiveValue;
		SummonDynamicMaterial->GetScalarParameterValue(Info, EmissiveValue);

		UE_LOG(LogTemp, Warning, TEXT("Summon Val: %f"), EmissiveValue);

		EmissiveValue = FMath::Max(2, EmissiveValue - SummonEmissiveRate * DeltaTime);
		SummonDynamicMaterial->SetScalarParameterValue("Emissive", EmissiveValue);
	}
}

TStatId UPalInfo::GetStatId() const
{
	return TStatId();
}

UWorld* UPalInfo::GetWorld() const
{
	auto Actor = Cast<AActor>(GetOuter());
	if (Actor)
	{
		return Actor->GetWorld();
	}
	return nullptr;
}

bool UPalInfo::IsPalSummoned() const
{
	return PalSummoned != nullptr;
}

bool UPalInfo::IsRetrievingPal() const
{
	UE_LOG(LogTemp, Warning, TEXT("RetrievingHandle: %s"), RetrievingHandle.IsValid() ? TEXT("Retrieving") : TEXT("NOT RETRIEVING"));
	UE_LOG(LogTemp, Warning, TEXT("DeadRetrieveHandle: %s"), DeadRetrieveHandle.IsValid() ? TEXT("DeadRetrieve") : TEXT("NOT DEAD RETRIEVING"));
	return RetrievingHandle.IsValid();
}

void UPalInfo::InitializePalInfos(ACharacter* Owner)
{
	UE_LOG(LogTemp, Warning, TEXT("PalInfo::InitializePalInfos()"));

	for (int i = 0; i < PalInfos.Num(); ++i)
	{
		if (PalInfos[i].PalClass)
		{
			APPalCharacter* SpawnedPal = GetWorld()->SpawnActorDeferred<APPalCharacter>(
				PalInfos[i].PalClass, FTransform::Identity, Owner);
			if (SpawnedPal)
			{
				SpawnedPal->Initialize(FVector(0.f, 0.f, 10000.f), Owner, this);
				UGameplayStatics::FinishSpawningActor(SpawnedPal, FTransform::Identity);
				PalInfos[i].Pal = SpawnedPal;
			}
		}
	}
}

void UPalInfo::InitializePalSlotUI(UPalSlotUI* _SlotUI)
{
	SlotUI = _SlotUI;
}

void UPalInfo::DeclareDead(APPalCharacter* DeadPal)
{
	if (ensureAlways(DeadPal == PalSummoned))
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "StartRetrieve");
		GetWorld()->GetTimerManager().SetTimer(DeadRetrieveHandle, Delegate, 3.f, false);
	}
}

APPalCharacter* UPalInfo::GetSummonedPal() const
{
	return PalSummoned;
}

void UPalInfo::StartRetrieve()
{
	if (RetrievingHandle.IsValid())
	{
		return;
	}

	if (DeadRetrieveHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DeadRetrieveHandle);
	}

	if (PalSummoned)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartRetrieve called. Add fade out material to pal."));

		if (RetrieveMaterial)
		{
			UMaterialInstanceDynamic* Dynamic = UMaterialInstanceDynamic::Create(RetrieveMaterial, GetOuter());
			if (Dynamic)
			{
				RetrieveDynamicMaterial = Dynamic;
				FHashedMaterialParameterInfo Info("Emission");
				float Val;
				RetrieveDynamicMaterial->GetScalarParameterValue(Info, Val);
				DefaultRetrieveEmissiveVal = Val;

				//RetrieveDynamicMaterial->SetScalarParameterValue("Emissive", DefaultRetrieveEmissiveVal);
			}
		}

		for (int i = 0; i < PalSummoned->GetMesh()->GetMaterials().Num(); ++i)
		{
			if (RetrieveDynamicMaterial)
			{
				PalSummoned->GetMesh()->SetMaterial(i, RetrieveDynamicMaterial);
			}
		}



		UE_LOG(LogTemp, Warning, TEXT("Time before: %f"), PalSummoned->GetActorTimeDilation());

		auto Status = PalSummoned->GetComponentByClass<UPalStatusComp>();
		if (Status->IsDead())
		{
			//PalSummoned->GetMesh()->SetCollisionProfileName("Pawn");
			PalSummoned->GetMesh()->SetSimulatePhysics(false);
			PalSummoned->GetMesh()->SetComponentTickEnabled(false);
		}

		PalSummoned->CustomTimeDilation = 0.f;
		UE_LOG(LogTemp, Warning, TEXT("Time after: %f"), PalSummoned->GetActorTimeDilation());

		GetWorld()->GetTimerManager().SetTimer(RetrievingHandle, this, &UPalInfo::RetrievePal, RetrievalDuration, false);
	}
}

void UPalInfo::RetrievePal()
{
	// Player 초기화
	if (Player == nullptr) // 이미 초기화된 경우에는 다시 초기화하지 않도록 체크
	{
		APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
		if (PlayerController)
		{
			Player = Cast<APCharacter>(PlayerController->GetPawn());
		}
	}

	// Player가 유효한지 확인하고 bSummonPal 상태를 변경
	if (Player)
	{
		Player->bSummonPal = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is nullptr. Cannot set bSummonPal to false."));
	}

	if (PalSummoned)
	{
		GetWorld()->GetTimerManager().ClearTimer(RetrievingHandle);
		PalSummoned->Retrieve(FVector(0.f, 0.f, 10000.f));

		auto Status = PalSummoned->GetComponentByClass<UPalStatusComp>();
		if (Status->IsDead())
		{
			PalSummoned->GetMesh()->SetCollisionProfileName("Pawn");
			PalSummoned->GetMesh()->SetComponentTickEnabled(true);
		}

		PalSummoned = nullptr;
	}
	else UE_LOG(LogTemp, Warning, TEXT("No summoned pal to retrieve"));
}

void UPalInfo::SummonPal(AActor* Initiator, AActor* Target, FVector SpawnLocation, int32 Index)
{
	if (PalInfos.IsValidIndex(Index))
	{
		if (!PalSummoned)
		{
			PalInfos[Index].Pal->Summon(SpawnLocation, Target);
			PalSummoned = PalInfos[Index].Pal;
			SummonedIndex = Index;
			PalSummoned->CustomTimeDilation = 1.f;

			if (PalSummoned->GetMesh())
			{
				UMaterialInstanceDynamic* Dynamic = UMaterialInstanceDynamic::Create(SummonMaterial, GetOuter());
				if (Dynamic)
				{
					SummonDynamicMaterial = Dynamic;
					FHashedMaterialParameterInfo Info("Emission");
					float Val;
					SummonDynamicMaterial->GetScalarParameterValue(Info, Val);
					DefaultSummonEmissiveVal = Val;

					//SummonDynamicMaterial->SetScalarParameterValue("Emissive", DefaultSummonEmissiveVal);
				}

				for (int i = 0; i < PalSummoned->GetMesh()->GetMaterials().Num(); ++i)
				{
					if (SummonDynamicMaterial)
					{
						PalSummoned->GetMesh()->SetMaterial(i, SummonDynamicMaterial);
					}
				}

				GetWorld()->GetTimerManager().SetTimer(StartSummonHandle, this, &UPalInfo::FinishSummon, SummonDuration, false);

				UParticleSystemComponent* SystemLight = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawnParticleEffects_Light, PalSummoned->GetActorTransform());
				UParticleSystemComponent* SystemSphere = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawnParticleEffects_Sphere, PalSummoned->GetActorTransform());
				if (SystemLight && SystemSphere)
				{
					SystemLight->SetWorldScale3D(FVector(2.f));
					SystemSphere->SetWorldScale3D(FVector(3.5f));
				}
			}
		}
		else UE_LOG(LogTemp, Warning, TEXT("Summoned pal already exists"));
	}
}

void UPalInfo::FinishSummon()
{
	GetWorld()->GetTimerManager().ClearTimer(StartSummonHandle);
	if (PalSummoned && SlotUI)
	{
		if (ensureAlways(PalInfos.IsValidIndex(SummonedIndex)))
		{
			TArray<UMaterialInterface*> PalMaterials = PalSummoned->GetMesh()->GetMaterials();
			for (int i = 0; i < PalMaterials.Num(); ++i)
			{
				PalSummoned->GetMesh()->SetMaterial(i, PalInfos[SummonedIndex].PalMaterials[i]);
			}
		}
	}
}

void UPalInfo::KillSwitch()
{
	if (PalSummoned)
	{
		FHitResult Hit;
		UPStatusLibrary::ApplyDamage(PalSummoned, PalSummoned, Hit.ImpactPoint, 10000);
	}
}

FPalData::FPalData(FString _Name, TSubclassOf<AActor> _PalClass, UTexture2D* _SlotImage, UTexture2D* _PalInfoImage)
{
	Name = _Name;
	PalClass = _PalClass;
	SlotImage = _SlotImage;
	PalInfoImage = _PalInfoImage;
}