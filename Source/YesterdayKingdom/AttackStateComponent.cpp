// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackStateComponent.h"
#include "EnemyDefinition.h"
#include "EnemyBase.h"
#include "EnemyFSMTypes.h"
#include "EnemyFSMControllerComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"

void UAttackStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	
	if (!OwnerCharacter || !EnemyDefinition)
	{
		return;
	}

	StopMove();

	const FEnemyAttackSet& AttackSet = EnemyDefinition->AttackSet;

	// 서브 공격 사용 여부 판단
	if (!AttackSet.SubAttackRowName.IsNone() &&
		FMath::FRand() <= AttackSet.SubAttackChance)
	{
		OwnerCharacter->DoSubAttack();
	}
	else
	{
		OwnerCharacter->DoMainAttack();
	}

	UE_LOG(LogTemp, Log, TEXT("[FSM][Attack] Enter : %s"), *OwnerCharacter->GetName());
}

void UAttackStateComponent::OnStateUpdate(float DeltaTime)
{
	Super::OnStateUpdate(DeltaTime);
	
	if (!OwnerCharacter || !FSMController)
	{
		return;
	}

	// 죽었으면 Down
	if (OwnerCharacter->IsDead())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Down);
		return;
	}

	// 공격 끝나면 Chase 복귀
	if (!OwnerCharacter->IsAttacking())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Chase);
	}
}

void UAttackStateComponent::OnStateExit()
{
	Super::OnStateExit();
}
