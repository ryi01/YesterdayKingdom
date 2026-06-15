// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentTypes.h"
#include "EquipmentComponent.generated.h"

class APlayerCharacter;
struct FItemData;
class UBaseStatComponent;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<APlayerCharacter> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;
	UPROPERTY()
	TObjectPtr<UBaseStatComponent> StatComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipmentSlotType, FEquipmentSlot> EquippedSlots;
public:	
	// Sets default values for this component's properties
	UEquipmentComponent();
	
	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnEquipmentChanged OnEquipmentChanged;
protected:
	void InitializeSlots();
	
	bool CanEquipItem(const FItemData* ItemData) const;
	
	bool MakeEquipmentViewData(EEquipmentSlotType SlotType, FEquipmentSlotViewData& OutViewData) const;

	void RefreshEquipmentStats();
public:	
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool EquipItem(FName ItemRowName);
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool UnequipItem(EEquipmentSlotType SlotType);
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool IsEquipped(EEquipmentSlotType SlotType) const;
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FName GetEquippedItem(EEquipmentSlotType SlotType) const;
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	TArray<FEquipmentSlotViewData> GetEquipmentViewData() const;
	UFUNCTION(BlueprintCallable, Category = "Equipment|Save")
	void SaveEquipmentData();
	UFUNCTION(BlueprintCallable, Category = "Equipment|Save")
	void LoadEquipmentData();
	const FItemData* GetItemData(FName ItemRowName) const;
};
