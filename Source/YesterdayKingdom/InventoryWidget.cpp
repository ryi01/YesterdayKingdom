// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "InventoryWidget.h"
#include "InventoryComponent.h"
#include "InventoryTabBtnWidget.h"
#include "InventoryItemSlotWidget.h"
#include "MoneyWidget.h"
#include "GameFramework/PlayerController.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventoryWidget::OnMoneyUpdated(int32 TotalMoney)
{
	if (MoneyWidget)
	{
		MoneyWidget->RefreshTotalMoney(TotalMoney);
	}
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

}