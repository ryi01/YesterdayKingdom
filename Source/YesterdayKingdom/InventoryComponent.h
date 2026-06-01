// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryTypes.h"
#include "Engine/DataTable.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|Data")
	TObjectPtr<UDataTable> ItemDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	int32 MaxSlotCount = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventorySlot> ItemSlots;

public:	
	// Sets default values for this component's properties
	UInventoryComponent();
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

protected:


	// 슬롯 데이터를 UI 표시용 데이터로 변환
	bool MakeSlotViewData(int32 SlotIndex, FInventorySlotViewData& OutViewData) const;

public:	
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(FName ItemRowName, int32 Amount = 1);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FName ItemRowName, int32 Amount = 1);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemAt(int32 SlotIndex, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(FName ItemRowName, int32 Amount = 1) const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SwapSlots(int32 FromIndex, int32 ToIndex);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FInventorySlot>& GetItemSlots() const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FInventorySlotViewData> GetAllSlotViewData() const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FInventorySlotViewData> GetSlotViewDataByType(EItemType ItemType) const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool IsValidSlotIndex(int32 SlotIndex) const;
	
	// DT에서 아이템 데이터 찾기
	const FItemData* GetItemData(FName ItemRowName) const;
};
