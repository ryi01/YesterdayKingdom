// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemSlotWidget.h"
#include "InventoryWidget.generated.h"

class UGridPanel;
class USizeBox;
class UButton;
class UMoneyWidget;
class UInventoryTabListWidget;
class UInventoryComponent;

UCLASS()
class YESTERDAYKINGDOM_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
protected:
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Slot")
	int32 ColumnCount = 6;
	UPROPERTY()
	TObjectPtr<class UInventoryComponent> InventoryComponent;
	//=======================================================================================
	// 아이템 카테고리 버튼
	//=======================================================================================
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_All;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_HP;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_MP;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_ST;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UGridPanel> GP_Item;
	//=======================================================================================
	// 아이템 사용 및 설명
	//=======================================================================================
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_Use;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_Sell;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_Remove;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> T_ItemName;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_ItemDescription;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_BuffDescription;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> I_Item;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Slot")
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;

	UPROPERTY()
	TArray<TObjectPtr<UItemSlotWidget>> SlotWidgets;
	
	UPROPERTY()
	int32 SelectedSlotIndex = INDEX_NONE;

protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void RefreshInventory();

	UFUNCTION()
	void OnItemSlotClicked(FInventorySlotViewData SlotData);

	void SetItemDescription(const FInventorySlotViewData& SlotData);
	void UpdateSelectedSlotVisual();

public:
	UFUNCTION(BlueprintCallable)
	void BindInventory(class UInventoryComponent* InInventory);
	UFUNCTION(BlueprintCallable)
	void ClearItemDescription();
};
