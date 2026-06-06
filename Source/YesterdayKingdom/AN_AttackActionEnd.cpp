// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_AttackActionEnd.h"

#include "EnemyBase.h"
#include "EnemyFSMControllerComponent.h"
#include "JumpAttackStateComponent.h"

void UAN_AttackActionEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                               const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (AEnemyBase* EnemyBase = Cast<AEnemyBase>(MeshComp->GetOwner()))
	{
		if (UEnemyFSMControllerComponent* FsmControllerComponent = EnemyBase->GetFSMControllerComponent())
		{
			if (UAttackActionStateComponent* AttackActionStateComponent = Cast<UAttackActionStateComponent>(FsmControllerComponent->GetCurrentStateComponent()))
			{
				AttackActionStateComponent->NotifyAttackActionEnd();
			}
		}
	}
}
