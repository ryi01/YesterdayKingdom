// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_AttackTrace.h"

#include "Attacker.h"

void UANS_AttackTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	AActor* OwnerActor = MeshComp->GetOwner();
	if (OwnerActor && OwnerActor->GetClass()->ImplementsInterface(UAttacker::StaticClass()))
	{
		IAttacker::Execute_BeginAttackTrace(OwnerActor);
	}
}

void UANS_AttackTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	AActor* OwnerActor = MeshComp->GetOwner();
	if (OwnerActor && OwnerActor->GetClass()->ImplementsInterface(UAttacker::StaticClass()))
	{
		IAttacker::Execute_DoAttackTrace(OwnerActor);
	}
}

void UANS_AttackTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	AActor* OwnerActor = MeshComp->GetOwner();
	if (OwnerActor && OwnerActor->GetClass()->ImplementsInterface(UAttacker::StaticClass()))
	{
		IAttacker::Execute_EndAttackTrace(OwnerActor);
	}
}
