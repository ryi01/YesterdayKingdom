// Fill out your copyright notice in the Description page of Project Settings.


#include "RotationAttackStateComponent.h"

#include "CombatBaseComponent.h"
#include "EnemyBase.h"
#include "EnemyFSMControllerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void URotationAttackStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	
	ClearAttackActionData();
	ClearActionTargetData();

	bStartedRotationMove = false;
	bStartedRotationAttack = false;
	RotationMoveElapsedTime = 0.f;
	CachedMaxWalkSpeed = 0.f;

	if (!OwnerCharacter || !FSMController) return;
	if (!InitializeActionTarget())
	{
		FSMController->ChangeState(NextState);
		return;
	}

	if (!InitializeAttackActionFromData())
	{
		FSMController->ChangeState(NextState);
		return;
	}

	ApplyMovePowerFromAttackData();
	
	bTrackPlayerDuringRotation = CurrentAttackDataRow->bTrackTargetDuringAction;
	
	UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent();
	if (!CombatComp)
	{
		FSMController->ChangeState(NextState);
		return;
	}

	if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
	{
		CachedMaxWalkSpeed = MovementComponent->MaxWalkSpeed;

		if (CurrentActionMovePower > 0.f)
		{
			MovementComponent->MaxWalkSpeed = CurrentActionMovePower;
		}
	}

	if (bStopMovementOnEnter)
	{
		StopMove();

		if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
		{
			MovementComponent->StopMovementImmediately();
		}
	}
	if (bFacePlayerOnEnter)
	{
		FacePlayerInstant();
	}
	
	SetRootMotionFromMontage(false);

	CombatComp->RequestAttackByRow(CurrentAttackRowName);

	OwnerCharacter->ClearSelectedAttackRowName();

	UE_LOG(LogTemp, Warning,
	TEXT("[FSM][RotationAttack] Enter : %s / Row=%s / Distance=%.2f / MoveSpeed=%.2f / MoveDuration=%.2f / Track=%d"),
	*OwnerCharacter->GetName(),
	*CurrentAttackRowName.ToString(),
	GetDistance2DToPlayer(),
	CurrentActionMovePower,
	RotationMoveDuration,
	bTrackPlayerDuringRotation);
	
}

void URotationAttackStateComponent::OnStateUpdate(float DeltaTime)
{
	Super::OnStateUpdate(DeltaTime);
	if (!FSMController || !OwnerCharacter) return;

	if (IsOwnerDead())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Dead);
		return;
	}

	if (bFinished) return;
	ElapsedTime += DeltaTime;
	if (ElapsedTime >= MaxActionTime)
	{
		FinishAttackAction();
		return;
	}
	
	if (bStartedRotationMove && !bStartedRotationAttack)
	{
		RotationMoveElapsedTime += DeltaTime;
		float DistanceToTargetLocation = FVector::Dist2D(OwnerCharacter->GetActorLocation(), ActionTargetLocation);
		if (bTrackPlayerDuringRotation && ActionTargetPawn.IsValid())
		{
			DistanceToTargetLocation = FVector::Dist2D(OwnerCharacter->GetActorLocation(),ActionTargetPawn->GetActorLocation());
		}

		if (DistanceToTargetLocation <= RotationMoveAcceptanceRadius || RotationMoveElapsedTime >= RotationMoveDuration)
		{
			bStartedRotationAttack = true;

			StopMove();
			ClearFocusTarget();
			
			UE_LOG(LogTemp, Warning, TEXT("[FSM][RotationAttack] Move Finished -> Attack Section"));
			if (!JumpToNextAttackSection()) return;
		}
	}
}

void URotationAttackStateComponent::OnStateExit()
{
	Super::OnStateExit();
	if (OwnerCharacter)
	{
		StopMove();

		if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
		{
			MovementComponent->StopMovementImmediately();

			if (CachedMaxWalkSpeed > 0.f)
			{
				MovementComponent->MaxWalkSpeed = CachedMaxWalkSpeed;
			}
		}
		
		OwnerCharacter->ClearSelectedAttackRowName();

		if (UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent())
		{
			CombatComp->EndAttackTrace();
			CombatComp->ResetAttackState();
		}
	}

	ClearFocusTarget();
	SetRootMotionFromMontage(false);

	bStartedRotationMove = false;
	bStartedRotationAttack = false;
	RotationMoveElapsedTime = 0.f;
	CachedMaxWalkSpeed = 0.f;

	UE_LOG(LogTemp, Warning,
	TEXT("[FSM][RotationAttack] Exit / Row=%s / StartedMove=%d / StartedAttack=%d / Finished=%d / Target=%s"),
	*CurrentAttackRowName.ToString(),
	bStartedRotationMove,
	bStartedRotationAttack,
	bFinished,
	*ActionTargetLocation.ToString());
	
	ClearActionTargetData();
	ClearAttackActionData();
}

void URotationAttackStateComponent::NotifyAttackActionStart()
{
	Super::NotifyAttackActionStart();
	UE_LOG(LogTemp, Warning,
	TEXT("[FSM][RotationAttack] StartNotify Called / bStartedMove=%d / Target=%s / Row=%s"),
	bStartedRotationMove,
	*ActionTargetLocation.ToString(),
	*CurrentAttackRowName.ToString());
	
	if (bStartedRotationMove) return;
	if (!OwnerCharacter || !FSMController) return;

	bStartedRotationMove = true;
	RotationMoveElapsedTime = 0.f;

	SetRootMotionFromMontage(false);
	
	if (bTrackPlayerDuringRotation && ActionTargetPawn.IsValid())
	{
		MoveToPlayer(RotationMoveAcceptanceRadius);
		SetFocusToPlayer();

		UE_LOG(LogTemp, Warning,
			TEXT("[FSM][RotationAttack] Start Notify -> MoveToActor / Target=%s / Row=%s"),
			*ActionTargetPawn->GetName(),
			*CurrentAttackRowName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning,
	TEXT("[FSM][RotationAttack] Start Notify -> MoveToLocation / Target=%s / Row=%s"),
	*ActionTargetLocation.ToString(),
	*CurrentAttackRowName.ToString());
		MoveToLocation(ActionTargetLocation, RotationMoveAcceptanceRadius, false);
		ClearFocusTarget();
	}
	
}
