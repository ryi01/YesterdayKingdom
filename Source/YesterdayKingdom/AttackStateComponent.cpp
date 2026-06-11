// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackStateComponent.h"

#include "CombatBaseComponent.h"
#include "EnemyDefinition.h"
#include "EnemyBase.h"
#include "EnemyFSMTypes.h"
#include "EnemyFSMControllerComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"


void UAttackStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	if (!OwnerCharacter || !FSMController || !EnemyDefinition) return;
	if (IsOwnerDead()) return;
	
	FacePlayerInstant();
	
	StopMove();
	
	bAttackCompletedNormally = false;
	bShouldChaseAfterAttack = false;
	
	OwnerCharacter->OnAttackCompleted.Unbind();
	OwnerCharacter->OnAttackCompleted.BindUObject(this, &UAttackStateComponent::HandleAttackCompleted);
	
	FName AttackRowName = OwnerCharacter->GetSelectedAttackRowName();
	if (AttackRowName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[FSM][Attack] AttackRowName is None : %s"),
			*OwnerCharacter->GetName());

		FSMController->ChangeState(NextState);
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[FSM][Attack] Start Attack : %s / RowName = %s"),
		*OwnerCharacter->GetName(),
		*AttackRowName.ToString());

	OwnerCharacter->DoAttackByRowName(AttackRowName);
	OwnerCharacter->ClearSelectedAttackRowName();

}

void UAttackStateComponent::OnStateExit()
{
	Super::OnStateExit();
	if (OwnerCharacter)
	{
		OwnerCharacter->OnAttackCompleted.Unbind();
		OwnerCharacter->ClearSelectedAttackRowName();

		OwnerCharacter->GetCombatComponent()->EndAttackTrace();
		OwnerCharacter->GetCombatComponent()->ResetAttackState();

	}

	UE_LOG(LogTemp, Log, TEXT("[FSM][Attack] Exit"));
}


void UAttackStateComponent::HandleAttackCompleted()
{
	if (!FSMController) return;
	UE_LOG(LogTemp, Warning, TEXT("[FSM][Attack] Attack Completed"));
	bAttackCompletedNormally = true;
	if (bShouldChaseAfterAttack)
	{
		FSMController->ChangeState(EEnemyFSMStateType::Chase);
		return;
	}

	FSMController->ChangeState(NextState);
}
void UAttackStateComponent::CheckAttackTargetState()
{
	if (bShouldChaseAfterAttack) return;
	if (IsPlayerTooFarDuringAttack())
	{
		bShouldChaseAfterAttack = true;
		if (UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent())
		{
			CombatComp->SetCanContinueCombo(false);
		}

		UE_LOG(LogTemp, Warning,
			TEXT("[FSM][Attack] Player Too Far / Chase After Combo End"));
	}
}

bool UAttackStateComponent::IsPlayerTooFarDuringAttack() const
{
	if (!OwnerCharacter)return false;
	APawn* PlayerPawn = GetTargetPlayer();
	if (!PlayerPawn) return false;

	const float DistanceToPlayer = FVector::Dist2D(OwnerCharacter->GetActorLocation(),PlayerPawn->GetActorLocation());

	return DistanceToPlayer > ChaseAfterAttackDistance;
}

