// Fill out your copyright notice in the Description page of Project Settings.


#include "ReturnStateComponent.h"

#include "EnemyBase.h"
#include "EnemyFSMControllerComponent.h"
#include "EnemyFSMTypes.h"

void UReturnStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	SetRootMotionFromMontage(false);
	if (OwnerCharacter)
	{
		OwnerCharacter->SetDefaultMoveSpeed();
		UE_LOG(LogTemp, Log, TEXT("[FSM][Return] Enter : %s"), *OwnerCharacter->GetName());
		OwnerCharacter->SetEnemyHPWidgetVisible(false);
	}
}

void UReturnStateComponent::OnStateUpdate(float DeltaTime)
{
	Super::OnStateUpdate(DeltaTime);
	if (!FSMController || !OwnerCharacter) return;
	if (IsOwnerDead())
	{
		return;
	}
	const float DistanceToHome = GetDistanceToHomeLocation();

	if (DistanceToHome <= ReChaseAllowedRadius && IsPlayerInDetectRange())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Chase);
		return;
	}

	if (IsNearHomeLocation(ReturnAcceptanceRadius))
	{
		StopMove();
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;
	}
	MoveToLocation(OwnerCharacter->GetHomeLocation(), ReturnAcceptanceRadius);
}

void UReturnStateComponent::OnStateExit()
{
	Super::OnStateExit();
	StopMove();

	if (OwnerCharacter)
	{
		UE_LOG(LogTemp, Log, TEXT("[FSM][Return] Exit : %s"), *OwnerCharacter->GetName());
	}
}
