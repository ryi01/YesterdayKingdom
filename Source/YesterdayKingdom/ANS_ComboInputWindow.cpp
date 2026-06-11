// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_ComboInputWindow.h"

#include "BaseCharacter.h"
#include "CombatBaseComponent.h"

void UANS_ComboInputWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (!OwnerCharacter) return;
	UCombatBaseComponent* CombatBaseComponent = OwnerCharacter->GetCombatComponent();
	if (!CombatBaseComponent) return;

	CombatBaseComponent->OpenComboInputBuffer();
}

void UANS_ComboInputWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (!OwnerCharacter) return;
	UCombatBaseComponent* CombatBaseComponent = OwnerCharacter->GetCombatComponent();
	if (!CombatBaseComponent) return;

	CombatBaseComponent->CloseComboInputBuffer();
}
