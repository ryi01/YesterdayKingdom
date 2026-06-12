// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.h"
#include "Blueprint/UserWidget.h"
#include "QuickSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UQuickSlotWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> I_QuickSlot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TB_Quick;
	
	UPROPERTY()
	FInventorySlotViewData SlotData;

	UPROPERTY()
	bool bHasItem = false;	
public:
	void SetQuickSlotData(const FInventorySlotViewData& InSlotData);
	void ClearQuickSlot();


};
