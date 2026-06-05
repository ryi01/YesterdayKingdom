// Fill out your copyright notice in the Description page of Project Settings.


#include "CooldownStateComponent.h"
#include "EnemyBase.h"
#include "EnemyDefinition.h"
#include "EnemyFSMControllerComponent.h"

void UCooldownStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	CooldownElapsedTime = 0.f;
	
	if (OwnerCharacter) StopMove();
	if (EnemyDefinition) CooldownDuration = EnemyDefinition->FSMTimeConfig.CooldownTime.GetRandomTime();
	else CooldownDuration = 1.f;
	
	UE_LOG(LogTemp, Log, TEXT("[FSM][Cooldown] Enter : %s / Duration = %.2f"),
		OwnerCharacter ? *OwnerCharacter->GetName() : TEXT("None"),
		CooldownDuration);
}

void UCooldownStateComponent::OnStateUpdate(float DeltaTime)
{
	Super::OnStateUpdate(DeltaTime);
	if (!FSMController) return;
	
	if (IsOwnerDead())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Dead);
		return;
	}
	CooldownElapsedTime += DeltaTime;

	if (CooldownElapsedTime >= CooldownDuration)
	{
		const EEnemyFSMStateType SelectedNextState = SelectNextStateAfterCooldown();
		UE_LOG(LogTemp, Warning, TEXT("[FSM][Cooldown] Next State : %d"),
			static_cast<uint8>(SelectedNextState));
		FSMController->ChangeState(SelectedNextState);
	}
}

void UCooldownStateComponent::OnStateExit()
{
	Super::OnStateExit();
	
	CooldownElapsedTime = 0.f;

	UE_LOG(LogTemp, Log, TEXT("[FSM][Cooldown] Exit : %s"),
		OwnerCharacter ? *OwnerCharacter->GetName() : TEXT("None"));
}

EEnemyFSMStateType UCooldownStateComponent::SelectNextStateAfterCooldown() const
{
	if (!EnemyDefinition) return NextState;

	const float SafeBackStepChance = FMath::Clamp(EnemyDefinition->FSMActionConfig.BackStepChance, 0.f, 1.f);	
	const float SafeFlankingChance = FMath::Clamp(EnemyDefinition->FSMActionConfig.FlankingChance, 0.f, 1.f);	
	const float RandomValue = FMath::FRand();
	if (RandomValue < SafeBackStepChance) return EEnemyFSMStateType::BackStep;
	if (RandomValue < SafeBackStepChance + SafeFlankingChance) return EEnemyFSMStateType::Flanking;
	
	return EEnemyFSMStateType::Chase;
}
