// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyFSMTypes.h"
#include "FSMStateComponent.h"
#include "CooldownStateComponent.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UCooldownStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()
private:
	float CooldownElapsedTime = 0.f;
	float CooldownDuration = 1.f;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Next")
	EEnemyFSMStateType NextState = EEnemyFSMStateType::Chase;

private:
	EEnemyFSMStateType SelectNextStateAfterCooldown() const;
public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate(float DeltaTime) override;
	virtual void OnStateExit() override;
};
