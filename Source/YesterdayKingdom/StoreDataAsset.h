// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StoreDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FStoreItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store")
	FName ItemRowName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store")
	bool bInfiniteStock = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store", meta = (ClampMin = "0"))
	int32 StockCount = 1;
	
};

UCLASS(BlueprintType)
class YESTERDAYKINGDOM_API UStoreDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store")
	FText StoreName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store")
	TArray<FStoreItemData> StoreItems;
	
};
