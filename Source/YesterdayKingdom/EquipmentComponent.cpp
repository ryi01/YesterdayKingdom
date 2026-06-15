// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"

#include "BaseStatComponent.h"
#include "InventoryComponent.h"
#include "PlayerCharacter.h"
#include "YesterdayKingdomGameInstance.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (OwnerCharacter)
	{	
		InventoryComponent = OwnerCharacter->GetInventoryComponent();
		StatComponent = OwnerCharacter->GetStatComponent();
	}
	InitializeSlots();
}

void UEquipmentComponent::InitializeSlots()
{
	EquippedSlots.Empty();
	
	FEquipmentSlot WeaponSlot;
	WeaponSlot.SlotType = EEquipmentSlotType::Weapon;
	EquippedSlots.Add(EEquipmentSlotType::Weapon, WeaponSlot);
	
	FEquipmentSlot HelmetSlot;
	HelmetSlot.SlotType = EEquipmentSlotType::Helmet;
	EquippedSlots.Add(EEquipmentSlotType::Helmet, HelmetSlot);

	FEquipmentSlot ArmorSlot;
	ArmorSlot.SlotType = EEquipmentSlotType::Armor;
	EquippedSlots.Add(EEquipmentSlotType::Armor, ArmorSlot);

	FEquipmentSlot GlovesSlot;
	GlovesSlot.SlotType = EEquipmentSlotType::Gloves;
	EquippedSlots.Add(EEquipmentSlotType::Gloves, GlovesSlot);

	FEquipmentSlot BootsSlot;
	BootsSlot.SlotType = EEquipmentSlotType::Boots;
	EquippedSlots.Add(EEquipmentSlotType::Boots, BootsSlot);

	FEquipmentSlot AccessorySlot;
	AccessorySlot.SlotType = EEquipmentSlotType::Accessory;
	EquippedSlots.Add(EEquipmentSlotType::Accessory, AccessorySlot);
	
}

bool UEquipmentComponent::EquipItem(FName ItemRowName)
{
	if (!InventoryComponent || ItemRowName.IsNone())return false;
	const FItemData* ItemData = GetItemData(ItemRowName);
	if (!CanEquipItem(ItemData)) 
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipmentComponent::EquipItem] Cannot equip item: %s"), *ItemRowName.ToString());
		return false;
	}
	const EEquipmentSlotType SlotType = ItemData->EquipmentSlotType;
	FEquipmentSlot* EquipmentSlot = EquippedSlots.Find(SlotType);
	if (!EquipmentSlot) 
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipmentComponent::EquipItem] Invalid slot type"));
		return false;
	}

	const FName PreviousItemRowName = EquipmentSlot->ItemRowName;
	// 먼저 새 장비를 인벤토리에서 제거
	if (!InventoryComponent->RemoveItem(ItemRowName, 1)) return false;
	
	// 기존 장비가 있으면 인벤토리로 돌려보냄
	if (!PreviousItemRowName.IsNone())
	{
		if (!InventoryComponent->AddItem(PreviousItemRowName, 1))
		{
			InventoryComponent->AddItem(ItemRowName, 1);
			UE_LOG(LogTemp, Warning, TEXT("[EquipmentComponent::EquipItem] Failed to return previous item"));
			return false;
		}
	}
	EquipmentSlot->ItemRowName = ItemRowName;
	RefreshEquipmentStats();
	OnEquipmentChanged.Broadcast();
	
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[EquipmentComponent::EquipItem] Equipped %s to slot %d"),
		*ItemRowName.ToString(),
		static_cast<uint8>(SlotType)
	);

	return true;

}

bool UEquipmentComponent::UnequipItem(EEquipmentSlotType SlotType)
{
	if (!InventoryComponent) return false;
	FEquipmentSlot* EquipSlot = EquippedSlots.Find(SlotType);
	if (!EquipSlot || EquipSlot->IsEmpty()) return false;
	
	const FName UnequipItemRowName = EquipSlot->ItemRowName;
	if (!InventoryComponent->AddItem(UnequipItemRowName, 1))
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipmentComponent::UnequipItem] Inventory full"));
		return false;
	}
	EquipSlot->Clear();
	RefreshEquipmentStats();
	OnEquipmentChanged.Broadcast();
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[EquipmentComponent::UnequipItem] Unequipped %s"),
		*UnequipItemRowName.ToString()
	);

	return true;
}

bool UEquipmentComponent::IsEquipped(EEquipmentSlotType SlotType) const
{
	const FEquipmentSlot* EquipmentSlot = EquippedSlots.Find(SlotType);
	return EquipmentSlot && !EquipmentSlot->IsEmpty();
}

FName UEquipmentComponent::GetEquippedItem(EEquipmentSlotType SlotType) const
{
	const FEquipmentSlot* EquipmentSlot = EquippedSlots.Find(SlotType);
	if (!EquipmentSlot) return NAME_None;
	return EquipmentSlot->ItemRowName;
}

TArray<FEquipmentSlotViewData> UEquipmentComponent::GetEquipmentViewData() const
{
	TArray<FEquipmentSlotViewData> Result;
	
	for (const TPair<EEquipmentSlotType, FEquipmentSlot>& Pair : EquippedSlots)
	{
		FEquipmentSlotViewData ViewData;
		ViewData.SlotType = Pair.Key;
		
		if (MakeEquipmentViewData(Pair.Key, ViewData))
		{
			Result.Add(ViewData);
		}
		else Result.Add(ViewData);
	}
	return Result;
}

const FItemData* UEquipmentComponent::GetItemData(FName ItemRowName) const
{
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[EquipmentComponent] InventoryComponent 없음"));
		return nullptr;
	}
	return InventoryComponent->GetItemData(ItemRowName);
	
}

bool UEquipmentComponent::CanEquipItem(const FItemData* ItemData) const
{
	if (!ItemData) return false;
	if (ItemData->ItemType != EItemType::Weapon && ItemData->ItemType != EItemType::Armor ||(ItemData->EquipmentSlotType == EEquipmentSlotType::None)) return false;
	return true;
}

bool UEquipmentComponent::MakeEquipmentViewData(EEquipmentSlotType SlotType, FEquipmentSlotViewData& OutViewData) const
{
	const FEquipmentSlot* EquipmentSlot = EquippedSlots.Find(SlotType);
	if (!EquipmentSlot) return false;
	OutViewData.SlotType = SlotType;
	if (EquipmentSlot->IsEmpty()) return false;
	const FItemData* ItemData = GetItemData(EquipmentSlot->ItemRowName);
	if (!ItemData) return false;
	
	OutViewData.ItemRowName = EquipmentSlot->ItemRowName;
	OutViewData.Icon = ItemData->Icon;
	OutViewData.ItemName = ItemData->ItemName;
	OutViewData.ItemDescription = ItemData->ItemDescription;
	OutViewData.EffectDescription = ItemData->EffectDescription;
	OutViewData.Rarity = ItemData->Rarity;
	OutViewData.AttackPower = ItemData->AttackPower;
	OutViewData.DefensePower = ItemData->DefensePower;

	return true;
}

void UEquipmentComponent::RefreshEquipmentStats()
{
	int32 TotalAttackBonus = 0;
	int32 TotalDefenseBonus = 0;
	for (const TPair<EEquipmentSlotType, FEquipmentSlot>& Pair : EquippedSlots)
	{
		const FEquipmentSlot& EquipSlot = Pair.Value;
		if (EquipSlot.IsEmpty()) continue;
		const FItemData* ItemData = GetItemData(EquipSlot.ItemRowName);
		if (!ItemData) continue;
		TotalAttackBonus += ItemData->AttackPower;
		TotalDefenseBonus += ItemData->DefensePower;
	}
	
	if (StatComponent)
	{
		StatComponent->SetEquipmentBonus(TotalAttackBonus, TotalDefenseBonus);
	}
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[EquipmentComponent] Equipment Bonus / Attack: %d / Defense: %d"),
		TotalAttackBonus,
		TotalDefenseBonus
	);
}
void UEquipmentComponent::SaveEquipmentData()
{
	UYesterdayKingdomGameInstance* GameInstance = GetWorld()->GetGameInstance<UYesterdayKingdomGameInstance>();
	if (!GameInstance) return;

	const int32 PlayerId = GameInstance->GetCurrentPlayerId();
	if (PlayerId <= 0) return;

	TArray<FEquipmentSaveData> SaveDataList;
	for (const TPair<EEquipmentSlotType, FEquipmentSlot>& Pair : EquippedSlots)
	{
		const FEquipmentSlot& EquipmentSlot = Pair.Value;
		if (EquipmentSlot.IsEmpty()) continue;
		
		FEquipmentSaveData SaveData;
		SaveData.EquipmentSlot = Pair.Key;
		SaveData.ItemRowName = EquipmentSlot.ItemRowName;
		SaveDataList.Add(SaveData);
	}
	
	GameInstance->SaveEquipmentData(PlayerId, SaveDataList);

}

void UEquipmentComponent::LoadEquipmentData()
{
	UYesterdayKingdomGameInstance* GameInstance = GetWorld()->GetGameInstance<UYesterdayKingdomGameInstance>();
	if (!GameInstance) return;

	const int32 PlayerId = GameInstance->GetCurrentPlayerId();
	if (PlayerId <= 0) return;
	
	TArray<FEquipmentSaveData>LoadDatas;
	if (!GameInstance->LoadEquipmentData(PlayerId, LoadDatas))
	{
		return;
	}
	
	InitializeSlots();
	
	for (const FEquipmentSaveData& LoadData : LoadDatas)
	{
		if (!LoadData.IsValid()) continue;
		FEquipmentSlot* EquipmentSlot = EquippedSlots.Find(LoadData.EquipmentSlot);
		if (!EquipmentSlot->IsEmpty())
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("[EquipmentComponent] 존재하지 않는 장비 슬롯 / Slot=%d"),
				static_cast<uint8>(LoadData.EquipmentSlot)
			);

			continue;
		}
		const FItemData* ItemData = GetItemData(LoadData.ItemRowName);
	if (!ItemData)continue;

		if (ItemData->EquipmentSlotType != LoadData.EquipmentSlot) continue;
		
		EquipmentSlot->ItemRowName = LoadData.ItemRowName;
	}
	
	RefreshEquipmentStats();
	OnEquipmentChanged.Broadcast();
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[EquipmentComponent] 장비 로드 완료 / PlayerId=%d / Count=%d"),
		PlayerId,
		LoadDatas.Num()
	);
}