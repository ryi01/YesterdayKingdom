// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyFSMTypes.h"
#include "FSMStateComponent.h"
#include "AttackActionStateComponent.generated.h"

struct FAttackDataRow;
/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UAttackActionStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Next")
	EEnemyFSMStateType NextState = EEnemyFSMStateType::Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|AttackAction")
	float MaxActionTime = 3.f;
	float ElapsedTime = 0.f;
	
	FName CurrentAttackRowName = NAME_None;

	const FAttackDataRow* CurrentAttackDataRow = nullptr;
	
	bool bFinished = false;
	
	FVector ActionTargetLocation = FVector::ZeroVector;

	TWeakObjectPtr<APawn> ActionTargetPawn;

	float CurrentActionMovePower = 900.f;
	
protected:
	bool InitializeAttackActionFromData();
	void FinishAttackAction();
	void ClearAttackActionData();	
	
	bool JumpToNextAttackSection();
	bool MoveToTargetLocation(const FVector& TargetLocation, float MovePower, bool bOverrideXY = true, bool bOverrideZ = false);
	
	bool InitializeActionTarget();
	void ClearActionTargetData();

	void ApplyMovePowerFromAttackData();

	bool MoveToActionTargetLocation(bool bOverrideXY = true, bool bOverrideZ = false);
	
	void StopActionMovement();
	

public:
	virtual void OnStateExit() override;
	
	virtual void NotifyAttackActionStart();
	virtual void NotifyAttackActionEnd();
};
