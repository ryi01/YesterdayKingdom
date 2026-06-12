// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTabBtnWidget.generated.h"

class UInventoryWidget;
class UInventoryComponent;
class UWidgetSwitcher;
class UBorder;
class UImage;
class UButton;
class USizeBox;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryBackRequested);

UENUM(BlueprintType)
enum class EMenuTabType : uint8
{
	Map		UMETA(DisplayName = "Map"),
	Quest	UMETA(DisplayName = "Quest"),
	Item	UMETA(DisplayName = "Item"),
	Weapon	UMETA(DisplayName = "Weapon"),
	System	UMETA(DisplayName = "System")
};

UCLASS()
class YESTERDAYKINGDOM_API UInventoryTabBtnWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 우측 상단 Money 위젯
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_Back;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_Map;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_Quest;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_Item;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_Weapon;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> BTN_System;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USizeBox> SB_Window;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WS_Window;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UInventoryWidget> Wbp_InventoryWidget;
	UPROPERTY(BlueprintAssignable)
	FOnInventoryBackRequested OnInventoryBackRequested;
private:
	UFUNCTION()
	void OnBackClicked();
	
	UFUNCTION()
	void OnMapClicked();

	UFUNCTION()
	void OnQuestClicked();

	UFUNCTION()
	void OnItemClicked();

	UFUNCTION()
	void OnWeaponClicked();

	UFUNCTION()
	void OnSystemClicked();
	
	void ChangeTab(EMenuTabType TabType);
	int32 GetTabIndex(EMenuTabType TabType) const;

protected:
	virtual void NativeConstruct() override;
public:
	void SetInventoryComponent(UInventoryComponent* InInventory);
	
};
