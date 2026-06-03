// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"
#include "PlayerCharacter.h"
#include "InventoryWidget.h"

void UPlayerHUDWidget::BindPlayer(class APlayerCharacter* InPlayer)
{
	OwnerPlayer = InPlayer;
	if (!OwnerPlayer) return;
	if (WBP_Inventory)
	{
		WBP_Inventory->BindInventory(OwnerPlayer->GetInventoryComponent());
		WBP_Inventory->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UPlayerHUDWidget::SetInventoryVisible(bool bVisible)
{
	const ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	FString InventoryOpen = bVisible ? TEXT("OPEN INVENTORY") :  TEXT("CLOSE INVENTORY") ;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *InventoryOpen);
	if (WBP_Inventory) WBP_Inventory->SetVisibility(NewVisibility);
}
