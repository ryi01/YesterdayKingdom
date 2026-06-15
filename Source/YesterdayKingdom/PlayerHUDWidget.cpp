// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"

#include "BaseStatComponent.h"
#include "BossWidget.h"
#include "EnemyBase.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "InventoryTabBtnWidget.h"
#include "PlayerCharacter.h"
#include "QuestComponent.h"
#include "QuickSlotWidget.h"
#include "StoreWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"


void UPlayerHUDWidget::BindPlayer(class APlayerCharacter* InPlayer)
{
	OwnerPlayer = InPlayer;
	if (!OwnerPlayer) return;
	OwnerPlayer->OnPlayerDead.RemoveDynamic(this, &UPlayerHUDWidget::PlayerDeadWidget);
	OwnerPlayer->OnPlayerDead.AddDynamic(this, &UPlayerHUDWidget::PlayerDeadWidget);
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

		WBP_InventoryTab->OnInventoryBackRequested.RemoveDynamic(this, &UPlayerHUDWidget::HandleInventoryBackRequested);
		WBP_InventoryTab->OnInventoryBackRequested.AddDynamic(this, &UPlayerHUDWidget::HandleInventoryBackRequested);
	}
	if (WBP_Store)
	{
		WBP_Store->ClearSelectedItem();
		WBP_Store->OnStoreCloseRequested.RemoveDynamic(this, &UPlayerHUDWidget::CloseStore);
		WBP_Store->OnStoreCloseRequested.AddDynamic(this, &UPlayerHUDWidget::CloseStore);
	}
	if (UQuestComponent* QuestComponent = OwnerPlayer->GetQuestComponent())
	{
		BoundQuestComponent = QuestComponent;
		BoundQuestComponent->OnQuestChanged.RemoveDynamic(this, &UPlayerHUDWidget::RefreshQuestDescription);
		BoundQuestComponent->OnQuestChanged.AddDynamic(this, &UPlayerHUDWidget::RefreshQuestDescription);
		RefreshQuestDescription();
	}
	if (UEquipmentComponent* EquipmentComponent = OwnerPlayer->GetEquipmentComponent())
	{
		EquipmentComponent->OnEquipmentChanged.RemoveDynamic(this, &UPlayerHUDWidget::RefreshEquipmentIcons);
		EquipmentComponent->OnEquipmentChanged.AddDynamic(this, &UPlayerHUDWidget::RefreshEquipmentIcons);
		RefreshEquipmentIcons();
	}
	
	SetSwitcherIndex(0);
}

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	QuickSlots.Empty();
	QuickSlots.Add(QuickSlot1);
	QuickSlots.Add(QuickSlot2);
	QuickSlots.Add(QuickSlot3);
	QuickSlots.Add(QuickSlot4);
	QuickSlots.Add(QuickSlot5);

	ClearAllQuickSlots();
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
	SetHUDPage(bVisible ? EHUDPage::Inventory : EHUDPage::Main);
	
}


void UPlayerHUDWidget::UpdateHP(float CurrentHP, float MaxHP)
{
	TargetHPPercent = MaxHP > 0.f ? CurrentHP / MaxHP : 0.f;
	if (TB_HP)
	{
		const int32 CurrentValue = FMath::CeilToInt(CurrentHP);
		const int32 MaxValue = FMath::CeilToInt(MaxHP);
		TB_HP->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentValue, MaxValue)));
	}
}

void UPlayerHUDWidget::UpdateST(float CurrentST, float MaxST)
{
	TargetSTPercent = MaxST > 0.f ? CurrentST / MaxST : 0.f;
	if (TB_ST)
	{
		const int32 CurrentValue = FMath::CeilToInt(CurrentST);
		const int32 MaxValue = FMath::CeilToInt(MaxST);
		TB_ST->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentValue, MaxValue)));
	}
}

void UPlayerHUDWidget::UpdateMP(float CurrentMP, float MaxMP)
{
	TargetMPPercent = MaxMP > 0.f ? CurrentMP / MaxMP : 0.f;
	if (TB_MP)
	{
		const int32 CurrentValue = FMath::CeilToInt(CurrentMP);
		const int32 MaxValue = FMath::CeilToInt(MaxMP);
		TB_MP->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentValue, MaxValue)));
	}
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

void UPlayerHUDWidget::HandleInventoryBackRequested()
{
	if (OwnerPlayer)
	{
		OwnerPlayer->CloseInventory();
	}
}

void UPlayerHUDWidget::SetVisibleBossHPBar(bool bEnable)
{
	const ESlateVisibility NewVisibility  = bEnable ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	WBP_BossHP->SetVisibility(NewVisibility );
}

//=====================================================================================================
// 퀵 슬롯
//=====================================================================================================
void UPlayerHUDWidget::SetQuickSlot(int32 QuickSlotIndex, const FInventorySlotViewData& SlotData)
{
	if (!QuickSlots.IsValidIndex(QuickSlotIndex)) return;
	if (!QuickSlots[QuickSlotIndex]) return;

	QuickSlots[QuickSlotIndex]->SetQuickSlotData(SlotData);
}

void UPlayerHUDWidget::UpdateQuickSlot(int32 QuickSlotIndex, FName ItemRowName)
{
	if (!OwnerPlayer) return;
	if (!QuickSlots.IsValidIndex(QuickSlotIndex)) return;
	if (!QuickSlots[QuickSlotIndex]) return;

	UInventoryComponent* InventoryComponent = OwnerPlayer->GetInventoryComponent();
	if (!InventoryComponent) return;

	if (ItemRowName.IsNone())
	{
		ClearQuickSlot(QuickSlotIndex);
		return;
	}

	const TArray<FInventorySlotViewData> AllSlotData = InventoryComponent->GetAllSlotViewData();

	for (const FInventorySlotViewData& SlotData : AllSlotData)
	{
		if (SlotData.ItemRowName == ItemRowName)
		{
			SetQuickSlot(QuickSlotIndex, SlotData);
			return;
		}
	}

	ClearQuickSlot(QuickSlotIndex);
}

void UPlayerHUDWidget::ClearQuickSlot(int32 QuickSlotIndex)
{
	if (!QuickSlots.IsValidIndex(QuickSlotIndex)) return;
	if (!QuickSlots[QuickSlotIndex]) return;

	QuickSlots[QuickSlotIndex]->ClearQuickSlot();
}

void UPlayerHUDWidget::ClearAllQuickSlots()
{
	for (UQuickSlotWidget* QuickSlot : QuickSlots)
	{
		if (!QuickSlot) continue;

		QuickSlot->ClearQuickSlot();
	}
}
//=====================================================================================================
// 스토어
//=====================================================================================================
void UPlayerHUDWidget::OpenStore(UStoreComponent* InStoreComponent)
{
	if (!InStoreComponent || !WBP_Store) return;
	UInventoryComponent* InventoryComponent = OwnerPlayer->GetInventoryComponent();
	UGoldComponent* GoldComponent = OwnerPlayer->GetGoldComponent();
	
	WBP_Store->BindStore(InStoreComponent, InventoryComponent, GoldComponent);
	SetHUDPage(EHUDPage::Store);
}

void UPlayerHUDWidget::CloseStore()
{
	if (!OwnerPlayer) return;
	SetHUDPage(EHUDPage::Main);
	OwnerPlayer->CloseStoreUI();
}


void UPlayerHUDWidget::SetHUDPage(EHUDPage Page)
{
	if (!WS_HUD) return;

	switch (Page)
	{
	case EHUDPage::Main:
		WS_HUD->SetActiveWidgetIndex(0);
		break;

	case EHUDPage::Inventory:
		WS_HUD->SetActiveWidgetIndex(1);
		break;

	case EHUDPage::Store:
		WS_HUD->SetActiveWidgetIndex(2);
		break;
	case EHUDPage::Dead:
		WS_HUD->SetActiveWidgetIndex(3);
		break;
	}
}
//=====================================================================================================
// 스토어
//=====================================================================================================
void UPlayerHUDWidget::RefreshQuestDescription()
{
	if (!TB_QuestDes || !BoundQuestComponent) return;
	FQuestDataRow QuestDataRow;
	if (!BoundQuestComponent->GetCurrentQuestData(QuestDataRow))
	{
		TB_QuestDes->SetText(FText::FromString(TEXT("모든 퀘스트 완료")));
		return;
	}
	const FQuestInstance QuestInstance = BoundQuestComponent->GetCurrentQuestInstance();
	const FText QuestText = FText::Format(FText::FromString(TEXT("{0}\n{1} / {2}")), QuestDataRow.Description,FText::AsNumber(QuestInstance.CurrentCount),FText::AsNumber(QuestInstance.TargetCount));
	TB_QuestDes->SetText(QuestText);
}

//=====================================================================================================
// 장비창
//=====================================================================================================
void UPlayerHUDWidget::RefreshEquipmentIcons()
{
	if (!OwnerPlayer) return;
	UEquipmentComponent* EquipmentComponent = OwnerPlayer->GetEquipmentComponent();
	if (!EquipmentComponent) return;
	SetEquipmentIcon(Armor1, EquipmentComponent, EEquipmentSlotType::Helmet, DefaultHelmetIcon);
	SetEquipmentIcon(Armor2, EquipmentComponent, EEquipmentSlotType::Armor, DefaultArmorIcon);
	SetEquipmentIcon(Armor3, EquipmentComponent, EEquipmentSlotType::Boots, DefaultBootsIcon);
}

void UPlayerHUDWidget::SetEquipmentIcon(UImage* ImageWidget, UEquipmentComponent* EquipmentComponent, EEquipmentSlotType SlotType, UTexture2D* DefaultIcon)
{
	if (!ImageWidget || !EquipmentComponent) return;
	const FName ItemRowName = EquipmentComponent->GetEquippedItem(SlotType);
	
	if (ItemRowName.IsNone())
	{
		ImageWidget->SetBrushFromTexture(DefaultIcon);
		return;
	}

	const FItemData* ItemData = EquipmentComponent->GetItemData(ItemRowName);

	if (!ItemData || !ItemData->Icon)
	{
		ImageWidget->SetBrushFromTexture(DefaultIcon);
		return;
	}

	ImageWidget->SetBrushFromTexture(ItemData->Icon);
}
void UPlayerHUDWidget::PlayerDeadWidget()
{
	SetHUDPage(EHUDPage::Dead);
	if (OwnerPlayer)
	{
		OwnerPlayer->SetUIMode(true);
	}
}
