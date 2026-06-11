// Fill out your copyright notice in the Description page of Project Settings.


#include "HitStateComponent.h"

#include "EnemyBase.h"
#include "EnemyDefinition.h"
#include "EnemyFSMControllerComponent.h"
#include "EnemyFSMTypes.h"
#include "GameFramework/CharacterMovementComponent.h"



void UHitStateComponent::OnStateEnter()
{
	Super::OnStateEnter();

	if (OwnerCharacter)
	{
		SetRootMotionFromMontage(false);
		StopMove();
		if (UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
			MoveComp->Velocity = FVector::ZeroVector;
		}

		SetFocusToPlayer();
		
		if (EnemyDefinition && EnemyDefinition->HitMontage)
		{
			if (UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance())
			{
				OwnerCharacter->PlayAnimMontage(EnemyDefinition->HitMontage);
			}
		}
	}
	GetWorld()->GetTimerManager().ClearTimer(HitLockTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(HitLockTimerHandle, this, &UHitStateComponent::FinishHitState, HitReactionLockTime, false);
	UE_LOG(LogTemp, Warning, TEXT("[FSM][Hit] Enter"));
}

void UHitStateComponent::OnStateExit()
{
	Super::OnStateExit();
	HitMontageEndedDelegate.Unbind();
	UE_LOG(LogTemp, Warning, TEXT("[FSM][Hit] Exit"));
	
}
void UHitStateComponent::HandleHitMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	DecideNextState();
}

void UHitStateComponent::DecideNextState()
{
	if (!FSMController) return;
	if (IsOwnerDead())
	{
		return;
	}

	FSMController->ChangeState(EEnemyFSMStateType::Cooldown);
}

void UHitStateComponent::FinishHitState()
{
	DecideNextState();
}
