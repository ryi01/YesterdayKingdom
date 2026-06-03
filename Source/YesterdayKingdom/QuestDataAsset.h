// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestTypes.h"
#include "Engine/DataAsset.h"
#include "QuestDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UQuestDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest")
	int32 QuestID = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest")
	FText QuestTitle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest")
	EQuestObjectiveType ObjectiveType = EQuestObjectiveType::None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest")
	FName TargetID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Quest")
	int32 TargetCount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Reward")
	int32 RewardGold = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Reward")
	FName RewardItemID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Reward")
	int32 RewardItemCount = 1;
};
