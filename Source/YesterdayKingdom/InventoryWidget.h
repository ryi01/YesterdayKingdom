// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UMoneyWidget;
class UInventoryTabListWidget;
class UInventoryComponent;

UCLASS()
class YESTERDAYKINGDOM_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 게임 머니 업데이트 처리
	UFUNCTION()
	void OnMoneyUpdated(int32 TotalMoney);
	
protected:
	virtual void NativeConstruct() override;
	
	// 우측 상단 Money 위젯
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMoneyWidget> MoneyWidget;
	
	// 인벤토리 탭 / 아이템목록 위젯
/*	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UInventoryTabListWidget> InventoryTabListWidget;
	
	// 슬롯 클릭 델리게이트 이벤트 메소드
	UFUNCTION()
	void OnListSlotClicked(const FInventorySlotData& SlotData);
	
	// 인벤토리 컴포넌트
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;
	
	// 아이템 정보 디테일에서 삭제 델리게이트 발동 시 호출될 이벤트 메소드
	UFUNCTION()
	void OnDeleteBtnClicked(const FInventorySlotData& SlotData);
	
	// 아이템 정보 디테일에서 무기 장착 델리게이트 발동 시 호출될 이벤트 메소드
	UFUNCTION()
	void OnEquipBtnClicked(const FInventorySlotData& SlotData);
*/

};
