// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "InventoryWidget.h"
#include "InventoryComponent.h"
#include "InventoryTabBtnWidget.h"
#include "InventoryItemSlotWidget.h"
#include "ItemSlotWidget.h"
#include "MoneyWidget.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "GameFramework/PlayerController.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
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

	const TArray<FInventorySlot>& InventorySlots = InventoryComponent->GetItemSlots();
	const TArray<FInventorySlotViewData> ViewDataList = InventoryComponent->GetAllSlotViewData();

	TMap<int32, FInventorySlotViewData> ViewDataMap;
	
	for (const FInventorySlotViewData& ViewData : ViewDataList)
	{
		ViewDataMap.Add(ViewData.SlotIndex, ViewData);
	}
	
	for (int32 SlotIndex = 0; SlotIndex < InventorySlots.Num(); SlotIndex++)
	{
		UItemSlotWidget* SlotWidget = CreateWidget<UItemSlotWidget>(GetOwningPlayer(), ItemSlotWidgetClass);
		if (!SlotWidget) continue;

		if (const FInventorySlotViewData* FoundViewData = ViewDataMap.Find(SlotIndex))
		{
			SlotWidget->SetSlotData(*FoundViewData);
		}
		else
		{
			SlotWidget->SetEmptySlot(SlotIndex);
		}

		const int32 Row = SlotIndex / ColumnCount;
		const int32 Column = SlotIndex % ColumnCount;

		UGridSlot* GridSlot = GP_Item->AddChildToGrid(SlotWidget, Row, Column);
		if (GridSlot)
		{
			GridSlot->SetPadding(FMargin(4.f));
		}
	}
}