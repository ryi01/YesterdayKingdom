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

	const int32 SlotCount = InventoryComponent->GetItemSlots().Num();
	for (int32 SlotIndex = 0; SlotIndex < SlotCount; SlotIndex++)
	{
		UItemSlotWidget* SlotWidget = CreateWidget<UItemSlotWidget>(GetOwningPlayer(), ItemSlotWidgetClass);
		if (!SlotWidget) continue;
		const int32 Row = SlotIndex / ColumnCount;
		const int32 Column = SlotIndex % ColumnCount;

		UGridSlot* GridSlot = GP_Item->AddChildToGrid(SlotWidget, Row, Column);
		if (GridSlot)
		{
			GridSlot->SetPadding(FMargin(4.f));
		}
	}
}