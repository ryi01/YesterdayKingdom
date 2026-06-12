// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.h"
#include "Blueprint/UserWidget.h"
#include "ItemSlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSlotClicked, FInventorySlotViewData, SlotData);

UCLASS()
class YESTERDAYKINGDOM_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> BTN_ItemSlot;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> I_ItemIcon;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TB_Count;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UBorder> BD_Selected;
	
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FInventorySlotViewData SlotData;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	bool bHasItem = false;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnItemSlotClicked OnItemSlotClicked;
protected:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void HandleSlotClicked();
public:
	void SetEmptySlot(int32 InSlotIndex);
	void SetSlotData(const FInventorySlotViewData& InViewData);
	void SetSelected(bool bSelected);
	
	int32 GetSlotIndex() const;
};
