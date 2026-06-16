// Fill out your copyright notice in the Description page of Project Settings.


#include "ReviveStateComponent.h"
#include "BaseStatComponent.h"
#include "EnemyDefinition.h"
#include "EnemyElite.h"
#include "EnemyFSMTypes.h"
#include "EnemyFSMControllerComponent.h"
#include "EnemyPuppetMaster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"

void UReviveStateComponent::OnStateEnter()
{
	Super::OnStateEnter();

	if (!OwnerCharacter || !EnemyDefinition)
	{
		return;
	}

	StopMove();
	
	OwnerCharacter->bUseControllerRotationYaw = false;

	if (UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement())
	{
		Movement->bUseControllerDesiredRotation = false;
		Movement->bOrientRotationToMovement = false;
		Movement->StopMovementImmediately();
	}

	if (UBaseStatComponent* StatComp = OwnerCharacter->GetStatComponent())
	{
		const float ReviveHP =
			StatComp->GetMaxHP() * EnemyDefinition->ReviveHPPercent;

		StatComp->SetCurrentHP(ReviveHP);
		UE_LOG(LogTemp, Warning, TEXT("[Revive] HP: %.1f / MaxHP: %.1f / IsDead: %d"),
		StatComp->GetCurrentHP(),
		StatComp->GetMaxHP(),
		StatComp->IsDead());
	}
	
	if (AEnemyElite* Elite = Cast<AEnemyElite>(OwnerCharacter))
	{
		Elite->RequestReviveEffect();
	}

	OwnerCharacter->ReviveMontage();
	
	UE_LOG(LogTemp, Warning, TEXT("[FSM][Revive] Enter"));

}

void UReviveStateComponent::OnStateUpdate(float DeltaTime)
{
	Super::OnStateUpdate(DeltaTime);

	if (!OwnerCharacter || !FSMController)
	{
		return;
	}

	// Revive 몽타주 재생 중이면 이동 막기
	StopMove();

	if (OwnerCharacter->IsAnyMontagePlaying())
	{
		return;
	}

	FSMController->ChangeState(EEnemyFSMStateType::Chase);
	
}

void UReviveStateComponent::OnStateExit()
{
	Super::OnStateExit();

	if (!OwnerCharacter || !EnemyDefinition)
	{
		return;
	}
	
	OwnerCharacter->bUseControllerRotationYaw = true;

	if (UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement())
	{
		Movement->bUseControllerDesiredRotation = true;
		Movement->bOrientRotationToMovement = false;
	}

	if (UBaseStatComponent* StatComp = OwnerCharacter->GetStatComponent())
	{
		const float ReviveHP =
			StatComp->GetMaxHP() * EnemyDefinition->ReviveHPPercent;

		StatComp->SetCurrentHP(ReviveHP);
	}
	
	if (AEnemyElite* Elite = Cast<AEnemyElite>(OwnerCharacter))
	{
		Elite->StopReviveEffect();
	}
	
	if (UCapsuleComponent* Capsule = OwnerCharacter->GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}
