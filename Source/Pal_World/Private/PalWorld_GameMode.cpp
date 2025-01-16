// Fill out your copyright notice in the Description page of Project Settings.


#include "PalWorld_GameMode.h"
#include "Justin/PPalCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sunny/PCharacter.h"
#include "Justin/Components/PStatusComponent.h"
#include "Justin/Widgets/MatchOverWidget.h"
#include "Eddie/GrizzBolt.h"
#include "Eddie/GrizzBoltActorComponent.h"
#include "Justin/Widgets/FadeWidget.h"
#include "Components/AudioComponent.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

APalWorld_GameMode::APalWorld_GameMode()
{
	DefeatedWidgetDelay = 2.f;
	BossEliminatedWidgetDelay = 2.f;
	bMatchOver = false;

	AudioComp = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
}

void APalWorld_GameMode::RemoveWidgets()
{ 
	if (IsValid(DefeatedWidget))
		DefeatedWidget->RemoveFromParent();
	if (IsValid(BossEliminatedWidget)) 
		BossEliminatedWidget->RemoveFromParent();
}

USoundBase* APalWorld_GameMode::GetSound(EBGM BGMSound) const
{
	auto BGM = BGMSounds.Find(BGMSound);
	if (BGM && *BGM)
	{
		return *BGM;
	}
	return nullptr;
}

void APalWorld_GameMode::SetupPlayerAndBoss()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, FString::Printf(TEXT("Test BeginPlay Game mode")));

	SetupPlayer();
	SetupBoss();

	if (Player)
	{
		auto PC = Player->GetController<APlayerController>();
		if (PC)
		{
			PC->SetInputMode(FInputModeGameOnly());
		}

		auto StatusComp = Player->GetComponentByClass<UPStatusComponent>();
		if (StatusComp)
		{
			StatusComp->OnDied.AddUObject(this, &APalWorld_GameMode::OnPlayerDead);
		}
	}

	if (Boss)
	{
		auto StatusComp = Boss->GetComponentByClass<UPStatusComponent>();
		if (StatusComp)
		{
			StatusComp->OnDied.AddUObject(this, &APalWorld_GameMode::OnBossDead);
		}

		Boss->OnCountDownEnd.BindUObject(this, &APalWorld_GameMode::OnCountDownEnd);
	}

	if (AudioComp)
	{
		auto BossFight = BGMSounds.Find(EBGM::BOSSFIGHT);
		if (BossFight && *BossFight)
		{
			AudioComp->SetSound(*BossFight);
			AudioComp->Play();
			AudioComp->FadeIn(1.5f);
		}
	}
}

bool APalWorld_GameMode::IsMatchOver() const
{
	return bMatchOver;
}

void APalWorld_GameMode::SetupPlayer()
{
	//FString Name = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
	//if (Name != TEXT("Level_ProtoTestMap")) return;

	auto Actor = UGameplayStatics::GetActorOfClass(GetWorld(), APCharacter::StaticClass());
	Player = Cast<APCharacter>(Actor);

	if (!Player)
	{
		//Spawn Player
		TArray<APlayerStart*> Start;
		TArray<AActor*> StartActor;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), StartActor);

		for (auto what : StartActor)
		{
			Start.Add(Cast<APlayerStart>(what));
		}

		FTransform PlayerTrans;

		for (auto Huh : Start)
		{
			if (Huh->PlayerStartTag == "PlayerStart")
			{
				Actor = GetWorld()->SpawnActorDeferred<APCharacter>(PlayerActorClass, Huh->GetActorTransform());
				if (Actor)
				{
					Player = Cast<APCharacter>(Actor);
					PlayerTrans = Actor->GetActorTransform();
				}
				else
				{
					FString SubLevelName = Huh->GetLevel()->GetOuter()->GetName();
					UE_LOG(LogTemp, Warning, TEXT("Failed to Spawn Player in Level %s"), *SubLevelName);
				}
			}
		}

		if (Player)
		{
			auto PC = GetWorld()->GetFirstPlayerController();
			if (PC)
			{
				PC->Possess(Player);
				UGameplayStatics::FinishSpawningActor(Player, PlayerTrans);
			}
		}

		//for (TActorIterator<APlayerStart> Iter(GetWorld()); !Iter; ++Iter)
		//{
		//	if (*Iter)
		//	{
		//		if (Iter->ActorHasTag("PlayerStart"))
		//		{
		//			FString Name = Iter->GetLevel()->GetOuter()->GetName();
		//			UE_LOG(LogTemp, Warning, TEXT("Player Start : %s"), *Name);

		//			//GetWorld()->SpawnActorDeferred(PlayerActorClass, )
		//		}
		//		else if (Iter->ActorHasTag("BossStart"))
		//		{
		//			UE_LOG(LogTemp, Warning, TEXT("Boss Start"));
		//		}
		//	}
		//}	
	}
}

void APalWorld_GameMode::SetupBoss()
{
	auto Actor = UGameplayStatics::GetActorOfClass(GetWorld(), AGrizzBolt::StaticClass());
	Boss = Cast<AGrizzBolt>(Actor);

	if (!Boss)
	{
		//Spawn Boss
		TArray<APlayerStart*> Start;
		TArray<AActor*> StartActor;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), StartActor);

		for (auto what : StartActor)
		{
			Start.Add(Cast<APlayerStart>(what));
		}

		FTransform BossTrans;

		for (auto Huh : Start)
		{
			if (Huh->PlayerStartTag == "BossStart")
			{
				Actor = GetWorld()->SpawnActorDeferred<AGrizzBolt>(BossActorClass, Huh->GetActorTransform());
				if (Actor)
				{
					Boss = Cast<AGrizzBolt>(Actor);
					BossTrans = Actor->GetActorTransform();
				}
				else
				{
					FString SubLevelName = Huh->GetLevel()->GetOuter()->GetName();
					UE_LOG(LogTemp, Warning, TEXT("Failed to Spawn Boss in Level %s"), *SubLevelName);
				}
			}
		}

		if (Boss)
		{
			UGameplayStatics::FinishSpawningActor(Boss, BossTrans);
		}
	}
}

void APalWorld_GameMode::OnPlayerDead()
{
	if (!bMatchOver)
	{
		bMatchOver = true;
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &APalWorld_GameMode::OnDefeated, DefeatedWidgetDelay);

		if (AudioComp)
		{
			AudioComp->FadeOut(DefeatedWidgetDelay, 0.f);
		}

		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
	}
}

void APalWorld_GameMode::OnCountDownEnd()
{
	if (!bMatchOver)
	{
		bMatchOver = true;
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &APalWorld_GameMode::OnDefeated, DefeatedWidgetDelay);

		if (AudioComp)
		{
			AudioComp->FadeOut(DefeatedWidgetDelay, 0.f);
		}
	}
}

void APalWorld_GameMode::OnBossDead()
{
	if (!bMatchOver)
	{
		bMatchOver = true;
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &APalWorld_GameMode::OnBossEliminated, BossEliminatedWidgetDelay, false);

		if (AudioComp)
		{
			AudioComp->FadeOut(BossEliminatedWidgetDelay, 0.f);
		}
	}
}

void APalWorld_GameMode::OnDefeated()
{
	UE_LOG(LogTemp, Warning, TEXT("Player is Dead!"));

	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());

	DefeatedWidget = CreateWidget<UMatchOverWidget>(GetWorld(), DefeatedWidgetClass);
	if (DefeatedWidget)
	{
		DefeatedWidget->Pressed.BindUObject(this, &APalWorld_GameMode::ButtonPressed);

		DefeatedWidget->AddToViewport(5);
		if (Player)
		{
			auto PC = Player->GetController<APlayerController>();
			if (PC)
			{
				PC->SetInputMode(FInputModeUIOnly());
				PC->SetShowMouseCursor(true);
			}
		}
	}

	auto Sound = BGMSounds.Find(EBGM::THUNDER);
	if (Sound && *Sound)
	{
		AudioComp->SetSound(*Sound);
		AudioComp->Play();
	}
}

void APalWorld_GameMode::OnBossEliminated()
{
	UE_LOG(LogTemp, Warning, TEXT("Boss is Dead!"));

	//UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());

	BossEliminatedWidget = CreateWidget<UMatchOverWidget>(GetWorld(), BossEliminatedWidgetClass);
	if (BossEliminatedWidget)
	{
		BossEliminatedWidget->Pressed.BindUObject(this, &APalWorld_GameMode::ButtonPressed);

		BossEliminatedWidget->AddToViewport(5);

		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &APalWorld_GameMode::ButtonPressed, 12.f, false);

		//if (Player)
		//{
		//	auto PC = Player->GetController<APlayerController>();
		//	if (PC)
		//	{
		//		PC->SetIgnoreMoveInput(true);
		//		PC->SetIgnoreLookInput(true);
		//		PC->SetShowMouseCursor(true);
		//		PC->SetInputMode(FInputModeUIOnly());
		//	}
		//}
	}
}

void APalWorld_GameMode::ButtonPressed()
{
	OnReturnPressed.Broadcast();
	AudioComp->FadeOut(.8f, 0.f);
	bMatchOver = false;
}
