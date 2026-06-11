// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryTabBtnWidget.h"

#include "InventoryWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"


void UInventoryTabBtnWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_Map)
	{
		BTN_Map->OnClicked.AddDynamic(this, &UInventoryTabBtnWidget::OnMapClicked);
	}

	if (BTN_Quest)
	{
		BTN_Quest->OnClicked.AddDynamic(this, &UInventoryTabBtnWidget::OnQuestClicked);
	}

	if (BTN_Item)
	{
		BTN_Item->OnClicked.AddDynamic(this, &UInventoryTabBtnWidget::OnItemClicked);
	}

	if (BTN_Weapon)
	{
		BTN_Weapon->OnClicked.AddDynamic(this, &UInventoryTabBtnWidget::OnWeaponClicked);
	}

	if (BTN_System)
	{
		BTN_System->OnClicked.AddDynamic(this, &UInventoryTabBtnWidget::OnSystemClicked);
	}

	ChangeTab(EMenuTabType::Item);
}

void UInventoryTabBtnWidget::SetInventoryComponent(UInventoryComponent* InInventory)
{
	Wbp_InventoryWidget->BindInventory(InInventory);
	
}

void UInventoryTabBtnWidget::OnMapClicked()
{
	ChangeTab(EMenuTabType::Map);
}

void UInventoryTabBtnWidget::OnQuestClicked()
{
	ChangeTab(EMenuTabType::Quest);
}

void UInventoryTabBtnWidget::OnItemClicked()
{
	ChangeTab(EMenuTabType::Item);
}

void UInventoryTabBtnWidget::OnWeaponClicked()
{
	ChangeTab(EMenuTabType::Weapon);
}

void UInventoryTabBtnWidget::OnSystemClicked()
{
	ChangeTab(EMenuTabType::System);
}

int32 UInventoryTabBtnWidget::GetTabIndex(EMenuTabType TabType) const
{
	switch (TabType)
	{
	case EMenuTabType::Map:
		return 0;

	case EMenuTabType::Quest:
		return 1;

	case EMenuTabType::Item:
		return 2;

	case EMenuTabType::Weapon:
		return 3;

	case EMenuTabType::System:
		return 4;

	default:
		return 0;
	}
}

void UInventoryTabBtnWidget::ChangeTab(EMenuTabType TabType)
{
	if (!WS_Window) return;

	WS_Window->SetActiveWidgetIndex(GetTabIndex(TabType));
}
