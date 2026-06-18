// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_PuppetSound.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "EnemyNomal.h"
#include "EnemyElite.h"

//void UANS_PuppetSound::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
//{
//	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
//	
//	if (!MeshComp || !LoopSound)
//	{
//		return;
//	}
//	
//	AActor* Owner = MeshComp->GetOwner();
//	if (!Owner)
//	{
//		return;
//	}
//	
//	if (AEnemyElite* EnemyElite = Cast<AEnemyElite>(Owner))
//	{
//		EnemyElite->StatePuppetLoopSound(LoopSound);
//	}
//}
//
//void UANS_PuppetSound::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
//{
//	Super::NotifyEnd(MeshComp, Animation);
//	
//	if (!MeshComp)
//	{
//		return;
//	}
//	
//	AActor* Owner = MeshComp->GetOwner();
//	if (!Owner)
//	{
//		return;
//	}
//	
//	if (AEnemyElite* EnemyElite = Cast<AEnemyElite>(Owner))
//	{
//		EnemyElite->StopPuppetLoopSound(FadeOutTime);
//	}
//	
//}
