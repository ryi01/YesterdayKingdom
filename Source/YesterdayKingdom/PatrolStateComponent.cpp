// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolStateComponent.h"
#include "EnemyBase.h"
#include "EnemyFSMControllerComponent.h"
#include "EnemyFSMTypes.h"
#include "EnemyDefinition.h"
#include "Components/CapsuleComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPatrolStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	bHasPatrolTarget = false;

	PatrolTargetLocation = FVector::ZeroVector;
	if (!FSMController || !OwnerCharacter) return;
	if (IsOwnerDead())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Dead);
		return;
	}

	if (OwnerCharacter)
	{
		OwnerCharacter->SetDefaultMoveSpeed();
		UE_LOG(LogTemp, Log, TEXT("[FSM][Patrol] Enter : %s"), *OwnerCharacter->GetName());
	}
	RunPatrolEQS();
}

void UPatrolStateComponent::OnStateUpdate(float DeltaTime)
{
	Super::OnStateUpdate(DeltaTime);
	if (!FSMController || !OwnerCharacter) return;
	if (IsOwnerDead())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Dead);
		return;
	}
	if (IsPlayerInDetectRange())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Chase);
		return;
	}
	if (!bHasPatrolTarget) return;
	// 목표가 없으면 다시 EQS 실행
	if (!bHasPatrolTarget)
	{
		RunPatrolEQS();
		return;
	}
	
	const float DistanceToPatrolTarget = FVector::Dist(OwnerCharacter->GetActorLocation(), PatrolTargetLocation);
	float FinalAcceptanceRadius = PatrolAcceptanceRadius;
	if (OwnerCharacter->GetCapsuleComponent())
	{
		FinalAcceptanceRadius += OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	}
	if (DistanceToPatrolTarget <= FinalAcceptanceRadius)
	{
		StopMove();
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;	
	}
	MoveToLocation(PatrolTargetLocation, PatrolAcceptanceRadius);
}

void UPatrolStateComponent::OnStateExit()
{
	Super::OnStateExit();
	StopMove();
	bHasPatrolTarget = false;
	PatrolTargetLocation = FVector::ZeroVector;
	if (OwnerCharacter)
	{
		UE_LOG(LogTemp, Log, TEXT("[FSM][Patrol] Exit : %s"), *OwnerCharacter->GetName());
	}
}
void UPatrolStateComponent::RunPatrolEQS()
{
	if (!FSMController || !OwnerCharacter) return;
	if (!EnemyDefinition)
	{
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;
	}

	if (!PatrolQuery)
	{
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;
	}

	const float MinPatrolDistance = 100.f;
	const float MaxPatrolDistance = EnemyDefinition->PatrolRadius;
	
	if (MaxPatrolDistance <= MinPatrolDistance)
	{
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;
	}
	
	FEnvQueryRequest QueryRequest(PatrolQuery, OwnerCharacter);

	QueryRequest.SetFloatParam(TEXT("MinPatrolDistance"), MinPatrolDistance);
	QueryRequest.SetFloatParam(TEXT("MaxPatrolDistance"), MaxPatrolDistance);

	QueryRequest.Execute(EEnvQueryRunMode::AllMatching,this,&UPatrolStateComponent::OnPatrolQueryFinished);
}

void UPatrolStateComponent::OnPatrolQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	if (!FSMController || !OwnerCharacter) return;
	if (!Result.IsValid() || !Result->IsSuccessful())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;
	}
	TArray<FVector> Locations;
	Result->GetAllAsLocations(Locations);
	if (Locations.IsEmpty())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;
	}
	TArray<FVector> ValidLocations;

	for (const FVector& Location : Locations)
	{
		const float DistanceFromLast = FVector::Dist(Location, LastPatrolTargetLocation);

		if (DistanceFromLast > SameTargetRejectDistance)
		{
			ValidLocations.Add(Location);
		}
	}

	const TArray<FVector>& PickPool = ValidLocations.IsEmpty() ? Locations : ValidLocations;

	const int32 RandomIndex = FMath::RandRange(0, PickPool.Num() - 1);

	PatrolTargetLocation = PickPool[RandomIndex];
	LastPatrolTargetLocation = PatrolTargetLocation;
	bHasPatrolTarget = true;
	MoveToLocation(PatrolTargetLocation, PatrolAcceptanceRadius);
}
