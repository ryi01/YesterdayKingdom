// Fill out your copyright notice in the Description page of Project Settings.


#include "DownStateComponent.h"

#include "EnemyDefinition.h"
#include "EnemyFSMControllerComponent.h"
#include "EnemyFSMTypes.h"
#include "EnemyBase.h"
#include "EnemyElite.h"
#include "PhysicsEngine/PhysicsSettings.h"

void UDownStateComponent::OnStateEnter()
{
	Super::OnStateEnter();

	StopMove();
	OwnerCharacter->DownMontage();

	UE_LOG(LogTemp, Log, TEXT("[FSM][Down] Enter"));

}

void UDownStateComponent::OnStateUpdate(float DeltaTime)
{
	Super::OnStateUpdate(DeltaTime);
	
	if (!OwnerCharacter || !FSMController || !EnemyDefinition)
	{
		return;
	}

	const float DownTime = EnemyDefinition->ReviveDelay;

	if (FSMController->GetStateElapsedTime() >= DownTime)
	{
		FSMController->ChangeState(EEnemyFSMStateType::Revive);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[Down] Time: %.2f / ReviveDelay: %.2f"),
	FSMController->GetStateElapsedTime(),
	EnemyDefinition->ReviveDelay);
	
	UE_LOG(LogTemp, Warning, TEXT("[Down] Go Revive"));
}

void UDownStateComponent::OnStateExit()
{
	Super::OnStateExit();
}
