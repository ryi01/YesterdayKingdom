// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSMStateComponent.h"
#include "EnemyFSMTypes.h"
#include "IdleStatComponent.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UIdleStatComponent : public UFSMStateComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM|Time")
	float CurrentStateDuration = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Idle")
	EEnemyFSMStateType NextIdleState = EEnemyFSMStateType::Patrol;
public:
	// 대기 상태 초기화 처리 메소드 (대기 상태로 전이시 1회 호출)
	virtual void OnStateEnter() override;

	// 대기 상태 진행 처리 메소드 (대기 상태 전이 후 Tick과 동일하게 호출)
	virtual void OnStateUpdate(float) override;

	// 대기 상태 종료 처리 메소드 (대기 상태에서 다른상태(배회, 추적)로 전이시 1회 호출)
	virtual void OnStateExit() override;
	
	void SetNextIdleState(EEnemyFSMStateType InNextState);
};
