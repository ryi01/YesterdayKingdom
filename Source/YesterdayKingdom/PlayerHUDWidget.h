// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UProgressBar;
class UVerticalBox;
class UImage;
class UHorizontalBox;

UCLASS()
class YESTERDAYKINGDOM_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UInventoryTabBtnWidget> WBP_InventoryTab;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBossWidget> WBP_BossHP;
	
	UPROPERTY()
	TObjectPtr<class APlayerCharacter> OwnerPlayer;
	
	// ===============================================================================
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> WS_HUD;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UVerticalBox> HUDBar;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UProgressBar> HP;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UProgressBar> MP;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UProgressBar> ST;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> Logo;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> Map;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> Quick_Slot;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> QuickSlot1;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> QuickSlot2;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> QuickSlot3;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> QuickSlot4;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> QuickSlot5;
	
	float TargetHPPercent = 1.f;
	float TargetSTPercent = 1.f;
	float TargetMPPercent = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	float BarInterpSpeed = 8.f;
	//=====================================================================================================
	// 보스 체력바 관련
	//=====================================================================================================
	UPROPERTY()
	TObjectPtr<class AEnemyBase> BoundBoss;

	UPROPERTY()
	TObjectPtr<class UBaseStatComponent> BoundBossStatComponent;
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//=====================================================================================================
	// 스위처
	//=====================================================================================================
	UFUNCTION()
	void SetSwitcherIndex(int32 index);
	//=====================================================================================================
	// 플레이어 체력바
	//=====================================================================================================
	UFUNCTION()
	void UpdateHP(float CurrentHP, float MaxHP);
	UFUNCTION()
	void UpdateST(float CurrentST, float MaxST);
	UFUNCTION()
	void UpdateMP(float CurrentMP, float MaxMP);
	//=====================================================================================================
	// 보스 체력바 관련
	//=====================================================================================================

	UFUNCTION()
	void HandleBossHPChanged(float CurrentHP, float MaxHP);
public:
	UFUNCTION(BlueprintCallable)
	void BindPlayer(class APlayerCharacter* InPlayer);

	UFUNCTION(BlueprintCallable)
	void BindBoss(AEnemyBase* Boss);
	void UnbindBoss();
	
	UFUNCTION(BlueprintCallable)
	void SetInventoryVisible(bool bVisible);
	
	void SetVisibleBossHPBar(bool bEnable);
	
};
