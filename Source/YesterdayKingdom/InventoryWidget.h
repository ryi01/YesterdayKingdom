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
	virtual void NativeConstruct() override;

	UPROPERTY()
	TObjectPtr<class UInventoryComponent> InventoryComponent;
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Slot")
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Slot")
	int32 ColumnCount = 6;

protected:
	UFUNCTION()
	void RefreshInventory();
public:
	UFUNCTION(BlueprintCallable)
	void BindInventory(class UInventoryComponent* InInventory);
};
