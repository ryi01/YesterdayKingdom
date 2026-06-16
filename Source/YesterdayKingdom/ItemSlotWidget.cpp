// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlotWidget.h"

#include "InventoryTypes.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
void UItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_ItemSlot)
	{
		BTN_ItemSlot->OnClicked.RemoveDynamic(this, &UItemSlotWidget::HandleSlotClicked);
		BTN_ItemSlot->OnClicked.AddDynamic(this, &UItemSlotWidget::HandleSlotClicked);
	}
	SetSelected(false);
}


void UItemSlotWidget::SetEmptySlot(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
	bHasItem = false;
	SlotData = FInventorySlotViewData();
	SetSelected(false);
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
	SlotData = InViewData;
	SlotIndex = InViewData.SlotIndex;
	bHasItem = true;
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

void UItemSlotWidget::SetSelected(bool bSelected)
{
	if (!BD_Selected) return;
	BD_Selected->SetVisibility(bSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

int32 UItemSlotWidget::GetSlotIndex() const
{
	return SlotIndex;
}

void UItemSlotWidget::HandleSlotClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("[ItemSlotWidget] Clicked / SlotIndex=%d / bHasItem=%d"),
	SlotIndex,
	bHasItem
);

	if (!bHasItem) return;
	UE_LOG(LogTemp, Warning, TEXT("[ItemSlotWidget] Broadcast / Item=%s / Desc=%s"),
		*SlotData.ItemRowName.ToString(),
		*SlotData.ItemDescription.ToString()
	);

	OnItemSlotClicked.Broadcast(SlotData);
}
