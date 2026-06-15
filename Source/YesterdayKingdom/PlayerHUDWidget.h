// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UEquipmentComponent;
class UTextBlock;
class UQuickSlotWidget;
class UProgressBar;
class UVerticalBox;
class UImage;
class UHorizontalBox;
UENUM(BlueprintType)
enum class EHUDPage : uint8
{
	Main,
	Inventory,
	Store,
	Dead
};
UCLASS()
class YESTERDAYKINGDOM_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UInventoryTabBtnWidget> WBP_InventoryTab;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBossWidget> WBP_BossHP;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UStoreWidget> WBP_Store;
	
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
	TObjectPtr<UTextBlock> TB_HP;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_MP;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_ST;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_QuestDes;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> Armor1;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> Armor2;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> Armor3;
	
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
	//=====================================================================================================
	// 장비창
	//=====================================================================================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment UI")
	TObjectPtr<UTexture2D> DefaultHelmetIcon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment UI")
	TObjectPtr<UTexture2D> DefaultArmorIcon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment UI")
	TObjectPtr<UTexture2D> DefaultBootsIcon;
	//=====================================================================================================
	// 퀘스트
	//=====================================================================================================
	UPROPERTY()
	TObjectPtr<UQuestComponent> BoundQuestComponent;
private:
	void SetHUDPage(EHUDPage Page);
	//=====================================================================================================
	// 스토어
	//=====================================================================================================
	UFUNCTION()
	void RefreshQuestDescription();
protected:
	virtual void NativeConstruct() override;
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
	
	UFUNCTION()
	void HandleInventoryBackRequested();
	
	//=====================================================================================================
	// 퀵 슬롯
	//=====================================================================================================
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TArray<TObjectPtr<UQuickSlotWidget>> QuickSlots;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UQuickSlotWidget> QuickSlot1;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UQuickSlotWidget> QuickSlot2;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UQuickSlotWidget> QuickSlot3;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UQuickSlotWidget> QuickSlot4;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UQuickSlotWidget> QuickSlot5;
public:
	UFUNCTION(BlueprintCallable)
	void BindPlayer(class APlayerCharacter* InPlayer);

	UFUNCTION(BlueprintCallable)
	void BindBoss(AEnemyBase* Boss);
	void UnbindBoss();
	
	UFUNCTION(BlueprintCallable)
	void SetInventoryVisible(bool bVisible);
	
	void SetVisibleBossHPBar(bool bEnable);
	
	UFUNCTION()
	void PlayerDeadWidget();
	//=====================================================================================================
	// 퀵 슬롯
	//=====================================================================================================
	void SetQuickSlot(int32 QuickSlotIndex, const FInventorySlotViewData& SlotData);
	void UpdateQuickSlot(int32 QuickSlotIndex, FName ItemRowName);
	void ClearQuickSlot(int32 QuickSlotIndex);
	void ClearAllQuickSlots();
	//=====================================================================================================
	// 스토어
	//=====================================================================================================
	void OpenStore(UStoreComponent* InStoreComponent);
	UFUNCTION()
	void CloseStore();

	//=====================================================================================================
	// 장비창
	//=====================================================================================================
	UFUNCTION()
	void RefreshEquipmentIcons();
	void SetEquipmentIcon(UImage* ImageWidget,UEquipmentComponent* EquipmentComponent,EEquipmentSlotType SlotType, UTexture2D* DefaultIcon);
};
