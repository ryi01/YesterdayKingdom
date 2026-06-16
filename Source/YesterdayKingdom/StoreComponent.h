// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StoreComponent.generated.h"

class UGoldComponent;
class UInventoryComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStoreChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API UStoreComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	// 이 상점이 사용할 판매 목록 DA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store|Data")
	TObjectPtr<UStoreDataAsset> StoreData;
public:	
	// Sets default values for this component's properties
	UStoreComponent();
	UPROPERTY(BlueprintAssignable, Category = "Store")
	FOnStoreChanged OnStoreChanged;
protected:
	// 상점 아이템 인덱스 찾기
	int32 FindStoreItemIndex(FName ItemRowName) const;

	// 재고 차감
	bool ConsumeStock(FName ItemRowName, int32 Count);

public:	
	// Called when the game starts
	virtual void BeginPlay() override;
	// 상점 데이터 반환
	UFUNCTION(BlueprintCallable, Category = "Store")
	UStoreDataAsset* GetStoreData() const { return StoreData; }

	// 상점 데이터 세팅
	UFUNCTION(BlueprintCallable, Category = "Store")
	void SetStoreData(UStoreDataAsset* InStoreData);
	
	UFUNCTION(BlueprintCallable, Category = "Store")
	bool BuyItem(UInventoryComponent* BuyerInventory, UGoldComponent* BuyerGold, FName ItemRowName, int32 Count =1);
	UFUNCTION(BlueprintCallable, Category = "Store")
	bool SellItem(UInventoryComponent* SellerInventory, UGoldComponent* SellerGold, FName ItemRowName, int32 Count = 1);
	
	// 상점에 해당 아이템이 있는지 확인
	UFUNCTION(BlueprintCallable, Category = "Store")
	bool HasStoreItem(FName ItemRowName) const;

	// 재고 확인
	UFUNCTION(BlueprintCallable, Category = "Store")
	bool HasStock(FName ItemRowName, int32 Count = 1) const;
	
	UFUNCTION(BlueprintCallable, Category = "Store|Debug")
	void DebugLogStoreItems() const;

};
