// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_AttackAnimationEnd.h"

#include "Attacker.h"

void UAN_AttackAnimationEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	AActor* OwnerActor = MeshComp->GetOwner();
	if (OwnerActor && OwnerActor->GetClass()->ImplementsInterface(UAttacker::StaticClass()))
	{
		IAttacker::Execute_ClearAttackAnimation(OwnerActor);
	}
}
