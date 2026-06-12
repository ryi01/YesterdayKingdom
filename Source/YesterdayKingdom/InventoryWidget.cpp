// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "InventoryWidget.h"
#include "InventoryComponent.h"
#include "InventoryTabBtnWidget.h"
#include "ItemSlotWidget.h"
#include "MoneyWidget.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ClearItemDescription();
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
		SelectedSlotIndex = INDEX_NONE;
		ClearItemDescription();
	}

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
		
		SlotWidget->OnItemSlotClicked.AddDynamic(this, &UInventoryWidget::OnItemSlotClicked);
		
		if (const FInventorySlotViewData* FoundViewData = ViewDataMap.Find(SlotIndex))
		{
			SlotWidget->SetSlotData(*FoundViewData);
		}
		else
		{
			SlotWidget->SetEmptySlot(SlotIndex);
		}

		SlotWidget->SetSelected(SlotIndex == SelectedSlotIndex);
		
		const int32 Row = SlotIndex / ColumnCount;
		const int32 Column = SlotIndex % ColumnCount;

		UGridSlot* GridSlot = GP_Item->AddChildToGrid(SlotWidget, Row, Column);
		if (GridSlot)
		{
			GridSlot->SetPadding(FMargin(4.f));
		}
		SlotWidgets.Add(SlotWidget);
	}
}
//=======================================================================================
// 아이템 설명
//=======================================================================================
void UInventoryWidget::OnItemSlotClicked(FInventorySlotViewData SlotData)
{
	UE_LOG(LogTemp, Warning, TEXT("[InventoryWidget] OnItemSlotClicked / SlotIndex=%d / Item=%s / Desc=%s"),
	SlotData.SlotIndex,
	*SlotData.ItemRowName.ToString(),
	*SlotData.ItemDescription.ToString()
);
	SelectedSlotIndex = SlotData.SlotIndex;
	UpdateSelectedSlotVisual();
	SetItemDescription(SlotData);
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
// 아이템 설명
//=======================================================================================