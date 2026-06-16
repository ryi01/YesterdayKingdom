// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyFSMTypes.h"
#include "FSMStateComponent.h"
#include "HitStateComponent.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UHitStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()
	
protected:
	bool bWaitingHitMontage = false;
	
	FTimerHandle HitLockTimerHandle;
	FOnMontageEnded HitMontageEndedDelegate;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage|Reaction")
	float HitReactionLockTime = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Idle")
	EEnemyFSMStateType NextState = EEnemyFSMStateType::Cooldown;
	
protected:
	void HandleHitMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void DecideNextState();

public:
	virtual void OnStateEnter() override;
	virtual void OnStateExit() override;
	void FinishHitState();
};
