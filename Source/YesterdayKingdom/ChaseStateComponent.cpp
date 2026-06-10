// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaseStateComponent.h"

#include "EnemyBase.h"
#include "EnemyDefinition.h"
#include "EnemyFSMControllerComponent.h"
#include "EnemyFSMTypes.h"

void UChaseStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	ChaseElapsedTime = 0.f;

	StopMove();
	SetRootMotionFromMontage(false);

	if (OwnerCharacter)
	{
		OwnerCharacter->SetCombatMoveSpeed();
		UE_LOG(LogTemp, Log, TEXT("[FSM][Chase] Enter : %s"), *OwnerCharacter->GetName());
	}
}

void UChaseStateComponent::OnStateUpdate(float X)
{
	Super::OnStateUpdate(X);
	if (!FSMController) return;
	if (IsOwnerDead())
	{
		return;
	}
	
	if (!IsPlayerValid())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Return);
		return;
	}
	ChaseElapsedTime += X;

	if (ChaseElapsedTime < ChaseStartDelay)
	{
		return;
	}
	
	if (EnemyDefinition && EnemyDefinition->EnemyRole != EEnemyRole::Boss)
	{
		if (IsTooFarFromHome(EnemyDefinition->ReturnRadius) || IsPlayerLost(LoseTargetMultiplier))
		{
			FSMController->ChangeState(EEnemyFSMStateType::Return);
			return;
		}
	}
	const float DistanceToPlayer = GetDistance2DToPlayer();
	if (EnemyDefinition && EnemyDefinition->EnemyRole == EEnemyRole::Boss)
	{
		const bool bCanSelectPattern = OwnerCharacter && !OwnerCharacter->IsPatternSelectBlocked();

		if (bCanSelectPattern && DistanceToPlayer <= EnemyDefinition->PatternSelectRange)
		{
			StopMove();
			FSMController->ChangeState(EEnemyFSMStateType::PatternSelect);
			return;
		}
	}
	else
	{
		if (IsPlayerInAttackRange())
		{
			StopMove();
			FSMController->ChangeState(NextAttackState);
			return;
		}
	}
	MoveToPlayer(AcceptanceRadius);
	
}

void UChaseStateComponent::OnStateExit()
{
	Super::OnStateExit();
	
	StopMove();

	if (OwnerCharacter)
	{
		UE_LOG(LogTemp, Log, TEXT("[FSM][Chase] Exit : %s"), *OwnerCharacter->GetName());
	}
}

void UChaseStateComponent::SetNextAttackState(EEnemyFSMStateType InNextAttackState)
{
	NextAttackState = InNextAttackState;
}
