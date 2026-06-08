// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestComponent.h"

#include "GoldComponent.h"
#include "InventoryComponent.h"
#include "PlayerCharacter.h"
#include "QuestDataAsset.h"
#include "QuestTypes.h"

// Sets default values for this component's properties
UQuestComponent::UQuestComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UQuestComponent::BeginPlay()
{
	Super::BeginPlay();
	
	StartQuestLine();
}

void UQuestComponent::StartQuestLine()
{
	if (!QuestDataTable || QuestOrder.Num() <= 0) return;
	
	StartQuestByIndex(0);
}

bool UQuestComponent::StartQuestByIndex(int32 QuestIndex)
{
	if (!QuestOrder.IsValidIndex(QuestIndex)) return false;
	const FName QuestRowName = QuestOrder[QuestIndex];
	
	const bool bStarted = StartQuestByRowName(QuestRowName);
	if (!bStarted) return false;
	CurrentQuestIndex = QuestIndex;
	return true;
	
}

bool UQuestComponent::StartQuestByRowName(FName QuestRowName)
{
	if (QuestRowName.IsNone()) return false;
	const FQuestDataRow* QuestDataRow = GetQuestData(QuestRowName);
	if (!QuestDataRow) return false;
	CurrentQuestInstance.QuestRowName = QuestRowName;
	CurrentQuestInstance.QuestID = QuestDataRow->QuestID;
	CurrentQuestInstance.State = EQuestState::InProgress;
	CurrentQuestInstance.CurrentCount = 0;
	CurrentQuestInstance.TargetCount = QuestDataRow->TargetCount;
	
	OnCurrentQuestChanged.Broadcast(QuestRowName, *QuestDataRow);
	OnQuestChanged.Broadcast();
	
	PrintCurrentQuestToScreen();
	
	return true;
}

bool UQuestComponent::AddProgress(EQuestObjectiveType ObjectiveType, FName TargetID, int32 Amount)
{
	if (!CurrentQuestInstance.IsValid()) return false;
	if (CurrentQuestInstance.State != EQuestState::InProgress) return false;
	
	const FQuestDataRow* QuestDataRow = GetQuestData(CurrentQuestInstance.QuestRowName);
	if (!QuestDataRow) return false;
	if (!IsObjectiveMatched(*QuestDataRow, ObjectiveType, TargetID)) return false;
	
	CurrentQuestInstance.CurrentCount = FMath::Clamp(CurrentQuestInstance.CurrentCount + Amount, 0, CurrentQuestInstance.TargetCount);
	
	UE_LOG(LogTemp, Log, TEXT("[Quest] Progress: %s / %d / %d"),
		*CurrentQuestInstance.QuestRowName.ToString(),
		CurrentQuestInstance.CurrentCount,
		CurrentQuestInstance.TargetCount
	);
	PrintCurrentQuestToScreen();
	if (CurrentQuestInstance.CurrentCount >= CurrentQuestInstance.TargetCount)
	{
		CurrentQuestInstance.State = EQuestState::ObjectiveDone;
		OnQuestChanged.Broadcast();
		
		CompleteCurrentQuest();
		return true;
	}
	OnQuestChanged.Broadcast();
	return true;
}

bool UQuestComponent::CompleteCurrentQuest()
{
	if (!CurrentQuestInstance.IsValid()) return false;
	if (CurrentQuestInstance.State != EQuestState::ObjectiveDone) return false;
	const FQuestDataRow* QuestDataRow = GetQuestData(CurrentQuestInstance.QuestRowName);
	if (!QuestDataRow) return false;
	CurrentQuestInstance.State = EQuestState::Completed;
	UE_LOG(LogTemp, Log, TEXT("[Quest] Completed: %s"),
		*CurrentQuestInstance.QuestRowName.ToString()
	);

	GiveReward(*QuestDataRow);
	
	OnQuestChanged.Broadcast();
	
	StartNextQuest();
	PrintCurrentQuestToScreen();
	return true;
}

bool UQuestComponent::StartNextQuest()
{
	const int32 NextIndex = CurrentQuestIndex + 1;
	if (!QuestOrder.IsValidIndex(NextIndex))
	{
		UE_LOG(LogTemp, Log, TEXT("[Quest] All Quest Completed"));

		CurrentQuestIndex = INDEX_NONE;
		CurrentQuestInstance.Reset();

		OnQuestChanged.Broadcast();
		return false;
	}
	
	return StartQuestByIndex(NextIndex);
}

bool UQuestComponent::GetCurrentQuestData(FQuestDataRow& OutQuestData) const
{
	if (!CurrentQuestInstance.IsValid()) return false;
	
	const FQuestDataRow* QuestDataRow = GetQuestData(CurrentQuestInstance.QuestRowName);
	if (!QuestDataRow) return false;
	
	OutQuestData = *QuestDataRow;
	return true;
}

const FQuestDataRow* UQuestComponent::GetQuestData(FName QuestRowName) const
{
	if (!QuestDataTable) return nullptr;
	return QuestDataTable->FindRow<FQuestDataRow>(QuestRowName, TEXT("QuestData"));
}

bool UQuestComponent::IsObjectiveMatched(const FQuestDataRow& QuestData, EQuestObjectiveType ObjectiveType,
	FName TargetID) const
{
	if (QuestData.ObjectiveType != ObjectiveType) return false;
	if (!QuestData.TargetID.IsNone() && QuestData.TargetID != TargetID) return false;
	return true;
}

void UQuestComponent::GiveReward(const FQuestDataRow& QuestData)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!PlayerCharacter) return;
	if (QuestData.RewardGold > 0)
	{
		if (UGoldComponent* GoldComp = PlayerCharacter->GetGoldComponent())
		{
			GoldComp->AddGold(QuestData.RewardGold);
		}
	}
	if (!QuestData.RewardItemID.IsNone() && QuestData.RewardItemCount > 0)
	{
		if (UInventoryComponent* InventoryComp = PlayerCharacter->GetInventoryComponent())
		{
			InventoryComp->AddItem(QuestData.RewardItemID, QuestData.RewardItemCount);
		}
	}
}
bool UQuestComponent::HasCurrentQuest() const
{
	return CurrentQuestInstance.IsValid();
}

FQuestInstance UQuestComponent::GetCurrentQuestInstance() const
{
	return CurrentQuestInstance;
}

void UQuestComponent::PrintCurrentQuestToScreen() const
{
	if (!GEngine) return;

	if (!CurrentQuestInstance.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Yellow,
			TEXT("[Quest] No Current Quest")
		);
		return;
	}

	const FQuestDataRow* QuestDataRow = GetQuestData(CurrentQuestInstance.QuestRowName);
	if (!QuestDataRow)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Red,
			TEXT("[Quest] Quest Data Missing")
		);
		return;
	}

	const FString Message = FString::Printf(
		TEXT("[Quest] %s | %d / %d | State: %s"),
		*CurrentQuestInstance.QuestRowName.ToString(),
		CurrentQuestInstance.CurrentCount,
		CurrentQuestInstance.TargetCount,
		*StaticEnum<EQuestState>()->GetNameStringByValue(static_cast<int64>(CurrentQuestInstance.State))
	);

	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Cyan,
		Message
	);
}
