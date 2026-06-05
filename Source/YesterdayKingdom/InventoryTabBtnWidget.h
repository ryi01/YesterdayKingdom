// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTabBtnWidget.generated.h"

class UBorder;
class UImage;
class UTextBlock;

UENUM(BlueprintType)
enum class EInventoryType : uint8
{
	Weapon		UMETA(Display = "Weapon"),
	Food		UMETA(Display = "Food"),
	Material	UMETA(Display = "Material"),
	Quest		UMETA(Display = "Quest")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabClicked, EInventoryType, TabType);

UCLASS()
class YESTERDAYKINGDOM_API UInventoryTabBtnWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tab")
	EInventoryType TabType = EInventoryType::Weapon;
	
	// 탭 버튼 활성화
	UFUNCTION(BlueprintCallable, Category = "Tab")
	void SetActive(bool bActive);
	
	// 현재 탭버튼의 항목 종류 반환
	EInventoryType GetTabType() const { return TabType; }
	bool IsActive() const { return bIsActive; }
	
	UPROPERTY(BlueprintAssignable, Category = "Tab")
	FOnTabClicked OnTabClicked;
	
protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UBorder> Border_Background;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UBorder> Border_ActiveLine;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> Img_Icon;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_TabName;
	
	// ====================================================================================
	
	UPROPERTY(EditAnywhere, Category = "Tab|Style")
	FLinearColor ActiveBgColor = FLinearColor(1.0f, 0.5f, 0.5f, 1.0f);
	
	UPROPERTY(EditAnywhere, Category = "Tab|Style")
	FLinearColor InActiveBgColor = FLinearColor(0.08f, 0.08f, 0.08f, 0.08f);
	
	UPROPERTY(EditAnywhere, Category = "Tab|Style")
	FLinearColor HoverBgColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	UPROPERTY(EditAnywhere, Category = "Tab|Style")
	FLinearColor ActiveLineColor = FLinearColor(1.0f, 0.5f, 0.5f, 1.0f);
	
	UPROPERTY(EditAnywhere, Category = "Tab|Style")
	FLinearColor InActiveLineColor = FLinearColor(0.08f, 0.08f, 0.08f, 0.08f);
	
	UPROPERTY(EditAnywhere, Category = "Tab|Style")
	float ActiveOpacity = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Tab|Style")
	float InActiveOpacity = 0.35;
	
	UPROPERTY(EditAnywhere, Category = "Tab|Style")
	float HoverOpacity = 0.7f;
	
	
private:
	bool bIsActive = false;
	bool bIsHovered = false;
	
	void RefreshStyle();
};
