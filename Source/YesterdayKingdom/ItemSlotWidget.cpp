// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlotWidget.h"

#include "InventoryTypes.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemSlotWidget::SetEmptySlot(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;

	if (I_ItemIcon)
	{
		I_ItemIcon->SetBrushFromTexture(nullptr);
		I_ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	}

	if (TB_Count)
	{
		TB_Count->SetText(FText::GetEmpty());
		TB_Count->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UItemSlotWidget::SetSlotData(const FInventorySlotViewData& InViewData)
{
	SlotIndex = InViewData.SlotIndex;
	if (I_ItemIcon)
	{
		I_ItemIcon->SetBrushFromTexture(InViewData.Icon);
		I_ItemIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	if (TB_Count)
	{
		if (InViewData.Count > 1)
		{
			TB_Count->SetText(FText::AsNumber(InViewData.Count));
			TB_Count->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			TB_Count->SetText(FText::GetEmpty());
			TB_Count->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
