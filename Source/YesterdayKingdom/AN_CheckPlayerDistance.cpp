// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_CheckPlayerDistance.h"

#include "AttackStateComponent.h"
#include "EnemyBase.h"
#include "EnemyFSMControllerComponent.h"
#include "FSMStateComponent.h"

void UAN_CheckPlayerDistance::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp) return;

	AEnemyBase* Enemy = Cast<AEnemyBase>(MeshComp->GetOwner());
	if (!Enemy) return;

	UEnemyFSMControllerComponent* FSMController = Enemy->GetFSMControllerComponent();
	if (!FSMController) return;

	UFSMStateComponent* CurrentState = FSMController->GetCurrentStateComponent();
	UAttackStateComponent* AttackState = Cast<UAttackStateComponent>(CurrentState);
	if (!AttackState) return;

	AttackState->CheckAttackTargetState();
}
