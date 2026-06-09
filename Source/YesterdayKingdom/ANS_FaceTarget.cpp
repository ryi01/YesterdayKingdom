// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_FaceTarget.h"

#include "CombatBaseComponent.h"
#include "CommonEnumTypes.h"
#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"

void UANS_FaceTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ANS_FaceTarget] Begin"));
	}
}

void UANS_FaceTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (!MeshComp) return;

	AEnemyBase* Enemy = Cast<AEnemyBase>(MeshComp->GetOwner());
	if (!Enemy) return;

	UCombatBaseComponent* CombatComp = Enemy->GetCombatComponent();
	if (!CombatComp) return;

	const FAttackDataRow* AttackDataRow = CombatComp->GetCurrentAttackDataRow();

	float RotationSpeed = DefaultRotationSpeed;
	if (bUseAttackDataOption)
	{
		if (!AttackDataRow) return;
		if (!AttackDataRow->bFaceTargetDuringAction) return;

		RotationSpeed = AttackDataRow->FaceTargetRotationSpeed;
	}

	if (RotationSpeed <= 0.f) return;
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(Enemy, 0);
	if (!PlayerPawn) return;

	FVector Direction = PlayerPawn->GetActorLocation() - Enemy->GetActorLocation();
	Direction.Z = 0.f;

	if (Direction.IsNearlyZero()) return;

	const FRotator CurrentRotation = Enemy->GetActorRotation();
	const FRotator TargetRotation = Direction.Rotation();
	
	const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, FrameDeltaTime, RotationSpeed);
	Enemy->SetActorRotation(NewRotation);
}

void UANS_FaceTarget::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (bDebugLog)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ANS_FaceTarget] End"));
	}
}
