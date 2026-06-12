// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "PlayerCharacter.h"
#include "QuestComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	ItemSlots.SetNum(MaxSlotCount);
}
//===============================================================================================
// 아이템 추가
//===============================================================================================
bool UInventoryComponent::AddItem(FName ItemRowName, int32 Amount, bool bNotifyQuest)
{
	if (ItemRowName.IsNone() || Amount <= 0) return false;
	const FItemData* ItemData = GetItemData(ItemRowName);
	if (!ItemData) return false;
	
	int32 RemainAmount = Amount;
	int32 AddedAmount = 0;
	
	// 기존 슬롯에 중첩
	for (FInventorySlot& InventorySlot : ItemSlots)
	{
		if (RemainAmount <= 0) break;
		// 기존 슬롯에 동일한 아이템이 존재하고, 슬롯에 있는 개수가 maxStackCount가 아니라면
		if (InventorySlot.ItemRowName == ItemRowName && InventorySlot.Count < ItemData->MaxStackCount)
		{
			// 여유 공간을 확인하고 : maxStackCount - 슬롯에 있는 개수
			const int32 Space = ItemData->MaxStackCount - InventorySlot.Count;
			// 여유공간과 들어온 값을 비교해서 더 적은 값 추출
			const int32 AddAmount = FMath::Min(Space, RemainAmount);
			// 슬롯 개수를 증가하고
			InventorySlot.Count += AddAmount;
			// 남은 추가 개수를 줄임
			RemainAmount -= AddAmount;
			AddedAmount += AddAmount;
		}
	}
	// 남은 추가 개수가 있다면 슬롯을 돌면서 비어있는 곳을 찾음
	for (FInventorySlot& Slot : ItemSlots)
	{
		if (RemainAmount <= 0) break;
		if (Slot.IsEmpty())
		{
			// 빈슬롯에 넣을 수 있는 개수와 남은 개수를 비교해서 더 적은 값을 빈 공간에 넣고
			const int32 AddAmount = FMath::Min(ItemData->MaxStackCount, RemainAmount);
			// 빈 슬롯의 ItemRowName을 넣음
			Slot.ItemRowName = ItemRowName;
			// 남은 추가 개수를 증가 시킴
			Slot.Count = AddAmount;
			
			RemainAmount -= AddAmount;
			AddedAmount += AddAmount;
		}
	}
	
	if (AddedAmount > 0)
	{
		OnInventoryChanged.Broadcast();
		
		UE_LOG(LogTemp, Log, TEXT("[InventoryComponent::AddItem] Added %s x%d / Requested: %d"),
					*ItemRowName.ToString(),
					AddedAmount,
					Amount
				);

		if (bNotifyQuest)
		{
			NotifyQuestItemCollected(ItemRowName, AddedAmount);
		}
	}
	if (RemainAmount > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InventoryComponent::AddItem] Inventory full. Item: %s / Remain: %d"),
			*ItemRowName.ToString(),
			RemainAmount
		);
	}
	return RemainAmount <= 0;
}
//===============================================================================================
// 아이템 제거 
//===============================================================================================
bool UInventoryComponent::RemoveItem(FName ItemRowName, int32 Amount)
{
	if (ItemRowName.IsNone() || Amount <= 0) return false;
	if (!HasItem(ItemRowName, Amount)) return false;
	
	int32 RemainingAmount = Amount;
	
	for (FInventorySlot& Slot : ItemSlots)
	{
		if (Slot.ItemRowName == ItemRowName)
		{
			// 해당 슬롯에서 제거할 만큼의 갯수를 가지고 있는지 확인 
			const int32 RemoveAmount = FMath::Min(RemainingAmount, Slot.Count);
			// 슬롯에서 제거
			Slot.Count -= RemoveAmount;
			// 남은 제거 개수 감소
			RemainingAmount -= RemoveAmount;
			if (Slot.Count <= 0) Slot.Clear();
			// 남은 제거 개수가 없으면
			if (RemainingAmount <= 0)
			{
				// 성공
				OnInventoryChanged.Broadcast();

				UE_LOG(LogTemp, Log, TEXT("[InventoryComponent::RemoveItem] Removed %s x%d"), *ItemRowName.ToString(), Amount);
				return true;
			}
		}
	}
	OnInventoryChanged.Broadcast();
	return RemainingAmount <= 0;
}
//===============================================================================================
// 특정 슬롯에서 아이템을 제거 
//===============================================================================================
bool UInventoryComponent::RemoveItemAt(int32 SlotIndex, int32 Amount)
{
	if (!IsValidSlotIndex(SlotIndex) || Amount <= 0) return false;
	// 슬롯을 찾아서
	FInventorySlot& Slot = ItemSlots[SlotIndex];
	if (Slot.IsEmpty()) return false;
	// 슬롯 갯수를 줄이고
	Slot.Count -= Amount;
	
	if (Slot.Count <= 0) Slot.Clear();
	// 인벤토리갱신
	OnInventoryChanged.Broadcast();
	
	return true;
}
//===============================================================================================
// 특정 아이템을 원하는 개수 이상 가지고 있는가
//===============================================================================================
bool UInventoryComponent::HasItem(FName ItemRowName, int32 Amount) const
{
	if (ItemRowName.IsNone() || Amount <= 0) return false;
	int32 TotalCount = 0;
	
	for (const FInventorySlot& Slot : ItemSlots)
	{
		// 원하는 아이템이 있는 슬롯에 접근해
		if (Slot.ItemRowName == ItemRowName)
		{
			// TotalCount 누적
			TotalCount += Slot.Count;
			if (TotalCount >= Amount) return true;
		}
	}
	return false;
}
//===============================================================================================
// 아이템 슬롯 위치 변경
//===============================================================================================
bool UInventoryComponent::SwapSlots(int32 FromIndex, int32 ToIndex)
{
	if (!IsValidSlotIndex(FromIndex) || !IsValidSlotIndex(ToIndex)) return false;
	if (FromIndex == ToIndex) return false;
	// 아이템 데이터끼리 교환
	ItemSlots.Swap(FromIndex, ToIndex);
	OnInventoryChanged.Broadcast();
	
	return true;
	
}

const TArray<FInventorySlot>& UInventoryComponent::GetItemSlots() const
{
	return ItemSlots;
}
//===============================================================================================
// 모든 슬롯을 UI 표시용 데이터로 변환해서 반환
//===============================================================================================
TArray<FInventorySlotViewData> UInventoryComponent::GetAllSlotViewData() const
{
	TArray<FInventorySlotViewData> Result;
	
	for (int32 i = 0; i < ItemSlots.Num(); i++)
	{
		FInventorySlotViewData ViewData;
		// UI용 데이터를 만들어서
		if (MakeSlotViewData(i, ViewData))
		{
			// 추가
			Result.Add(ViewData);
		}
	}
	return Result;
}
//===============================================================================================
// 특정 타입의 아이템만 UI 용으로 반환
//===============================================================================================
TArray<FInventorySlotViewData> UInventoryComponent::GetSlotViewDataByType(EItemType ItemType) const
{
	TArray<FInventorySlotViewData> Result;
	for (int32 i = 0; i < ItemSlots.Num(); i++)
	{
		const FInventorySlot& Slot = ItemSlots[i];
		if (Slot.IsEmpty()) continue;
		const FItemData* ItemData = GetItemData(Slot.ItemRowName);
		if (!ItemData || ItemData->ItemType != ItemType) continue;
		
		FInventorySlotViewData ViewData;
		if (MakeSlotViewData(i, ViewData))
		{
			Result.Add(ViewData);
		}
	}
	return Result;
}
//===============================================================================================
// 아이템 슬롯에 있는지 확인 
//===============================================================================================
bool UInventoryComponent::IsValidSlotIndex(int32 SlotIndex) const
{
	return ItemSlots.IsValidIndex(SlotIndex);
}
//===============================================================================================
// 아이템의 원본 데이터 탐색
//===============================================================================================
const FItemData* UInventoryComponent::GetItemData(FName ItemRowName) const
{
	// DT가 존재하는지, ItemRowName이 있는지 확인
	if (!ItemDataTable) return nullptr;
	if (ItemRowName.IsNone()) return nullptr;
	// 있다면 DT에서 해당 Row를 추출
	return ItemDataTable->FindRow<FItemData>(ItemRowName, TEXT("InventoryComponent::GetItemData"));
}
//===============================================================================================
// 아이템의 퀘스트 아이템 추가 
//===============================================================================================
void UInventoryComponent::NotifyQuestItemCollected(FName ItemRowName, int32 Amount)
{
	if (ItemRowName.IsNone() || Amount <= 0) return;
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!PlayerCharacter) return;

	UQuestComponent* QuestComponent = PlayerCharacter->GetQuestComponent();
	if (!QuestComponent) return;
	
	QuestComponent->AddProgress(EQuestObjectiveType::CollectItem, ItemRowName, Amount);
}

//===============================================================================================
// 실제 아이템 데이터를 UI용으로 변환 
//===============================================================================================
bool UInventoryComponent::MakeSlotViewData(int32 SlotIndex, FInventorySlotViewData& OutViewData) const
{
	if (!IsValidSlotIndex(SlotIndex)) return false;
	const FInventorySlot& Slot = ItemSlots[SlotIndex];
	if (Slot.IsEmpty()) return false;
	const FItemData* ItemData = GetItemData(Slot.ItemRowName);
	if (!ItemData) return false;
	
	OutViewData.ItemRowName = Slot.ItemRowName;
	OutViewData.Icon = ItemData->Icon;
	OutViewData.ItemName = ItemData->ItemName;
	OutViewData.Count = Slot.Count;
	OutViewData.SlotIndex = SlotIndex;
	OutViewData.ItemDescription = ItemData->ItemDescription;
	OutViewData.EffectDescription = ItemData->EffectDescription;
	OutViewData.Rarity = ItemData->Rarity;
	OutViewData.BuyPrice = ItemData->BuyPrice;
	OutViewData.SellPrice = ItemData->SellPrice;

	return true;
}