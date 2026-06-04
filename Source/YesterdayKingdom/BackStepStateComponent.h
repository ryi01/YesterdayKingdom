// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyFSMTypes.h"
#include "FSMStateComponent.h"
#include "BackStepStateComponent.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UBackStepStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()
private:
	float ElapsedTime = 0.f;
	float CurrentBackStepDuration = 0.8f;
	bool bHasLanded = false;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Next")
	EEnemyFSMStateType NextState = EEnemyFSMStateType::Chase;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|BackStep")
	TObjectPtr<UAnimMontage> BackStepMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|BackStep")
	float BackStepVerticalPower = 220.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|BackStep")
	float DefaultBackStepDistance = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|BackStep")
	float DefaultBackStepDuration = 0.8f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|BackStep")
	float MaxBackStepDuration = 1.5f;``
private:
	void HandleEnemyLanded();
public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate(float DeltaTime) override;
	virtual void OnStateExit() override;
	
};
