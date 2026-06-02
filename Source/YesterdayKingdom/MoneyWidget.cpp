// Fill out your copyright notice in the Description page of Project Settings.


#include "MoneyWidget.h"
#include "Components/TextBlock.h"

void UMoneyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	// 게임 머니 표시 초기화
	if (Text_TotalMoney)
	{
		Text_TotalMoney->SetText(FText::FromString("0"));
	}

}

// 게임 머니 정보 표시 갱신
void UMoneyWidget::RefreshTotalMoney(int32 TotalMoney)
{
	if (Text_TotalMoney)
	{
		Text_TotalMoney->SetText(FText::AsNumber(TotalMoney));
	}
}

