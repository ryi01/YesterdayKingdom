// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackActionStateComponent.h"
#include "EnemyFSMTypes.h"
#include "FSMStateComponent.h"
#include "JumpAttackStateComponent.generated.h"

struct FAttackDataRow;
class UAnimMontage;

UENUM()
enum class EJumpAttackStep : uint8
{
	None,
	JumpStart,
	Flying,
	Attack
};
UCLASS()
class YESTERDAYKINGDOM_API UJumpAttackStateComponent : public UAttackActionStateComponent
{
	GENERATED_BODY()
protected:
	// 현재 점프 공격 단계
	EJumpAttackStep CurrentStep = EJumpAttackStep::None;

	// DT에서 읽어온 점프 공격 수치
	float CurrentJumpUpPower = 700.f;
	float CurrentAttackTriggerDistance = 250.f;
	float CurrentAttackTriggerHeight = 150.f;

	// 점프 이동을 이미 했는지
	bool bDidJumpToPlayer = false;

	// 공격 섹션으로 넘어갔는지
	bool bStartedAttack = false;
	
private:
	void TryStartAttackSection();
	void StartAttackSection();

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate(float DeltaTime) override;
	virtual void OnStateExit() override;

	virtual void NotifyAttackActionStart() override;
	
};
