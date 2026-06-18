// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreComponent.h"

#include "GoldComponent.h"
#include "InventoryComponent.h"
#include "ItemData.h"
#include "StoreDataAsset.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UStoreComponent::UStoreComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UStoreComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UStoreComponent::SetStoreData(UStoreDataAsset* InStoreData)
{
	StoreData = InStoreData;
	OnStoreChanged.Broadcast();
}

bool UStoreComponent::BuyItem(UInventoryComponent* BuyerInventory, UGoldComponent* BuyerGold, FName ItemRowName,
	int32 Count)
{
	if (!StoreData || !BuyerGold || !BuyerInventory) return false;
	if (ItemRowName.IsNone() || Count <= 0) return false;
	if (!HasStock(ItemRowName, Count)) return false;
	
	const FItemData* ItemData = BuyerInventory->GetItemData(ItemRowName);
	if (!ItemData) return false;
	
	const int32 TotalPrice = ItemData->BuyPrice * Count;
	if (!BuyerGold->CanAfford(TotalPrice))
	{
		UE_LOG(LogTemp, Warning, TEXT("[StoreComponent::BuyItem] Not enough gold"));
		return false;
	}
	// 인벤토리에 넣기
	if (!BuyerInventory->AddItem(ItemRowName, Count))
	{
		UE_LOG(LogTemp, Warning, TEXT("[StoreComponent::BuyItem] SpendGold failed"));
		return false;
	}
	if (!ConsumeStock(ItemRowName, Count))
	{
		BuyerInventory->RemoveItem(ItemRowName, Count);
		return false;
	}
	if (TotalPrice > 0 && !BuyerGold->SpendGold(TotalPrice))
	{
		BuyerInventory->RemoveItem(ItemRowName, Count);
		const int32 StoreItemIndex = FindStoreItemIndex(ItemRowName);
		if (StoreItemIndex != INDEX_NONE)
		{
			FStoreItemData& StoreItem = StoreData->StoreItems[StoreItemIndex];
			if (!StoreItem.bInfiniteStock) StoreItem.StockCount += Count;
		}
		return false;
	}
	if (SellBuySound) UGameplayStatics::PlaySound2D(this, SellBuySound, SellBuyVolume, 1.f, 0.f, nullptr, nullptr, true);
	OnStoreChanged.Broadcast();
	return true;
}

bool UStoreComponent::SellItem(UInventoryComponent* SellerInventory, UGoldComponent* SellerGold, FName ItemRowName,
	int32 Count)
{
	if (!StoreData || !SellerInventory || !SellerGold) return false;
	if (ItemRowName.IsNone() || Count <= 0) return false;
	
	const FItemData* ItemData = SellerInventory->GetItemData(ItemRowName);
	if (!ItemData) return false;
	
	if (!SellerInventory->HasItem(ItemRowName, Count))
	{
		UE_LOG(LogTemp, Warning, TEXT("[StoreComponent::SellItem] Not enough item"));
		return false;
	}
	
	const int32 TotalPrice = ItemData->SellPrice * Count;
	if (!SellerInventory->RemoveItem(ItemRowName, Count)) return false;
	SellerGold->AddGold(TotalPrice);
	
	if (SellBuySound) UGameplayStatics::PlaySound2D(this, SellBuySound, SellBuyVolume, 1.f, 0.f, nullptr, nullptr, true);
	OnStoreChanged.Broadcast();
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[StoreComponent::SellItem] Sold %s x%d / Price: %d"),
		*ItemRowName.ToString(),
		Count,
		TotalPrice
	);

	return true;
}

bool UStoreComponent::HasStoreItem(FName ItemRowName) const
{
	return FindStoreItemIndex(ItemRowName) != INDEX_NONE;
}

bool UStoreComponent::HasStock(FName ItemRowName, int32 Count) const
{
	if (!StoreData || ItemRowName.IsNone() || Count <= 0) return false;
	const int32 Index = FindStoreItemIndex(ItemRowName);
	if (Index == INDEX_NONE) return false;
	const FStoreItemData& StoreItemData = StoreData->StoreItems[Index];
	if (StoreItemData.bInfiniteStock) return true;

	return StoreItemData.StockCount >= Count;
}


int32 UStoreComponent::FindStoreItemIndex(FName ItemRowName) const
{
	if (!StoreData || ItemRowName.IsNone() ) return INDEX_NONE;
	for (int32 i = 0; i < StoreData->StoreItems.Num(); i++)
	{
		if (StoreData->StoreItems[i].ItemRowName == ItemRowName) return i;
	}
	return INDEX_NONE;
}

bool UStoreComponent::ConsumeStock(FName ItemRowName, int32 Count)
{
	if (!StoreData || ItemRowName.IsNone() || Count <= 0) return false;
	
	const int32 Index = FindStoreItemIndex(ItemRowName);
	if (Index == INDEX_NONE) return false;
	FStoreItemData& StoreItem = StoreData->StoreItems[Index];
	if (StoreItem.bInfiniteStock) return true;
	if (StoreItem.StockCount < Count) return false;
	
	StoreItem.StockCount -= Count;
	
	return true;
}

void UStoreComponent::DebugLogStoreItems() const
{
	if (!StoreData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StoreComponent] StoreData is null"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("=============================="));
	UE_LOG(LogTemp, Log, TEXT("[StoreComponent] StoreName : %s"), *StoreData->StoreName.ToString());
	UE_LOG(LogTemp, Log, TEXT("[StoreComponent] Item Count : %d"), StoreData->StoreItems.Num());

	for (int32 i = 0; i < StoreData->StoreItems.Num(); i++)
	{
		const FStoreItemData& StoreItem = StoreData->StoreItems[i];

		UE_LOG(
			LogTemp,
			Log,
			TEXT("[StoreItem %d] RowName: %s / Infinite: %s / Stock: %d"),
			i,
			*StoreItem.ItemRowName.ToString(),
			StoreItem.bInfiniteStock ? TEXT("true") : TEXT("false"),
			StoreItem.StockCount
		);
	}

	UE_LOG(LogTemp, Log, TEXT("=============================="));
}