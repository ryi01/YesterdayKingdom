// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpAttackStateComponent.h"

#include "CombatBaseComponent.h"
#include "EnemyBase.h"
#include "EnemyFSMControllerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UJumpAttackStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	
	if (!OwnerCharacter || !FSMController) return;
	if (bFinished || !CurrentAttackDataRow) return;
	
	ElapsedTime = 0.f;
	bDidJumpToPlayer = false;
	bStartedAttack = false;
	CurrentStep = EJumpAttackStep::JumpStart;
	
	ApplyMovePowerFromAttackData();
	CurrentJumpUpPower = CurrentAttackDataRow->JumpUpPower;
	CurrentAttackTriggerDistance = CurrentAttackDataRow->AttackTriggerDistance;
	CurrentAttackTriggerHeight = CurrentAttackDataRow->AttackTriggerHeight;
	
	StopMove();
	if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
	}
	
	FacePlayerInstant();
	SetRootMotionFromMontage(false);
	
	OwnerCharacter->LaunchCharacter(FVector(0.f, 0.f, CurrentJumpUpPower), false, true);

	OwnerCharacter->ClearSelectedAttackRowName();
	
	UE_LOG(LogTemp, Warning, TEXT("[FSM][JumpAttack] Enter : %s / Distance = %.2f"),
		*OwnerCharacter->GetName(),
		GetDistanceToPlayer());
}


void UJumpAttackStateComponent::OnStateUpdate(float DeltaTime)
{
	UFSMStateComponent::OnStateUpdate(DeltaTime);
	if (!FSMController || !OwnerCharacter) return;
	if (IsOwnerDead())return;
	if (bFinished) return;
	
	ElapsedTime += DeltaTime;

	if (ElapsedTime >= MaxActionTime)
	{
		UE_LOG(LogTemp, Warning,
		TEXT("[FSM][JumpAttack] Timeout / Elapsed=%.2f / Max=%.2f"),
		ElapsedTime,
		MaxActionTime);
		FinishAttackAction();
		return;
	}

	if (CurrentStep == EJumpAttackStep::Flying && !bStartedAttack)
	{
		TryStartAttackSection();
	}
	UE_LOG(LogTemp, Warning,
	TEXT("[FSM][JumpAttack] Update / Step=%d / Started=%d / Finished=%d"),
	static_cast<int32>(CurrentStep),
	bStartedAttack,
	bFinished);
}

void UJumpAttackStateComponent::OnStateExit()
{
	Super::OnStateExit();

	if (OwnerCharacter)
	{
		OwnerCharacter->ClearSelectedAttackRowName();

		if (UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent())
		{
			CombatComp->EndAttackTrace();
			CombatComp->ResetAttackState();
		}
	}

	ClearFocusTarget();

	SetRootMotionFromMontage(false);

	bDidJumpToPlayer = false;
	bStartedAttack = false;
	CurrentStep = EJumpAttackStep::None;

	ClearActionTargetData();
	ClearAttackActionData();

	UE_LOG(LogTemp, Log, TEXT("[FSM][JumpAttack] Exit"));
}

void UJumpAttackStateComponent::NotifyAttackActionStart()
{
	if (bDidJumpToPlayer) return;
	if (!OwnerCharacter || !FSMController) return;

	bDidJumpToPlayer = true;
	CurrentStep = EJumpAttackStep::Flying;
	if (!JumpToNextAttackSection()) return;
	SetRootMotionFromMontage(false);
	MoveToActionTargetLocation(true, false);
	ClearFocusTarget();
}


void UJumpAttackStateComponent::TryStartAttackSection()
{
	if (bStartedAttack) return;
	if (!OwnerCharacter || !FSMController) return;
	UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement();
	if (!MovementComponent) return;
	const FVector OwnerLocation = OwnerCharacter->GetActorLocation();

	const float DistanceToTarget = FVector::Dist2D(OwnerLocation, ActionTargetLocation);
	const float HeightAboveTarget = OwnerLocation.Z - ActionTargetLocation.Z;
	const bool bIsDescending = MovementComponent->Velocity.Z < 0.f;
	const bool bWithinHorizontalRange =DistanceToTarget <= CurrentAttackTriggerDistance;
	const bool bWithinAttackHeight =HeightAboveTarget <= CurrentAttackTriggerHeight && HeightAboveTarget >= -100.f;
	
	UE_LOG(LogTemp, Warning,
			TEXT("[FSM][JumpAttack] TryStart / Distance=%.2f <= %.2f / HeightAbove=%.2f <= %.2f / VelocityZ=%.2f / Descending=%d"),
			DistanceToTarget,
			CurrentAttackTriggerDistance,
			HeightAboveTarget,
			CurrentAttackTriggerHeight,
			MovementComponent->Velocity.Z,
			bIsDescending);
	if (bIsDescending  && bWithinHorizontalRange  && bWithinAttackHeight)
	{
		StartAttackSection();
	}
	
}

void UJumpAttackStateComponent::StartAttackSection()
{
	if (bStartedAttack) return;
	if (!OwnerCharacter || !FSMController) return;
	
	UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent();
	if (!CombatComp) return;

	bStartedAttack = true;
	CurrentStep = EJumpAttackStep::Attack;
	if (!CombatComp->JumpToNextAttackSection())
	{
		FSMController->ChangeState(NextState);
	}

}

