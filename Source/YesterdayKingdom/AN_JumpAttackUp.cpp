// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_JumpAttackUp.h"

#include "EnemyBase.h"
#include "EnemyFSMControllerComponent.h"
#include "JumpAttackStateComponent.h"

void UAN_JumpAttackUp::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (AEnemyBase* EnemyBase = Cast<AEnemyBase>(MeshComp->GetOwner()))
	{
		if (UEnemyFSMControllerComponent* FsmControllerComponent = EnemyBase->GetFSMControllerComponent())
		{
			UJumpAttackStateComponent* JumpState = Cast<UJumpAttackStateComponent>(FsmControllerComponent->GetCurrentStateComponent());
			if (JumpState) JumpState->NotifyJumpToPlayer();
		}
	}
}
