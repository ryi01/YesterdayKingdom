// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreWidget.h"

#include "GoldComponent.h"
#include "InventoryComponent.h"
#include "ItemSlotWidget.h"
#include "StoreComponent.h"
#include "StoreDataAsset.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UStoreWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ClearSelectedItem();
}
void UStoreWidget::BindStore(class UStoreComponent* InStoreComponent, UInventoryComponent* InInventoryComponent, UGoldComponent* InGoldComponent)
{
	StoreComponent = InStoreComponent;
	InventoryComponent = InInventoryComponent;
	GoldComponent = InGoldComponent;
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(this,&UStoreWidget::RefreshInventorySlots);
		InventoryComponent->OnInventoryChanged.AddDynamic(this, &UStoreWidget::RefreshInventorySlots);
	}
	if (StoreComponent)
	{
		StoreComponent->OnStoreChanged.RemoveDynamic(this, &UStoreWidget::RefreshStoreSlots);
		StoreComponent->OnStoreChanged.AddDynamic(this, &UStoreWidget::RefreshStoreSlots);
	}
	if (GoldComponent && TB_Gold)
	{
		GoldComponent->OnGoldChanged.RemoveDynamic(this, &UStoreWidget::UpdateGold);
		GoldComponent->OnGoldChanged.AddDynamic(this, &UStoreWidget::UpdateGold);
		UpdateGold(GoldComponent->GetGold());
	}
	if (BTN_Sell)
	{
		BTN_Sell->OnClicked.RemoveDynamic(this, &UStoreWidget::OnTradeClicked);
		BTN_Sell->OnClicked.AddDynamic(this, &UStoreWidget::OnTradeClicked);
	}
	if (BTN_Back)
	{
		BTN_Back->OnClicked.RemoveDynamic(this, &UStoreWidget::OnBackClicked);
		BTN_Back->OnClicked.AddDynamic(this, &UStoreWidget::OnBackClicked);
	}
	
	RefreshStoreSlots();
	RefreshInventorySlots();
	ClearSelectedItem();
}

void UStoreWidget::SelectStoreItem(FName ItemRowName)
{
	UpdateSelectedItem(ItemRowName, EStoreSelectionType::Store);
}

void UStoreWidget::SelectInventoryItem(FName ItemRowName)
{
	UpdateSelectedItem(ItemRowName, EStoreSelectionType::Inventory);
}

void UStoreWidget::UpdateSelectedItem(FName ItemRowName, EStoreSelectionType SelectionType)
{
	if (!InventoryComponent || ItemRowName.IsNone())
	{
		ClearSelectedItem();
		return;
	}
	const FItemData* ItemData = InventoryComponent->GetItemData(ItemRowName);
	if (!ItemData)
	{
		ClearSelectedItem();
		return;
	}
	SelectedItemRowName = ItemRowName;
	SelectedItemType = SelectionType;
	
	if (I_Item)
	{
		I_Item->SetBrushFromTexture(ItemData->Icon);
		I_Item->SetVisibility(ItemData->Icon? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	if (T_ItemName)
	{
		T_ItemName->SetText(ItemData->ItemName);
	}
	if (TB_ItemDescription)
	{
		TB_ItemDescription->SetText(ItemData->ItemDescription);
	}

	if (TB_BuffDescription)
	{
		TB_BuffDescription->SetText(ItemData->EffectDescription);
	}
	const bool bStoreItem = SelectedItemType == EStoreSelectionType::Store;
	const int32 DisplayPrice = bStoreItem ? ItemData->BuyPrice : ItemData->SellPrice;
	if (TB_Price)
	{
		TB_Price->SetText(FText::AsNumber(DisplayPrice));
	}
	if (TB_Sell)
	{
		TB_Sell->SetText(bStoreItem ? FText::FromString(TEXT("구매")) : FText::FromString(TEXT("판매")));
	}
	if (BTN_Sell)
	{
		BTN_Sell->SetIsEnabled(true);
	}
}



void UStoreWidget::ClearSelectedItem()
{
	SelectedItemRowName = NAME_None;
	SelectedItemType = EStoreSelectionType::None;

	if (I_Item)
	{
		I_Item->SetBrushFromTexture(nullptr);
		I_Item->SetVisibility(ESlateVisibility::Hidden);
	}

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

	if (TB_Price)
	{
		TB_Price->SetText(FText::GetEmpty());
	}

	if (TB_Sell)
	{
		TB_Sell->SetText(FText::FromString(TEXT("구매")));
	}

	if (BTN_Sell)
	{
		BTN_Sell->SetIsEnabled(false);
	}
}

void UStoreWidget::HandleStoreItemClicked(const FInventorySlotViewData SlotViewData)
{
	if (SlotViewData.IsEmpty())
	{
		ClearSelectedItem();
		return;
	}

	SelectStoreItem(SlotViewData.ItemRowName);
}
void UStoreWidget::HandleInventoryItemClicked(const FInventorySlotViewData SlotData)
{
	if (SlotData.IsEmpty())
	{
		ClearSelectedItem();
		return;
	}

	SelectInventoryItem(SlotData.ItemRowName);
}



void UStoreWidget::RefreshStoreSlots()
{
	if (!HB_Store || !StoreComponent || !InventoryComponent) return;
	HB_Store->ClearChildren();
	const UStoreDataAsset* StoreDataAsset = StoreComponent->GetStoreData();
	if (!StoreDataAsset) return;
	
	for (int32 Index = 0; Index <StoreSlotCount; Index++)
	{
		UItemSlotWidget* ItemSlotWidget = nullptr;
		if (StoreDataAsset->StoreItems.IsValidIndex(Index))
		{
			const FStoreItemData& StoreItemData = StoreDataAsset->StoreItems[Index];
			const FItemData* ItemData = InventoryComponent->GetItemData(StoreItemData.ItemRowName);
			if (ItemData)
			{
				FInventorySlotViewData ViewData;
				ViewData.SlotIndex = Index;
				ViewData.ItemRowName = StoreItemData.ItemRowName;
				ViewData.ItemName = ItemData->ItemName;
				ViewData.ItemDescription = ItemData->ItemDescription;
				ViewData.EffectDescription = ItemData->EffectDescription;
				ViewData.Icon = ItemData->Icon;
				ViewData.BuyPrice = ItemData->BuyPrice;
				ViewData.SellPrice = ItemData->SellPrice;
				ViewData.Count = StoreItemData.bInfiniteStock ? 1 : StoreItemData.StockCount;
				ItemSlotWidget = CreateItemSlot(ViewData, true);
			}
		}
		if (!ItemSlotWidget)
		{
			ItemSlotWidget = CreateEmptyItemSlot(Index);
		}
		if (!ItemSlotWidget) continue;

		UHorizontalBoxSlot* HorizontalSlot = HB_Store->AddChildToHorizontalBox(ItemSlotWidget);

		if (HorizontalSlot)
		{
			HorizontalSlot->SetPadding(FMargin(2.f));
			HorizontalSlot->SetHorizontalAlignment(HAlign_Fill);
			HorizontalSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}
}

void UStoreWidget::RefreshInventorySlots()
{
	if (!GP_Item || !InventoryComponent) return;

	GP_Item->ClearChildren();
	const TArray<FInventorySlotViewData> SlotViewDataList = InventoryComponent->GetAllSlotViewData();

	for (int32 Index = 0;  Index < InventorySlotCount; Index++)
	{
		const FInventorySlotViewData* FoundViewData = nullptr;

		for (const FInventorySlotViewData& ViewData : SlotViewDataList)
		{
			if (ViewData.SlotIndex == Index)
			{
				FoundViewData = &ViewData;
				break;
			}
		}
		UItemSlotWidget* ItemSlotWidget = nullptr;

		if (FoundViewData)
		{
			ItemSlotWidget = CreateItemSlot(*FoundViewData, false);
		}
		else
		{
			ItemSlotWidget = CreateEmptyItemSlot(Index);
		}

		if (!ItemSlotWidget) continue;
		const int32 Row = Index / ColumnCount;
		const int32 Column = Index % ColumnCount;

		if (UGridSlot* GridSlot =GP_Item->AddChildToGrid(ItemSlotWidget, Row, Column))
		{
			GridSlot->SetPadding(FMargin(4.f));
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}
}
UItemSlotWidget* UStoreWidget::CreateItemSlot(const FInventorySlotViewData& SlotViewData, bool bStoreItem)
{
	if (!ItemSlotWidgetClass) return nullptr;
	UItemSlotWidget* ItemSlotWidget = CreateWidget<UItemSlotWidget>(GetOwningPlayer(), ItemSlotWidgetClass);
	if (!ItemSlotWidget) return nullptr;
	ItemSlotWidget->SetSlotData(SlotViewData);
	if (bStoreItem)
	{
		ItemSlotWidget->OnItemSlotClicked.RemoveDynamic(this, &UStoreWidget::HandleStoreItemClicked);
		ItemSlotWidget->OnItemSlotClicked.AddDynamic(this, &UStoreWidget::HandleStoreItemClicked);
	}
	else
	{
		ItemSlotWidget->OnItemSlotClicked.RemoveDynamic(this, &UStoreWidget::HandleInventoryItemClicked);
		ItemSlotWidget->OnItemSlotClicked.AddDynamic(this, &UStoreWidget::HandleInventoryItemClicked);
	}
	return ItemSlotWidget;
}
UItemSlotWidget* UStoreWidget::CreateEmptyItemSlot(int32 SlotIndex)
{
	if (!ItemSlotWidgetClass) return nullptr;
	UItemSlotWidget* ItemSlotWidget = CreateWidget<UItemSlotWidget>(GetOwningPlayer(), ItemSlotWidgetClass);
	if (!ItemSlotWidget) return nullptr;

	ItemSlotWidget->SetEmptySlot(SlotIndex);

	return ItemSlotWidget;
}
void UStoreWidget::OnTradeClicked()
{
	if (!StoreComponent || !InventoryComponent || !GoldComponent) return;
	if (SelectedItemRowName.IsNone()) return;

	bool bTradeSucceeded = false;

	switch (SelectedItemType)
	{
	case EStoreSelectionType::Store:
		bTradeSucceeded = StoreComponent->BuyItem(InventoryComponent,GoldComponent,SelectedItemRowName,1);
		break;

	case EStoreSelectionType::Inventory:
		bTradeSucceeded = StoreComponent->SellItem(InventoryComponent,GoldComponent,SelectedItemRowName,1);
		break;

	default:
		break;
	}

	if (!bTradeSucceeded) return;

	ClearSelectedItem();
}

void UStoreWidget::UpdateGold(int32 CurrentGold)
{
	if (TB_Gold)
	{
		TB_Gold->SetText(FText::AsNumber(CurrentGold));
	}
}

void UStoreWidget::OnBackClicked()
{
	ClearSelectedItem();
	OnStoreCloseRequested.Broadcast();
}
