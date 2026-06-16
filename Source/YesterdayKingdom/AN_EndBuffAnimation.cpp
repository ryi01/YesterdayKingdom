// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_EndBuffAnimation.h"

#include "PlayerCharacter.h"

void UAN_EndBuffAnimation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp) return;
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		Player->FinishBattleBuffCasting();
	}
}
