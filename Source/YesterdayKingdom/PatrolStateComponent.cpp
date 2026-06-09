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
	
	bIsPatrolActive = true;
	bHasPatrolTarget = false;
	bIsWaitingPatrolEQS = false;
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

	// 목표가 없으면 다시 EQS 실행
	if (!bHasPatrolTarget)
	{
		if (!bIsWaitingPatrolEQS)
		{
			RunPatrolEQS();
		}

		return;
	}
	
	const float DistanceToPatrolTarget = FVector::Dist(OwnerCharacter->GetActorLocation(), PatrolTargetLocation);
	float FinalAcceptanceRadius = PatrolAcceptanceRadius;
	if (OwnerCharacter->GetCapsuleComponent())
	{
		FinalAcceptanceRadius += OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	}
	UE_LOG(LogTemp, Warning, TEXT("[FSM][Patrol] Distance: %.2f / Acceptance: %.2f / Target: %s / Owner: %s"),
		DistanceToPatrolTarget,
		FinalAcceptanceRadius,
		*PatrolTargetLocation.ToString(),
		*OwnerCharacter->GetName());

	
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
	
	bIsPatrolActive = false;
	bHasPatrolTarget = false;
	bIsWaitingPatrolEQS = false;
	PatrolTargetLocation = FVector::ZeroVector;
	
	if (OwnerCharacter)
	{
		UE_LOG(LogTemp, Log, TEXT("[FSM][Patrol] Exit : %s"), *OwnerCharacter->GetName());
	}
}
void UPatrolStateComponent::RunPatrolEQS()
{
	if (!FSMController || !OwnerCharacter) return;
	if (!bIsPatrolActive) return;
	if (bIsWaitingPatrolEQS) return;
	
	const UEnemyDefinition* Definition = OwnerCharacter->GetEnemyDefinition();
	
	if (!Definition)
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
	const float MaxPatrolDistance = Definition->PatrolRadius;
	
	if (MaxPatrolDistance <= MinPatrolDistance)
	{
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;
	}
	
	bIsWaitingPatrolEQS = true;

	UE_LOG(LogTemp, Warning, TEXT("[FSM][Patrol] Run EQS : %s / Min: %.2f / Max: %.2f"),
		*OwnerCharacter->GetName(),
		MinPatrolDistance,
		MaxPatrolDistance);
	
	FEnvQueryRequest QueryRequest(PatrolQuery, OwnerCharacter);

	QueryRequest.SetFloatParam(TEXT("MinPatrolDistance"), MinPatrolDistance);
	QueryRequest.SetFloatParam(TEXT("MaxPatrolDistance"), MaxPatrolDistance);

	QueryRequest.Execute(EEnvQueryRunMode::AllMatching,this,&UPatrolStateComponent::OnPatrolQueryFinished);
}

void UPatrolStateComponent::OnPatrolQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	bIsWaitingPatrolEQS = false;

	if (!FSMController || !OwnerCharacter) return;
	
	if (!Result.IsValid() || !Result->IsSuccessful())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;
	}
	TArray<FVector> Locations;
	Result->GetAllAsLocations(Locations);
	
	UE_LOG(LogTemp, Warning, TEXT("[FSM][Patrol] EQS Locations Count : %d / Owner: %s"),
	Locations.Num(),
	*OwnerCharacter->GetName());
	
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

	UE_LOG(LogTemp, Warning, TEXT("[FSM][Patrol] Pick Target : %s / Owner: %s"),
		*PatrolTargetLocation.ToString(),
		*OwnerCharacter->GetName());

	MoveToLocation(PatrolTargetLocation, PatrolAcceptanceRadius);
}
