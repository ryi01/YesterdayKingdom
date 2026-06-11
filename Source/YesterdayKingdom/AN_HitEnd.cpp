// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_HitEnd.h"

#include "BaseCharacter.h"
#include "EnemyBase.h"
#include "EnemyFSMControllerComponent.h"
#include "FSMStateComponent.h"
#include "HitStateComponent.h"

void UAN_HitEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                        const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp) return;
	AEnemyBase* Character = Cast<AEnemyBase>(MeshComp->GetOwner());
	if (!Character) return;
	UEnemyFSMControllerComponent* FSM = Character->GetComponentByClass<UEnemyFSMControllerComponent>();
	if (FSM)
	{
		UFSMStateComponent* StateComponent = FSM->GetCurrentStateComponent();
		if (StateComponent)
		{
			UHitStateComponent* HitStateComponent = Cast<UHitStateComponent>(StateComponent);
			if (HitStateComponent) HitStateComponent->FinishHitState();
		}
	}
	
}
