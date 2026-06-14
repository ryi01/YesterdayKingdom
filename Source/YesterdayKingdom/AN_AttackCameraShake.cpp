// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_AttackCameraShake.h"

#include "BaseCharacter.h"
#include "CombatBaseComponent.h"

void UAN_AttackCameraShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp) return;
	ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (!Character) return;
	UCombatBaseComponent* CombatBaseComponent = Character->GetCombatComponent();
	if (CombatBaseComponent)
	{
		CombatBaseComponent->PlayCurrentAttackCameraShake();
	}
}
