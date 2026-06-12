// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UQuickSlotWidget::SetQuickSlotData(const FInventorySlotViewData& InSlotData)
{
	SlotData = InSlotData;
	bHasItem = !InSlotData.IsEmpty();

	if (I_QuickSlot)
	{
		I_QuickSlot->SetBrushFromTexture(InSlotData.Icon);
		I_QuickSlot->SetVisibility(bHasItem ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
	if (TB_Quick)
	{
		TB_Quick->SetText(FText::AsNumber(InSlotData.Count));
	}
}

void UQuickSlotWidget::ClearQuickSlot()
{
	SlotData = FInventorySlotViewData();
	bHasItem = false;

	if (I_QuickSlot)
	{
		I_QuickSlot->SetBrushFromTexture(nullptr);
		I_QuickSlot->SetVisibility(ESlateVisibility::Hidden);
	}
	if (TB_Quick)
	{
		TB_Quick->SetText(FText::GetEmpty());
	}
}
