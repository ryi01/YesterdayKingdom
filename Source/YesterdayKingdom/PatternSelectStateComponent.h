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
private:
	UPROPERTY()
	TMap<FName, float> LastPatternUsedTimeMap;
	
	bool bHasSelectedOpeningPattern = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|Next")
	EEnemyFSMStateType NextState = EEnemyFSMStateType::Attack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Pattern")
	float NoValidPatternChaseTime = 0.7f;
	
private:
	bool IsPatternOnCooldown(const FBossAttackPattern& Pattern) const;
	void MarkPatternUsed(const FBossAttackPattern& Pattern);
protected:
	int32 GetCurrentPhase() const;

public:
	virtual void OnStateEnter() override;
	
};
