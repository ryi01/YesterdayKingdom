// Fill out your copyright notice in the Description page of Project Settings.


#include "FlankingStateComponent.h"
#include "EnemyBase.h"
#include "EnemyDefinition.h"
#include "EnemyFSMControllerComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"


void UFlankingStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	SetRootMotionFromMontage(false);
	ElapsedTime = 0.f;
	MoveTargetLocation  = FVector::ZeroVector;
	CurrentMaxFlankingDuration = MaxFlankingDuration;
	if (!OwnerCharacter || !FSMController) return;
	if (EnemyDefinition)
	{
		CurrentMaxFlankingDuration = EnemyDefinition->FSMActionConfig.FlankingDuration;
	}

	OwnerCharacter->SetMoveSpeed(FlankingMoveSpeed);
	SetFocusToPlayer();
	
	RunFlankingEQS();
	
	UE_LOG(LogTemp, Warning, TEXT("[FSM][Flanking] Enter : %s"),
		*OwnerCharacter->GetName());
}

void UFlankingStateComponent::OnStateUpdate(float DeltaTime)
{
	Super::OnStateUpdate(DeltaTime);
	if (!FSMController) return;
	if (IsOwnerDead())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Dead);
		return;
	}
	if (!IsPlayerValid())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;
	}
	ElapsedTime += DeltaTime;
	if (ElapsedTime < MinFlankingDuration) return;
	
	const bool bHasTarget = !MoveTargetLocation.IsNearlyZero();
	const bool bReachedTarget = bHasTarget && FVector::Dist2D(OwnerCharacter->GetActorLocation(), MoveTargetLocation) <= AcceptanceRadius;
	
	const bool bTimeOver = ElapsedTime >= MaxFlankingDuration;
	if (bReachedTarget || bTimeOver) FSMController->ChangeState(NextState);
}

void UFlankingStateComponent::OnStateExit()
{
	Super::OnStateExit();
	StopMove();

	ClearFocusTarget();
	
	ElapsedTime = 0.f;
	MoveTargetLocation = FVector::ZeroVector;

	UE_LOG(LogTemp, Log, TEXT("[FSM][Flanking] Exit : %s"),
		OwnerCharacter ? *OwnerCharacter->GetName() : TEXT("None"));
	
}
void UFlankingStateComponent::RunFlankingEQS()
{
	if (!FlankingEQS)
	{
		FSMController->ChangeState(EEnemyFSMStateType::Chase);
		return;
	}
	
	FEnvQueryRequest QueryRequest(FlankingEQS, OwnerCharacter);
	
	QueryRequest.SetFloatParam(TEXT("MinFlankingDistance"), 300.f);
	QueryRequest.SetFloatParam(TEXT("MaxFlankingDistance"), EnemyDefinition->FSMActionConfig.FlankingRadius);
	
	QueryRequest.Execute(EEnvQueryRunMode::SingleResult, this, &UFlankingStateComponent::HandleFlankingEQSFinished);
}

void UFlankingStateComponent::HandleFlankingEQSFinished(TSharedPtr<FEnvQueryResult> Result)
{
	if (!Result.IsValid() || Result->IsAborted())
	{
		UE_LOG(LogTemp, Warning, TEXT("[FSM][Flanking] EQS Failed or Aborted : %s"),
			*OwnerCharacter->GetName());

		FSMController->ChangeState(EEnemyFSMStateType::Chase);
		return;
	}
	if (Result->Items.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FSM][Flanking] EQS No Result : %s"),
			*OwnerCharacter->GetName());

		FSMController->ChangeState(EEnemyFSMStateType::Chase);
		return;
	}
	
	MoveTargetLocation = Result->GetItemAsLocation(0);

	UE_LOG(LogTemp, Warning, TEXT("[FSM][Flanking] EQS Target : %s"),
		*MoveTargetLocation.ToString());

	MoveToLocation(MoveTargetLocation, AcceptanceRadius, true);
}