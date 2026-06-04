// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "InventoryComponent.h"

void UInventoryWidget::OnMoneyUpdated(int32 TotalMoney)
{
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
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