// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestTypes.h"
#include "QuestComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuestChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentQuestChanged, FName, QuestRowName, const FQuestDataRow&, QuestData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, FName, QuestRowName);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API UQuestComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UDataTable> QuestDataTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	TArray<FName> QuestOrder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 CurrentQuestIndex = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	FQuestInstance CurrentQuestInstance;
	
public:	
	// Sets default values for this component's properties
	UQuestComponent();
	
	UPROPERTY(BlueprintAssignable)
	FOnQuestChanged OnQuestChanged;

	UPROPERTY(BlueprintAssignable)
	FOnCurrentQuestChanged OnCurrentQuestChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestCompleted OnQuestCompleted;
private:
    void PrintCurrentQuestToScreen() const;
protected:
	const FQuestDataRow* GetQuestData(FName QuestRowName) const;

	bool IsObjectiveMatched(const FQuestDataRow& QuestData, EQuestObjectiveType ObjectiveType, FName TargetID) const;

	void GiveReward(const FQuestDataRow& QuestData);

public:	
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void StartQuestLine();

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool StartQuestByIndex(int32 QuestIndex);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool StartQuestByRowName(FName QuestRowName);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool AddProgress(EQuestObjectiveType ObjectiveType, FName TargetID, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool CompleteCurrentQuest();

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool StartNextQuest();

	UFUNCTION(BlueprintPure, Category = "Quest")
	bool HasCurrentQuest() const;

	UFUNCTION(BlueprintPure, Category = "Quest")
	FQuestInstance GetCurrentQuestInstance() const;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool GetCurrentQuestData(FQuestDataRow& OutQuestData) const;
};
