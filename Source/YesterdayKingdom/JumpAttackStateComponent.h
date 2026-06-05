// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
class YESTERDAYKINGDOM_API UJumpAttackStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()
protected:
	FVector JumpTargetLocation = FVector::ZeroVector;
	
	// 점프 공격이 끝나면 이동할 상태
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM")
	EEnemyFSMStateType NextState = EEnemyFSMStateType::Cooldown;

	// 현재 점프 공격 단계
	EJumpAttackStep CurrentStep = EJumpAttackStep::None;

	// 현재 실행 중인 Attack DT Row
	FName CurrentAttackRowName = NAME_None;

	// 현재 Attack DT Row 데이터 캐싱
	const FAttackDataRow* CurrentAttackDataRow = nullptr;

	// DT에서 읽어온 점프 공격 수치
	float CurrentJumpUpPower = 700.f;
	float CurrentJumpForwardPower = 1200.f;
	float CurrentAttackTriggerDistance = 250.f;
	float CurrentAttackTriggerHeight = 150.f;
	float CurrentMaxJumpAttackTime = 3.f;

	// 진행 시간
	float ElapsedTime = 0.f;

	// 점프 이동을 이미 했는지
	bool bDidJumpToPlayer = false;

	// 공격 섹션으로 넘어갔는지
	bool bStartedAttack = false;
	
private:
	bool InitializeJumpAttackFromData();
	
	void TryStartAttackSection();
	void StartAttackSection();
	void HandleAttackCompleted();
public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate(float DeltaTime) override;
	virtual void OnStateExit() override;

	// AnimNotify 1개만 사용
	void NotifyJumpToPlayer();
	
};
