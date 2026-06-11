// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "InventoryTypes.generated.h"

//===============================================================================================
// 인벤토리에서 사용하는 item 구조체 => DT의 모든 정보는 무겁기에 라이트하게 변경
//===============================================================================================
USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemRowName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;
	
	bool IsEmpty() const
	{
		return ItemRowName.IsNone() || Count <= 0;
	}
	
	void Clear()
	{
		ItemRowName = NAME_None;
		Count = 0;
	}
};
//===============================================================================================
// UI 전용 구조체
//===============================================================================================
USTRUCT(BlueprintType)
struct FInventorySlotViewData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemRowName = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UTexture2D> Icon = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SlotIndex = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText EffectDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemRarity Rarity = EItemRarity::Common;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BuyPrice = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SellPrice = 0;
	
	bool IsEmpty() const
	{
		return ItemRowName.IsNone() || Count <=0;
	}
};