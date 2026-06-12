// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "EquipmentTypes.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FEquipmentSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipmentSlotType SlotType = EEquipmentSlotType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemRowName = NAME_None;
	
	bool IsEmpty() const
	{
		return ItemRowName.IsNone();
	}
	
	void Clear()
	{
		ItemRowName = NAME_None;
	}
};

USTRUCT(BlueprintType)
struct FEquipmentSlotViewData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipmentSlotType SlotType = EEquipmentSlotType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemRowName = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Icon = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText EffectDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemRarity Rarity = EItemRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackPower = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DefensePower = 0;
	
	bool IsEmpty() const
	{
		return ItemRowName.IsNone();
	}
};
