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
		UAnimMontage* HitMontageToPlay = OwnerCharacter->GetHitMontage();
		if (EnemyDefinition && HitMontageToPlay)
		{
			if (UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance())
			{
				
				AnimInstance->Montage_Stop(0.03f);
				OwnerCharacter->PlayAnimMontage(HitMontageToPlay);
			}
		}
	}

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
	UE_LOG(LogTemp, Warning, TEXT("[FSM][Hit] DecideNextState"));
	FSMController->ChangeState(NextState);
}

void UHitStateComponent::FinishHitState()
{
	DecideNextState();
}
