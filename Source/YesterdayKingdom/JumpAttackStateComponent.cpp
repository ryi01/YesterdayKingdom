// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpAttackStateComponent.h"

#include "CombatBaseComponent.h"
#include "EnemyBase.h"
#include "EnemyFSMControllerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UJumpAttackStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	
	ElapsedTime = 0.f;
	bDidJumpToPlayer = false;
	bStartedAttack = false;
	CurrentStep = EJumpAttackStep::JumpStart;

	if (!OwnerCharacter || !FSMController) return;
	
	APawn* PlayerPawn = GetTargetPlayer();
	if (!PlayerPawn)
	{
		FSMController->ChangeState(NextState);
		return;
	}
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
	CurrentJumpUpPower = CurrentAttackDataRow->JumpUpPower;
	CurrentAttackTriggerDistance = CurrentAttackDataRow->AttackTriggerDistance;
	CurrentAttackTriggerHeight = CurrentAttackDataRow->AttackTriggerHeight;
	
	UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent();
	if (!CombatComp)
	{
		FSMController->ChangeState(NextState);
		return;
	}
	StopMove();
	if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
	}
	
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) 
	{
		FSMController->ChangeState(NextState);
		return;
	}
	
	FacePlayerInstant();
	SetRootMotionFromMontage(false);

	CombatComp->RequestAttackByRow(CurrentAttackRowName);

	OwnerCharacter->LaunchCharacter(FVector(0.f, 0.f, CurrentJumpUpPower), false, true);

	OwnerCharacter->ClearSelectedAttackRowName();
	
	UE_LOG(LogTemp, Warning, TEXT("[FSM][JumpAttack] Enter : %s / Distance = %.2f"),
		*OwnerCharacter->GetName(),
		GetDistanceToPlayer());
}


void UJumpAttackStateComponent::OnStateUpdate(float DeltaTime)
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
		FSMController->ChangeState(NextState);
		return;
	}

	if (CurrentStep == EJumpAttackStep::Flying && !bStartedAttack)
	{
		TryStartAttackSection();
	}
	
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

	const FVector OwnerLocation = OwnerCharacter->GetActorLocation();

	const float DistanceToTarget = FVector::Dist2D(OwnerLocation, ActionTargetLocation);
	const float HeightDiff = FMath::Abs(OwnerLocation.Z - ActionTargetLocation.Z);

	if (DistanceToTarget <= CurrentAttackTriggerDistance && HeightDiff <= CurrentAttackTriggerHeight)
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
	
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	bStartedAttack = true;
	CurrentStep = EJumpAttackStep::Attack;
	if (!CombatComp->JumpToNextAttackSection())
	{
		FSMController->ChangeState(NextState);
	}

}

