// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonEnumTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFlowSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameFlowStateChanged, EGameFlowState, PrevState, EGameFlowState, NewState);
UCLASS()
class YESTERDAYKINGDOM_API UGameFlowSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
private:
	UPROPERTY()
	EGameFlowState CurrentState = EGameFlowState::None;
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ========================================================
	// 전반적인 게임 state 관리 
	// ========================================================
	UFUNCTION(Blueprintable, Category = "GameFlow")
	void SetGameFlowState(EGameFlowState NewState);
	
	UFUNCTION(Blueprintable, Category = "GameFlow")
	EGameFlowState GetGameFlowState() const {return CurrentState;}
	
	UPROPERTY(BlueprintAssignable, Category = "GameFlow")
	FOnGameFlowStateChanged OnGameFlowStateChanged;
};
