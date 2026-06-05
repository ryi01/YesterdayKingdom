// Fill out your copyright notice in the Description page of Project Settings.


#include "IdleStatComponent.h"

#include "AIController.h"
#include "EnemyBase.h"
#include "EnemyDefinition.h"
#include "EnemyFSMControllerComponent.h"
#include "Kismet/GameplayStatics.h"

void UIdleStatComponent::OnStateEnter()
{
	Super::OnStateEnter();
	SetRootMotionFromMontage(false);
	CurrentStateDuration = 0.f;

	if (!OwnerCharacter) return;
	const FEnemyFSMTimeConfig& TimeConfig = OwnerCharacter->GetEnemyDefinition()->FSMTimeConfig;
	CurrentStateDuration = TimeConfig.IdleTime.GetRandomTime();
	if (AAIController* AIController = Cast<AAIController>(OwnerCharacter->GetController()))
	{
		AIController->StopMovement();
	}
	UE_LOG(LogTemp, Log, TEXT("[FSM][Idle] Enter : %s"), *OwnerCharacter->GetName());
}

void UIdleStatComponent::OnStateUpdate(float DeltaTime)
{
	Super::OnStateUpdate(DeltaTime);
	if (!OwnerCharacter || !FSMController) return;
	if (OwnerCharacter->IsDead())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Dead);
		return;
	}
	const float StateElapsedTime = FSMController->GetStateElapsedTime();
	if (StateElapsedTime < CurrentStateDuration) return;
	if (IsPlayerInDetectRange())
	{
		FSMController->ChangeState(NextIdleState);
	}
	UE_LOG(LogTemp, Warning, TEXT("[FSM][Idle] Idle Finished -> %s"),
	*UEnum::GetValueAsString(NextIdleState));

	FSMController->ChangeState(NextIdleState);
}

void UIdleStatComponent::OnStateExit()
{
	Super::OnStateExit();
	CurrentStateDuration = 0.f;
	if (OwnerCharacter)
	{
		UE_LOG(LogTemp, Log, TEXT("[FSM][Idle] Exit : %s"), *OwnerCharacter->GetName());
	}
}

void UIdleStatComponent::SetNextIdleState(EEnemyFSMStateType InNextState)
{
	NextIdleState = InNextState;
}
