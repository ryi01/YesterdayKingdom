// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_AttackPassThrough.h"

#include "BaseCharacter.h"
#include "CombatBaseComponent.h"

void UANS_AttackPassThrough::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (!MeshComp) return;

	ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (!OwnerCharacter) return;

	UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent();
	if (!CombatComp) return;

	CombatComp->SetPawnPassThrough(true);
}

void UANS_AttackPassThrough::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (!MeshComp) return;

	ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (!OwnerCharacter) return;

	UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent();
	if (!CombatComp) return;

	CombatComp->SetPawnPassThrough(false);
}
