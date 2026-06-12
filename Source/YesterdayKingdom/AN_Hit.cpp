// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_Hit.h"

#include "EnemyBase.h"

void UAN_Hit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                     const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	AEnemyBase* EnemyBase = Cast<AEnemyBase>(MeshComp->GetOwner());
	if (EnemyBase)
	{
		EnemyBase->SetHit();
	}
}
