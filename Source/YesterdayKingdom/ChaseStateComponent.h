// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyFSMTypes.h"
#include "FSMStateComponent.h"
#include "ChaseStateComponent.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UChaseStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Chase")
	EEnemyFSMStateType NextAttackState = EEnemyFSMStateType::Attack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Chase", meta = (ClampMin = "1.0"))
	float LoseTargetMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Chase", meta = (ClampMin = "0.0"))
	float AcceptanceRadius = 80.f;
protected:
	
public:
	// 대기 상태 초기화 처리 메소드 (대기 상태로 전이시 1회 호출)
	virtual void OnStateEnter() override;

	// 대기 상태 진행 처리 메소드 (대기 상태 전이 후 Tick과 동일하게 호출)
	virtual void OnStateUpdate(float) override;

	// 대기 상태 종료 처리 메소드 (대기 상태에서 다른상태(배회, 추적)로 전이시 1회 호출)
	virtual void OnStateExit() override;
	
	// 다음 스테이트 결정
	void SetNextAttackState(EEnemyFSMStateType InNextAttackState);

};
