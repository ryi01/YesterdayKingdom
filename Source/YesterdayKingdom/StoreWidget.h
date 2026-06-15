// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "Blueprint/UserWidget.h"
#include "StoreWidget.generated.h"

class UInventoryComponent;
class UItemSlotWidget;
struct FInventorySlotViewData;
class UButton;
class UImage;
class UTextBlock;
class UGridPanel;
class UHorizontalBox;
class UGoldComponent;
UENUM(BlueprintType)
enum class EStoreSelectionType : uint8
{
	None,
	Store,
	Inventory
};	
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStoreCloseRequested);
UCLASS()
class YESTERDAYKINGDOM_API UStoreWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<class UStoreComponent> StoreComponent;
	UPROPERTY()
	TObjectPtr<UGoldComponent> GoldComponent;
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Store|Widget",meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Store|Widget", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 ColumnCount = 7;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Store|Slot",meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 StoreSlotCount = 6;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Store|Slot",meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	int32 InventorySlotCount = 24;
	
	UPROPERTY()
	FName SelectedItemRowName = NAME_None;
	UPROPERTY()
	EStoreSelectionType SelectedItemType = EStoreSelectionType::None;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HB_Store;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UGridPanel> GP_Item;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> I_Item;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_Gold;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> T_ItemName;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_ItemDescription;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_BuffDescription;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_Sell;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_Price;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_Sell;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_Back;
public:
	UPROPERTY(BlueprintAssignable)
	FOnStoreCloseRequested OnStoreCloseRequested;
private:
	UItemSlotWidget* CreateEmptyItemSlot(int32 SlotIndex);
	
	void UpdateSelectedItem(FName ItemRowName, EStoreSelectionType SelectionType);
	UItemSlotWidget* CreateItemSlot(const FInventorySlotViewData& SlotViewData, bool bStoreItem);
	UFUNCTION()
	void HandleStoreItemClicked(const FInventorySlotViewData SlotViewData);
	UFUNCTION()
	void HandleInventoryItemClicked(const FInventorySlotViewData SlotData);
	UFUNCTION()
	void OnTradeClicked();
	UFUNCTION()
	void UpdateGold(int32 CurrentGold);
	UFUNCTION()
	void OnBackClicked();
protected:
	virtual void NativeConstruct() override;
public:
	void BindStore(class UStoreComponent* InStoreComponent, UInventoryComponent* InInventoryComponent,UGoldComponent* InGoldComponent);
	UFUNCTION()
	void RefreshStoreSlots();
	UFUNCTION()
	void RefreshInventorySlots();
	
	void SelectStoreItem(FName ItemRowName);
	void SelectInventoryItem(FName ItemRowName);
	void ClearSelectedItem();
};
