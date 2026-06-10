// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSMStateComponent.h"
#include "PhaseChangeStateComponent.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UPhaseChangeStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|PhaseChange")
	float PhaseChangeDuration = 1.5f;

	FTimerHandle PhaseChangeTimerHandle;
	
protected:
	void FinishPhaseChange();
public:
	virtual void OnStateEnter() override;
	virtual void OnStateExit() override;
};
