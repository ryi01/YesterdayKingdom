// Fill out your copyright notice in the Description page of Project Settings.


#include "CooldownStateComponent.h"
#include "EnemyBase.h"
#include "EnemyDefinition.h"
#include "EnemyFSMControllerComponent.h"

void UCooldownStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	CooldownElapsedTime = 0.f;
	
	StopMove(); 
	ClearFocusTarget();
	SetRootMotionFromMontage(false);
	if (OwnerCharacter)
	{
		FacePlayerInstant();
		SetFocusToPlayer();
	}

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
	ClearFocusTarget();

	UE_LOG(LogTemp, Log, TEXT("[FSM][Cooldown] Exit : %s"),
		OwnerCharacter ? *OwnerCharacter->GetName() : TEXT("None"));
}

EEnemyFSMStateType UCooldownStateComponent::SelectNextStateAfterCooldown() const
{
	if (!EnemyDefinition) return NextState;
	
	const float DistanceToPlayer = GetDistance2DToPlayer();

	const FEnemyFSMActionConfig& ActionConfig = EnemyDefinition->FSMActionConfig;

	const float SafeBackStepChance = FMath::Clamp(EnemyDefinition->FSMActionConfig.BackStepChance, 0.f, 1.f);	
	const float SafeFlankingChance = FMath::Clamp(EnemyDefinition->FSMActionConfig.FlankingChance, 0.f, 1.f);	
	const float RandomValue = FMath::FRand();
	if (DistanceToPlayer <= ActionConfig.ForceBackStepDistance)
	{
		const float CloseBackStepChance = FMath::Clamp(SafeBackStepChance + ActionConfig.CloseBackStepChanceBonus, 0.f, 1.f);
		if (RandomValue < CloseBackStepChance) return EEnemyFSMStateType::BackStep;
	}
	const bool bCanFlank = DistanceToPlayer >= ActionConfig.FlankingMinDistance && DistanceToPlayer <= ActionConfig.FlankingMaxDistance;
	if (bCanFlank && RandomValue < SafeBackStepChance + SafeFlankingChance) return EEnemyFSMStateType::Flanking;
	if (DistanceToPlayer <= ActionConfig.FlankingMinDistance && RandomValue < SafeBackStepChance)
	{
		return EEnemyFSMStateType::BackStep;
	}

	return EEnemyFSMStateType::Chase;
}
