// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "InventoryWidget.h"

#include "EquipmentComponent.h"
#include "GoldComponent.h"
#include "InventoryComponent.h"
#include "InventoryTabBtnWidget.h"
#include "ItemSlotWidget.h"
#include "MoneyWidget.h"
#include "PlayerCharacter.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ClearSelectedItem();
	ClearItemDescription();
	if (BTN_Use)
	{
		BTN_Use->OnClicked.RemoveDynamic(this, &UInventoryWidget::OnUseClicked);
		BTN_Use->OnClicked.AddDynamic(this, &UInventoryWidget::OnUseClicked);
	}
	if (BTN_Remove)
	{
		BTN_Remove->OnClicked.RemoveDynamic(this, &UInventoryWidget::OnRemoveClicked);
		BTN_Remove->OnClicked.AddDynamic(this, &UInventoryWidget::OnRemoveClicked);
	}
	if (BTN_All)
	{
		BTN_All->OnClicked.RemoveDynamic(this, &UInventoryWidget::OnAllFilterClicked);
		BTN_All->OnClicked.AddDynamic(this, &UInventoryWidget::OnAllFilterClicked);
	}

	if (BTN_CB)
	{
		BTN_CB->OnClicked.RemoveDynamic(this, &UInventoryWidget::OnCBFilterClicked);
		BTN_CB->OnClicked.AddDynamic(this, &UInventoryWidget::OnCBFilterClicked);
	}

	if (BTN_Weapon)
	{
		BTN_Weapon->OnClicked.RemoveDynamic(this, &UInventoryWidget::OnWBFilterClicked);
		BTN_Weapon->OnClicked.AddDynamic(this, &UInventoryWidget::OnWBFilterClicked);
	}

	if (BTN_Armor)
	{
		BTN_Armor->OnClicked.RemoveDynamic(this, &UInventoryWidget::OnArmorFilterClicked);
		BTN_Armor->OnClicked.AddDynamic(this, &UInventoryWidget::OnArmorFilterClicked);
	}
}

void UInventoryWidget::BindInventory(class UInventoryComponent* InInventory)
{
	InventoryComponent = InInventory;
	if (!InventoryComponent) return;

	InventoryComponent->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::RefreshInventory);
	RefreshInventory();
}

void UInventoryWidget::RefreshInventory()
{
	if (!InventoryComponent || !GP_Item || !ItemSlotWidgetClass) return;

	GP_Item->ClearChildren();
	SlotWidgets.Empty();

	const TArray<FInventorySlot>& InventorySlots = InventoryComponent->GetItemSlots();
	if (!InventorySlots.IsValidIndex(SelectedSlotIndex) || InventorySlots[SelectedSlotIndex].IsEmpty())
	{
		ClearSelectedItem();
	}

	TArray<FInventorySlotViewData> ViewDataList;
	if (bShowAllItems)
	{
		ViewDataList = InventoryComponent->GetAllSlotViewData();
	}
	else
	{
		ViewDataList = InventoryComponent->GetSlotViewDataByType(CurrentFilterType);
	}
	
	int32 DisplayIndex = 0;
	
	for (const FInventorySlotViewData& ViewData : ViewDataList)
	{
		UItemSlotWidget* SlotWidget = CreateWidget<UItemSlotWidget>(GetOwningPlayer(), ItemSlotWidgetClass);
		if (!SlotWidget) return;
		SlotWidget->OnItemSlotClicked.AddDynamic(this, &UInventoryWidget::OnItemSlotClicked);
		SlotWidget->SetSlotData(ViewData);
		SlotWidget->SetSelected(ViewData.SlotIndex == SelectedSlotIndex);
		const int32 Row = DisplayIndex / ColumnCount;
		const int32 Column = DisplayIndex % ColumnCount;
		if (UGridSlot* GridSlot = GP_Item->AddChildToGrid(SlotWidget, Row, Column))
		{
			GridSlot->SetPadding(FMargin(4.f));
		}
		SlotWidgets.Add(SlotWidget);
		DisplayIndex++;
	}
}
//=======================================================================================
// 아이템 설명
//=======================================================================================
void UInventoryWidget::OnItemSlotClicked(FInventorySlotViewData SlotData)
{
	if (SlotData.IsEmpty())
	{
		ClearSelectedItem();
		return;
	}

	SelectedSlotIndex = SlotData.SlotIndex;
	SelectedSlotData = SlotData;
	bHasSelectedItem = true;

	UpdateSelectedSlotVisual();
	SetItemDescription(SlotData);
	UpdateUseButtonByItemType(SlotData);
}

void UInventoryWidget::UpdateUseButtonByItemType(const FInventorySlotViewData& SlotData)
{
	if (!InventoryComponent || !TB_Use || !BTN_Use) return;

	const FItemData* ItemData = InventoryComponent->GetItemData(SlotData.ItemRowName);
	if (!ItemData) return;

	switch (ItemData->ItemType)
	{
	case EItemType::Consumable:
		TB_Use->SetText(FText::FromString(TEXT("Use")));
		BTN_Use->SetIsEnabled(true);
		break;

	case EItemType::Weapon:
	case EItemType::Armor:
		TB_Use->SetText(FText::FromString(TEXT("Equip")));
		BTN_Use->SetIsEnabled(true);
		break;

	case EItemType::Quest:
		TB_Use->SetText(FText::FromString(TEXT("Cannot Use")));
		BTN_Use->SetIsEnabled(false);
		break;

	default:
		TB_Use->SetText(FText::FromString(TEXT("Use")));
		BTN_Use->SetIsEnabled(false);
		break;
	}
}

void UInventoryWidget::ResetUseButton()
{
	if (TB_Use)
	{
		TB_Use->SetText(FText::FromString(TEXT("Use")));
	}

	if (BTN_Use)
	{
		BTN_Use->SetIsEnabled(false);
	}
}

void UInventoryWidget::SetItemDescription(const FInventorySlotViewData& SlotData)
{
	if (T_ItemName)
	{
		T_ItemName->SetText(SlotData.ItemName);
	}
	if (TB_ItemDescription)
	{
		TB_ItemDescription->SetText(SlotData.ItemDescription);
	}

	if (TB_BuffDescription)
	{
		TB_BuffDescription->SetText(SlotData.EffectDescription);
	}

	if (I_Item)
	{
		I_Item->SetBrushFromTexture(SlotData.Icon);
		I_Item->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UInventoryWidget::UpdateSelectedSlotVisual()
{
	for (UItemSlotWidget* SlotWidget : SlotWidgets)
	{
		if (!SlotWidget) continue;

		SlotWidget->SetSelected(SlotWidget->GetSlotIndex() == SelectedSlotIndex);
	}
}


void UInventoryWidget::ClearItemDescription()
{
	if (T_ItemName)
	{
		T_ItemName->SetText(FText::GetEmpty());
	}
	if (TB_ItemDescription)
	{
		TB_ItemDescription->SetText(FText::GetEmpty());
	}

	if (TB_BuffDescription)
	{
		TB_BuffDescription->SetText(FText::GetEmpty());
	}

	if (I_Item)
	{
		I_Item->SetBrushFromTexture(nullptr);
		I_Item->SetVisibility(ESlateVisibility::Hidden);
	}
}
//=======================================================================================
// 아이템 사용
//=======================================================================================
void UInventoryWidget::OnUseClicked()
{
	if (!InventoryComponent) return;
	if (!bHasSelectedItem) return;
	if (SelectedSlotIndex == INDEX_NONE) return;

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (!PlayerCharacter) return;

	const FName UsedItemRowName = SelectedSlotData.ItemRowName;

	const FItemData* ItemData = InventoryComponent->GetItemData(UsedItemRowName);
	if (!ItemData) return;

	switch (ItemData->ItemType)
	{
	case EItemType::Consumable:
		{
			const bool bUsed = PlayerCharacter->UseConsumableItem(*ItemData);
			if (!bUsed) return;

			if (!InventoryComponent->RemoveItemAt(SelectedSlotIndex, 1)) return;

			PlayerCharacter->RefreshQuickSlotByItem(UsedItemRowName);
			ClearSelectedItem();
			break;
		}

	case EItemType::Weapon:
	case EItemType::Armor:
		{
			UEquipmentComponent* EquipmentComponent = PlayerCharacter->GetEquipmentComponent();
			if (!EquipmentComponent) return;

			const bool bEquipped = EquipmentComponent->EquipItem(UsedItemRowName);
			if (!bEquipped) return;

			ClearSelectedItem();
			break;
		}

	case EItemType::Quest:
		{
			UE_LOG(LogTemp, Warning, TEXT("[InventoryWidget] Quest item cannot be used."));
			break;
		}

	default:
		break;
	}
}

void UInventoryWidget::OnSellClicked()
{
	if (!InventoryComponent) return;
	if (!bHasSelectedItem) return;
	if (SelectedSlotIndex == INDEX_NONE) return;

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (!PlayerCharacter) return;

	UGoldComponent* GoldComponent = PlayerCharacter->GetGoldComponent();
	if (!GoldComponent) return;

	const FName SoldItemRowName = SelectedSlotData.ItemRowName;

	const int32 SellPrice = SelectedSlotData.SellPrice;
	if (SellPrice > 0)
	{
		GoldComponent->AddGold(SellPrice);
	}

	if (!InventoryComponent->RemoveItemAt(SelectedSlotIndex, 1)) return;

	PlayerCharacter->RefreshQuickSlotByItem(SoldItemRowName);
	ClearSelectedItem();
}

void UInventoryWidget::OnRemoveClicked()
{
	if (!InventoryComponent) return;
	if (!bHasSelectedItem) return;
	if (SelectedSlotIndex == INDEX_NONE) return;

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (!PlayerCharacter) return;

	const FName RemovedItemRowName = SelectedSlotData.ItemRowName;

	if (!InventoryComponent->RemoveItemAt(SelectedSlotIndex, 1)) return;

	PlayerCharacter->RefreshQuickSlotByItem(RemovedItemRowName);
	ClearSelectedItem();
}

void UInventoryWidget::ClearSelectedItem()
{
	SelectedSlotIndex = INDEX_NONE;
	SelectedSlotData = FInventorySlotViewData();
	bHasSelectedItem = false;
	
	UpdateSelectedSlotVisual();
	ClearItemDescription();
	ResetUseButton();
}
//=======================================================================================
// 아이템 카테고리
//=======================================================================================
void UInventoryWidget::OnAllFilterClicked()
{
	bShowAllItems = true;
	CurrentFilterType = EItemType::None;

	ClearSelectedItem();
	RefreshInventory();
}

void UInventoryWidget::OnCBFilterClicked()
{
	bShowAllItems = false;
	CurrentFilterType = EItemType::Consumable;

	ClearSelectedItem();
	RefreshInventory();
}

void UInventoryWidget::OnWBFilterClicked()
{
	bShowAllItems = false;
	CurrentFilterType = EItemType::Weapon;

	ClearSelectedItem();
	RefreshInventory();
}

void UInventoryWidget::OnArmorFilterClicked()
{
	bShowAllItems = false;
	CurrentFilterType = EItemType::Armor;

	ClearSelectedItem();
	RefreshInventory();
}

