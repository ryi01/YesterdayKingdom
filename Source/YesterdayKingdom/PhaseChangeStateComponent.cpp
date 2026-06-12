// Fill out your copyright notice in the Description page of Project Settings.


#include "PhaseChangeStateComponent.h"

#include "EnemyBase.h"

void UPhaseChangeStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	if (!OwnerCharacter || !FSMController) return;
	StopMove();
	OwnerCharacter->ClearSelectedAttackRowName();
	UE_LOG(LogTemp, Warning,
	TEXT("[FSM][PhaseChange] Enter / Phase=%d / HP=%.2f"),
	OwnerCharacter->GetCurrentPhase(),
	OwnerCharacter->GetCurrentHP()
);

	GetWorld()->GetTimerManager().SetTimer(PhaseChangeTimerHandle, this, &UPhaseChangeStateComponent::FinishPhaseChange, PhaseChangeDuration, false);
}

void UPhaseChangeStateComponent::OnStateExit()
{
	Super::OnStateExit();
	GetWorld()->GetTimerManager().ClearTimer(PhaseChangeTimerHandle);
}

void UPhaseChangeStateComponent::FinishPhaseChange()
{
	if (!OwnerCharacter) return;

	OwnerCharacter->FinishPhaseChange();
}
