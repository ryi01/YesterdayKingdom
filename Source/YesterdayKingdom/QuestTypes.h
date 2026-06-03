// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestTypes.generated.h"

UENUM(BlueprintType)
enum class EQuestState : uint8
{
	NotStarted		UMETA(DisplayName="Not Started"),
	InProgress		UMETA(DisplayName="In Progress"),
	ObjectiveDone	UMETA(DisplayName="Objective Done"),
	Completed		UMETA(DisplayName="Completed")
};

UENUM(BlueprintType)
enum class EQuestObjectiveType : uint8
{
	None		UMETA(DisplayName="None"),
	KillEnemy	UMETA(DisplayName="Kill Enemy"),
	CollectItem	UMETA(DisplayName="Collect Item"),
	EnterArea	UMETA(DisplayName="Enter Area"),
	Interact	UMETA(DisplayName="Interact")
};

USTRUCT(BlueprintType)
struct FQuestInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName QuestRowName = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 QuestID = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQuestState State = EQuestState::NotStarted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TargetCount = 1;

	bool IsValid() const
	{
		return QuestID != INDEX_NONE;
	}

	void Reset()
	{
		QuestRowName = NAME_None;
		QuestID = INDEX_NONE;
		State = EQuestState::NotStarted;
		CurrentCount = 0;
		TargetCount = 1;
	}
};

USTRUCT(BlueprintType)
struct FQuestDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 QuestID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FText QuestTitle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	EQuestObjectiveType ObjectiveType = EQuestObjectiveType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName TargetID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 TargetCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
	int32 RewardGold = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
	FName RewardItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
	int32 RewardItemCount = 0;
};