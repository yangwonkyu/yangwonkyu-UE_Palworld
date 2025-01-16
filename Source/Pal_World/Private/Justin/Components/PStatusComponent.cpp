// Fill out your copyright notice in the Description page of Project Settings.


#include "Justin/Components/PStatusComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Justin/TestWidget.h"
#include "Sunny/PCharacter.h"
#include "Sunny/GunPlayerAnimInstance.h"
#include "Sunny/DamageTextUI.h"
#include "TimerManager.h"
#include "Justin/PPalCharacter.h"


UPStatusComponent::UPStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	MaxHp = 200.f;
	Hp = 0.f;
	Initialized = false;
}

void UPStatusComponent::BeginPlay()
{
	Super::BeginPlay();
	Hp = MaxHp;
	if (HealthWidgetClass)
	{
		auto Widget = CreateWidget(GetWorld(), HealthWidgetClass);
		if (Widget)
		{
			// 프로그레스 바 초기화
			UWidget* Test = Widget->GetWidgetFromName(TEXT("ProgressBar_Health"));
			ProgressBar_Health = Cast<UProgressBar>(Test);

			if (ProgressBar_Health)
			{
				Text_CurrentHealth = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("Text_CurrentHealth")));
				Text_MaxHealth = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("Text_MaxHealth")));

				HealthWidget = Widget;
				HealthWidget->AddToViewport();
				SetHealthPercent(Hp, MaxHp);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to initialize ProgressBar or DamageBar"));
			}

			auto Player = GetOwner<APCharacter>();
			// 각각의 컴포넌트가 nullptr이 아닌지 확인
			if (Player)
			{
				DamageBar = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("DamageBar")));
				if (DamageBar)
				{
					HideDamageBar();
				}
			}			
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create HealthWidget"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthWidgetClass is null"));
	}

	Initialized = true;

}

void UPStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPStatusComponent::ShowLowHealthUI()
{
	if (bIsLowHealthUIVisible)
	{
		return; // 이미 UI가 보이고 있으면 함수 종료
	}

	if (LowHealthUIClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::LowHealthUIClass OK"));
		LowHealthWidget = CreateWidget<UUserWidget>(GetWorld(), LowHealthUIClass);
		if (LowHealthWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::LowHealthWidget OK"));
			LowHealthWidget->AddToViewport();
			bIsLowHealthUIVisible = true;
			//LowHealthWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::LowHealthWidget No"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PCharacter::BeginPlay::LowHealthUIClass No"));
	}
}

void UPStatusComponent::HideWidget()
{
	HealthWidget->SetVisibility(ESlateVisibility::Collapsed);
	HealthWidget->RemoveFromParent();
}

void UPStatusComponent::ShowDamageTextUI(AActor* Initiator, FVector ImpactLocation, float Damage)
{
	UE_LOG(LogTemp, Warning, TEXT("UPStatusComponent::ShowDamageTextUI()"));

	if (DamageTextUIClass)
	{
		if (DamageTextUI && DamageTextUI->IsInViewport())
		{
			DamageTextUI->RemoveFromParent(); // 이전의 위젯을 제거
		}

		// DamageTextUI 위젯을 생성
		DamageTextUI = CreateWidget<UDamageTextUI>(GetWorld(), DamageTextUIClass);
		if (DamageTextUI)
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				// ImpactLocation에 랜덤 오프셋 추가 (1cm 범위 내, 360도 방향)
				FVector RandomDirection = FMath::VRand(); // 임의의 방향 생성
				FVector Offset = RandomDirection * 1.0f; // 1cm 거리로 스케일링
				ImpactLocation += Offset;

				// ImpactLocation을 화면 좌표로 변환
				FVector2D ScreenLocation;
				if (PlayerController->ProjectWorldLocationToScreen(ImpactLocation, ScreenLocation))
				{
					// DamageTextUI에 화면 좌표와 데미지 값을 전달
					DamageTextUI->ShowDamageTextUI(ScreenLocation, Damage);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to project world location to screen in ShowDamageTextUI"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("PlayerController is null in ShowDamageTextUI"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create DamageWidget in ShowDamageTextUI"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DamageTextUIClass is null in ShowDamageTextUI"));
	}


	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UPStatusComponent::HideDamageTextUI, 0.5f, false);
}

void UPStatusComponent::HideDamageTextUI()
{
	UE_LOG(LogTemp, Warning, TEXT("UPStatusComponent::HideDamageTextUI()"));

	if (DamageTextUI)
	{
		DamageTextUI->HideDamageTextUI();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DamageTextUI is null in HideDamageTextUI"));
	}

}


void UPStatusComponent::TakeDamage(AActor* Initiator, FVector ImpactLocation, float Damage)
{
	if (IsDead()) return;

	ShowDamageBar(Hp, MaxHp);
	float SetHp = Hp - Damage;
	Hp = SetHp;
	SetHealthPercent(Hp, MaxHp);
	ShowDamageTextUI(Initiator, ImpactLocation, Damage);

	if (Hp <= MaxHp * 0.3f)
	{
		ShowLowHealthUI();
	}

	auto* Player = GetOwner<APCharacter>();
	//auto* Pal = GetOwner<APPalCharacter>();
	if (Player)//(Initiator != Player) || Initiator != Pal)
	{
		if (HitCameraShakeClass)
		{
			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(HitCameraShakeClass);
		}

		check(DamageSFX);
		if (DamageSFX)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), DamageSFX);
		}
	}

	if (IsDead())
	{
		if (Player)
		{
			Player->bPlayerDead = true;
			if (Player && Player->Anim)
			{
				Player->Anim->PlayDeathMontage();
				//Player->OnGameOver();
			}			
		}
	}
	OnHealthChanged.Broadcast(Hp, MaxHp);
	if (IsDead()) OnDied.Broadcast();
}

float UPStatusComponent::GetHealth() const
{
	return Hp;
}

float UPStatusComponent::GetMaxHealth() const
{
	return MaxHp;
}

bool UPStatusComponent::IsDead() const
{
	return Hp <= 0.f;
}

bool UPStatusComponent::IsInitialized() const
{
	return Initialized;
}

void UPStatusComponent::SetHealthPercent(float InHealth, float InMaxHealth)
{
	if (ProgressBar_Health)
	{
		float Percent = InHealth / InMaxHealth;
		ProgressBar_Health->SetPercent(Percent);
		UE_LOG(LogTemp, Warning, TEXT("HealthBar->SetPercent called with Percent: %f"), Percent);

		// 체력에 따른 색깔 변경
		FLinearColor BarColor = FLinearColor::Green; // 기본 색상: 녹색

		if (Percent <= 0.4f)
		{
			BarColor = FLinearColor::Red; // 체력이 40% 이하일 때 빨간색으로 변경
		}

		ProgressBar_Health->SetFillColorAndOpacity(BarColor);
	}

	if (Text_CurrentHealth)
	{
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0);
		if (Hp <= 0)
		{
			Hp = 0;
		}
		Text_CurrentHealth->SetText(FText::AsNumber(Hp, &Opts));
	}

	if (Text_MaxHealth)
	{
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0);
		Text_MaxHealth->SetText(FText::AsNumber(MaxHp, &Opts));
	}
}

void UPStatusComponent::ShowDamageBar(float InHealth, float InMaxHealth)
{
	//if (DamageBar)
	//{
	//	DamageBar->SetVisibility(ESlateVisibility::Visible);

	//	float Percent = InHealth / InMaxHealth;
	//	DamageBar->SetPercent(Percent);
	//	//DamageBar->SetRenderOpacity(1.0f);  // 투명도를 최대화하여 DamageBar를 완전히 보이게 함
	//	UE_LOG(LogTemp, Log, TEXT("UPStatusComponent::ShowDamageBar() ::  Hp: %f, MaxHp: %f, Percent: %f"), InHealth, InMaxHealth, Percent);
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("DamageBar is nullptr in ShowDamageBar()"));
	//}

	FTimerHandle HideDanageBarTimerHandle;
	////GetWorld()->GetTimerManager().SetTimer(HideDanageBarTimerHandle, this, &UPStatusComponent::HideDamageBar, 0.2f, false);

	//// 타이머가 이미 활성화되어 있으면, 다시 시작하여 타이머를 연장
	//GetWorld()->GetTimerManager().ClearTimer(HideDanageBarTimerHandle);
	//GetWorld()->GetTimerManager().SetTimer(HideDanageBarTimerHandle, this, &UPStatusComponent::HideDamageBar, 0.8f, false);  // 시간은 상황에 따라 조정

	if (DamageBar && DamageBar->IsValidLowLevelFast())
	{
		// DamageBar 상태 업데이트
		float Percent = InHealth / InMaxHealth;
		DamageBar->SetPercent(Percent);
		UE_LOG(LogTemp, Log, TEXT("UPStatusComponent::ShowDamageBar() :: Hp: %f, MaxHp: %f, Percent: %f"), InHealth, InMaxHealth, Percent);

		if (!bIsDamageBarVisible)
		{
			DamageBar->SetVisibility(ESlateVisibility::Visible);
			bIsDamageBarVisible = true;
		}

		// 기존 타이머를 재설정하여 새로운 시간으로 연장
		GetWorld()->GetTimerManager().ClearTimer(HideDanageBarTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(HideDanageBarTimerHandle, this, &UPStatusComponent::HideDamageBar, 0.5f, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ShowDamageBar: DamageBar is nullptr or invalid"));
	}
}
void UPStatusComponent::HideDamageBar()
{
	if (DamageBar && DamageBar->IsValidLowLevelFast())
	{
		DamageBar->SetVisibility(ESlateVisibility::Hidden);
		bIsDamageBarVisible = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HideDamageBar: DamageBar is nullptr or invalid"));
	}
}
