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
	if (BTN_Back)
	{
		BTN_Back->OnClicked.AddDynamic(this, &UInventoryTabBtnWidget::OnBackClicked);
	}

	if (BTN_Item)
	{
		BTN_Item->OnClicked.AddDynamic(this, &UInventoryTabBtnWidget::OnItemClicked);
	}

	if (BTN_SkillTree)
	{
		BTN_SkillTree->OnClicked.AddDynamic(this, &UInventoryTabBtnWidget::OnSkillClicked);
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

void UInventoryTabBtnWidget::OnBackClicked()
{
	if (Wbp_InventoryWidget)
	{
		Wbp_InventoryWidget->ClearItemDescription();
	}

	OnInventoryBackRequested.Broadcast();
	
}

void UInventoryTabBtnWidget::OnItemClicked()
{
	ChangeTab(EMenuTabType::Item);
}

void UInventoryTabBtnWidget::OnSkillClicked()
{
	ChangeTab(EMenuTabType::Skill);
}

void UInventoryTabBtnWidget::OnSystemClicked()
{
	ChangeTab(EMenuTabType::System);
}

int32 UInventoryTabBtnWidget::GetTabIndex(EMenuTabType TabType) const
{
	switch (TabType)
	{
	case EMenuTabType::Item:
		return 0;

	case EMenuTabType::Skill:
		return 1;

	case EMenuTabType::System:
		return 2;

	default:
		return 0;
	}
}

void UInventoryTabBtnWidget::ChangeTab(EMenuTabType TabType)
{
	if (!WS_Window) return;

	WS_Window->SetActiveWidgetIndex(GetTabIndex(TabType));
}
