// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackActionStateComponent.h"

#include "CombatBaseComponent.h"
#include "EnemyBase.h"
#include "EnemyFSMControllerComponent.h"

bool UAttackActionStateComponent::InitializeAttackActionFromData()
{
	if (!OwnerCharacter) return false;
	UCombatBaseComponent* CombatBaseComponent = OwnerCharacter->GetCombatComponent();
	if (!CombatBaseComponent) return false;
	CurrentAttackRowName  = OwnerCharacter->GetSelectedAttackRowName();
	if (CurrentAttackRowName .IsNone()) return false;
	CurrentAttackDataRow = CombatBaseComponent->GetAttackDataByRow(CurrentAttackRowName);
	if (!CurrentAttackDataRow) return false;
	if (!CurrentAttackDataRow->Montage) return false;
	
	if (CurrentAttackDataRow->MaxJumpAttackTime > 0.f)
	{
		MaxActionTime = CurrentAttackDataRow->MaxJumpAttackTime;
	}

	return true;
	
}
bool UAttackActionStateComponent::InitializeActionTarget()
{
	APawn* PlayerPawn = GetTargetPlayer();
	if (!PlayerPawn) return false;

	ActionTargetPawn = PlayerPawn;
	ActionTargetLocation = PlayerPawn->GetActorLocation();

	return true;
}

void UAttackActionStateComponent::ClearActionTargetData()
{
	ActionTargetPawn = nullptr;
	ActionTargetLocation = FVector::ZeroVector;
	CurrentActionMovePower = 900.f;
}

void UAttackActionStateComponent::ApplyMovePowerFromAttackData()
{
	if (!CurrentAttackDataRow) return;

	if (CurrentAttackDataRow->JumpForwardPower > 0.f)
	{
		CurrentActionMovePower = CurrentAttackDataRow->JumpForwardPower;
	}
}

bool UAttackActionStateComponent::MoveToActionTargetLocation(bool bOverrideXY, bool bOverrideZ)
{
	return MoveToTargetLocation(ActionTargetLocation, CurrentActionMovePower, bOverrideXY, bOverrideZ);
}
bool UAttackActionStateComponent::JumpToNextAttackSection()
{
	if (!OwnerCharacter || !FSMController) return false;

	UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent();
	if (!CombatComp) return false;
	if (!CombatComp->JumpToNextAttackSection())
	{
		FSMController->ChangeState(NextState);
		return false;
	}
	UE_LOG(LogTemp, Warning,
		TEXT("[FSM][AttackAction] JumpToNextSection Success / Row=%s"),
		*CurrentAttackRowName.ToString());

	return true;
}

bool UAttackActionStateComponent::MoveToTargetLocation(const FVector& TargetLocation, float MovePower, bool bOverrideXY,
	bool bOverrideZ)
{
	if (!OwnerCharacter) return false;

	FVector MoveDirection = TargetLocation - OwnerCharacter->GetActorLocation();
	MoveDirection.Z = 0.f;

	if (MoveDirection.IsNearlyZero())
	{
		MoveDirection = OwnerCharacter->GetActorForwardVector();
		MoveDirection.Z = 0.f;
	}

	MoveDirection.Normalize();

	const FVector LaunchVelocity = MoveDirection * MovePower;
	
	OwnerCharacter->LaunchCharacter(LaunchVelocity, bOverrideXY, bOverrideZ);
	
	return true;
}



void UAttackActionStateComponent::NotifyAttackActionStart()
{
}

void UAttackActionStateComponent::NotifyAttackActionEnd()
{
	UE_LOG(LogTemp, Warning,
		TEXT("[FSM][AttackAction] EndNotify Called / Row=%s / bFinished=%d"),
		*CurrentAttackRowName.ToString(),
		bFinished);
	FinishAttackAction();
}

void UAttackActionStateComponent::FinishAttackAction()
{
	if (bFinished) return;
	if (!FSMController) return;
	bFinished = true;
	
	StopActionMovement();
	
	UE_LOG(LogTemp, Warning,
		TEXT("[FSM][AttackAction] FinishAttackAction / Row=%s / NextState=%d"),
		*CurrentAttackRowName.ToString(),
		static_cast<int32>(NextState));
	FSMController->ChangeState(NextState);
}

void UAttackActionStateComponent::ClearAttackActionData()
{
	ElapsedTime = 0.f;
	bFinished = false;
	CurrentAttackRowName = NAME_None;
	CurrentAttackDataRow = nullptr;
}

void UAttackActionStateComponent::StopActionMovement()
{
	if (!OwnerCharacter) return;

	StopMove();
	ClearFocusTarget();
	SetRootMotionFromMontage(false);
}

void UAttackActionStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	
	ClearAttackActionData();
	ClearActionTargetData();

	if (!OwnerCharacter || !FSMController)
	{
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

	StopMove();
	SetFocusToPlayer();

	UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent();
	if (!CombatComp)
	{
		FSMController->ChangeState(NextState);
		return;
	}

	if (!CombatComp->RequestAttackByRow(CurrentAttackRowName))
	{
		FSMController->ChangeState(NextState);
	}
	
}

void UAttackActionStateComponent::OnStateUpdate(float X)
{
	Super::OnStateUpdate(X);
	
	if (!OwnerCharacter || !FSMController)
	{
		return;
	}

	if (!OwnerCharacter->IsAttacking())
	{
		FinishAttackAction();
	}
}

void UAttackActionStateComponent::OnStateExit()
{
	Super::OnStateExit();
	StopActionMovement();
}
