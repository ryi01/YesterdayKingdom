// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackStateComponent.h"
#include "EnemyFSMControllerComponent.h"
#include "EnemyBase.h"
void UAttackStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	if (!OwnerCharacter || !FSMController || !EnemyDefinition) return;
	if (IsOwnerDead())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Dead);
		return;
	}
	StopMove();
	
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
	}

	UE_LOG(LogTemp, Log, TEXT("[FSM][Attack] Exit"));
}

void UAttackStateComponent::HandleAttackCompleted()
{
	if (!FSMController) return;
	UE_LOG(LogTemp, Warning, TEXT("[FSM][Attack] Attack Completed"));

	FSMController->ChangeState(NextState);
}

