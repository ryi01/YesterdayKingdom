// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MoneyWidget.generated.h"

class UTextBlock; // UI 텍스트 위젯

UCLASS()
class YESTERDAYKINGDOM_API UMoneyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 게임 머니 정보 표시 갱신
	UFUNCTION(BlueprintCallable, Category = "Money UI")
	void RefreshTotalMoney(int32 TotalMoney);
	
protected:
	// UI 위젯 초기화 메소드
	virtual void NativeOnInitialized() override;
	
	// 게임 머니 표시 텍스트 위젯 바인딩 속성
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Text_TotalMoney;
};
