// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_BossFootstep.h"

#include "EnemyBase.h"

void UAN_BossFootstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp) return;

	AEnemyBase* Enemy = Cast<AEnemyBase>(MeshComp->GetOwner());

	if (Enemy)
	{
		Enemy->PlayFootstepCameraShake();
	}
}
