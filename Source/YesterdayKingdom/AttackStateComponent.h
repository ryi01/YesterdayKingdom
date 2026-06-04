// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyFSMTypes.h"
#include "FSMStateComponent.h"
#include "AttackStateComponent.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UAttackStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Next")
	EEnemyFSMStateType NextState = EEnemyFSMStateType::Cooldown;
protected:
	void HandleAttackCompleted();
public:
	virtual void OnStateEnter() override;
	virtual void OnStateExit() override;
};
