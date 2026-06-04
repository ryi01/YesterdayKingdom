// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryTabBtnWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventoryTabBtnWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UInventoryTabBtnWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	if (!bIsActive)
	{
		bIsHovered = true;
		RefreshStyle();
	}
}

void UInventoryTabBtnWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	bIsHovered = false;
	RefreshStyle();
}

FReply UInventoryTabBtnWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		OnTabClicked.Broadcast(TabType);
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

void UInventoryTabBtnWidget::RefreshStyle()
{
	if (Border_Background)
	{
		FLinearColor BgColor = bIsActive ? ActiveBgColor : bIsHovered ? HoverBgColor : InActiveBgColor;
		Border_Background->SetBrushColor(BgColor);
	}
	
	if (Border_ActiveLine)
	{
		FLinearColor LineColor = bIsActive ? ActiveLineColor : InActiveLineColor;
		Border_ActiveLine->SetBrushColor(LineColor);
	}
	
	if (Img_Icon)
	{
		float Opacity = bIsActive ? ActiveOpacity : bIsHovered ? HoverOpacity : InActiveOpacity;
		Img_Icon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, Opacity));
	}
	
	if (Text_TabName)
	{
		float Opacity = bIsActive ? ActiveOpacity : bIsHovered ? HoverOpacity : InActiveOpacity;
		Text_TabName->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, Opacity));
	}
}

void UInventoryTabBtnWidget::SetActive(bool bActive)
{
	bIsActive = bActive;
	bIsHovered = false;
	RefreshStyle();
}
