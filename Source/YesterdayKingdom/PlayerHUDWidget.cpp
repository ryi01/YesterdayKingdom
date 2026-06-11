// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"

#include "BaseStatComponent.h"
#include "BossWidget.h"
#include "EnemyBase.h"
#include "InventoryTabBtnWidget.h"
#include "PlayerCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetSwitcher.h"


void UPlayerHUDWidget::BindPlayer(class APlayerCharacter* InPlayer)
{
	OwnerPlayer = InPlayer;
	if (!OwnerPlayer) return;
	if (UBaseStatComponent* StatComponent = OwnerPlayer->GetStatComponent())
	{
		StatComponent->OnHPChanged.AddDynamic(this, &UPlayerHUDWidget::UpdateHP);
		StatComponent->OnSTChanged.AddDynamic(this, &UPlayerHUDWidget::UpdateST);
		StatComponent->OnMPChanged.AddDynamic(this, &UPlayerHUDWidget::UpdateMP);

		UpdateHP(StatComponent->GetCurrentHP(), StatComponent->GetMaxHP());
		UpdateST(StatComponent->GetCurrentST(), StatComponent->GetMaxST());
		UpdateMP(StatComponent->GetCurrentMP(), StatComponent->GetMaxMP());
	}

	if (WBP_BossHP) SetVisibleBossHPBar(false);
	if (WBP_InventoryTab)
	{
		WBP_InventoryTab->SetInventoryComponent(OwnerPlayer->GetInventoryComponent());
	}
	SetSwitcherIndex(0);
}

void UPlayerHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (HP)
	{
		const float CurrentPercent = HP->GetPercent();
		const float NewPercent = FMath::FInterpTo(CurrentPercent, TargetHPPercent, InDeltaTime, BarInterpSpeed);
		HP->SetPercent(NewPercent);
	}

	if (ST)
	{
		const float CurrentPercent = ST->GetPercent();
		const float NewPercent = FMath::FInterpTo(CurrentPercent, TargetSTPercent, InDeltaTime, BarInterpSpeed);
		ST->SetPercent(NewPercent);
	}

	if (MP)
	{
		const float CurrentPercent = MP->GetPercent();
		const float NewPercent = FMath::FInterpTo(CurrentPercent, TargetMPPercent, InDeltaTime, BarInterpSpeed);
		MP->SetPercent(NewPercent);
	}
}

void UPlayerHUDWidget::SetSwitcherIndex(int32 index)
{
	WS_HUD->SetActiveWidgetIndex(index);
}


void UPlayerHUDWidget::SetInventoryVisible(bool bVisible)
{
	FString InventoryOpen = bVisible ? TEXT("OPEN INVENTORY") :  TEXT("CLOSE INVENTORY") ;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *InventoryOpen);
	if (bVisible) SetSwitcherIndex(1);
	else SetSwitcherIndex(0);
	
}


void UPlayerHUDWidget::UpdateHP(float CurrentHP, float MaxHP)
{
	TargetHPPercent = MaxHP > 0.f ? CurrentHP / MaxHP : 0.f;
}

void UPlayerHUDWidget::UpdateST(float CurrentST, float MaxST)
{
	TargetSTPercent = MaxST > 0.f ? CurrentST / MaxST : 0.f;
}

void UPlayerHUDWidget::UpdateMP(float CurrentMP, float MaxMP)
{
	TargetMPPercent = MaxMP > 0.f ? CurrentMP / MaxMP : 0.f;
}
//=====================================================================================================
// 보스 체력바 관련
//=====================================================================================================

void UPlayerHUDWidget::BindBoss(AEnemyBase* Boss)
{
	if (!Boss || !WBP_BossHP) return;
	UnbindBoss();
	
	BoundBoss = Boss;
	BoundBossStatComponent = Boss->GetStatComponent();
	
	if (!BoundBossStatComponent) return;
	
	BoundBossStatComponent->OnHPChanged.AddDynamic(this, &UPlayerHUDWidget::HandleBossHPChanged);
	SetVisibleBossHPBar(true);
	WBP_BossHP->SetBossHP(BoundBossStatComponent->GetCurrentHP(), BoundBossStatComponent->GetMaxHP());
}

void UPlayerHUDWidget::UnbindBoss()
{
	if (BoundBossStatComponent)
	{
		BoundBossStatComponent->OnHPChanged.RemoveDynamic(this,&UPlayerHUDWidget::HandleBossHPChanged);
	}
	BoundBoss = nullptr;
	BoundBossStatComponent = nullptr;

	SetVisibleBossHPBar(false);
}

void UPlayerHUDWidget::HandleBossHPChanged(float CurrentHP, float MaxHP)
{
	if (!WBP_BossHP) return;

	WBP_BossHP->SetBossHP(CurrentHP, MaxHP);

	if (CurrentHP <= 0.f && BoundBoss->GetCurrentPhase() > 1)
	{
		SetVisibleBossHPBar(false);
	}
}

void UPlayerHUDWidget::SetVisibleBossHPBar(bool bEnable)
{
	const ESlateVisibility NewVisibility  = bEnable ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	WBP_BossHP->SetVisibility(NewVisibility );
}