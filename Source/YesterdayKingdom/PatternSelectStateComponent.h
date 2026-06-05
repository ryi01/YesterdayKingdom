// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyDefinition.h"
#include "EnemyFSMTypes.h"
#include "FSMStateComponent.h"
#include "PatternSelectStateComponent.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UPatternSelectStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|Next")
	EEnemyFSMStateType NextState = EEnemyFSMStateType::Attack;

protected:
	FName SelectBossAttackPattern() const;
	bool CanUseBossPattern(const FBossAttackPattern& Pattern, float DistanceToPlayer) const;
	int32 GetCurrentPhase() const;
	EEnemyFSMStateType GetNextStateByAttackType(FName AttackRowName) const;
public:
	virtual void OnStateEnter() override;
	
};
