// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadStateComponent.h"

#include "AIController.h"
#include "EnemyBase.h"
#include "EnemyDefinition.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDeadStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	if (!OwnerCharacter) return;
	SetRootMotionFromMontage(true);
	StopMove();
	if (UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}
	if (AAIController* AIController = Cast<AAIController>(OwnerCharacter->GetController()))
	{
		AIController->StopMovement();
	}
	if (UCapsuleComponent* Capsule = OwnerCharacter->GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	OwnerCharacter->SetActorEnableCollision(false);

	float FinalDestroyDelay = DestroyDelay;
	
	if (EnemyDefinition && EnemyDefinition->DeathMontage)
	{
		if (UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance())
		{
			DeathMontageEndedDelegate.Unbind();
			DeathMontageEndedDelegate.BindUObject(this, &UDeadStateComponent::HandleDeathMontageEnded);

			AnimInstance->Montage_Play(EnemyDefinition->DeathMontage);
			AnimInstance->Montage_SetEndDelegate(DeathMontageEndedDelegate, EnemyDefinition->DeathMontage);

			const float MontageLength = EnemyDefinition->DeathMontage->GetPlayLength();
			const float FreezeTime = FMath::Max(0.01f, MontageLength - FreezeBeforeDeathMontageEndTime);

			GetWorld()->GetTimerManager().SetTimer(FreezeDeathPoseTimerHandle,this, &UDeadStateComponent::FreezeDeathPose, FreezeTime, false);
			FinalDestroyDelay = MontageLength + DestroyDelay;
		}
	}
	
	if (AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerCharacter))
	{
		FinalDestroyDelay = Enemy->GetDeathDestroyDelay();
	}
	UE_LOG(LogTemp, Warning, TEXT("[FSM][Dead] Enter : %s"),
		*OwnerCharacter->GetName());
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &UDeadStateComponent::DestroyOwner, FinalDestroyDelay, false);
}

void UDeadStateComponent::OnStateExit()
{
	Super::OnStateExit();
	GetWorld()->GetTimerManager().ClearTimer(DestroyTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FreezeDeathPoseTimerHandle);
}

void UDeadStateComponent::HandleDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	FreezeDeathPose();
}

void UDeadStateComponent::FreezeDeathPose()
{
	if (!OwnerCharacter) return;

	if (USkeletalMeshComponent* MeshComp = OwnerCharacter->GetMesh())
	{
		MeshComp->bPauseAnims = true;
		MeshComp->bNoSkeletonUpdate = false;
	}
}

void UDeadStateComponent::DestroyOwner()
{
	if (!OwnerCharacter) return;

	OwnerCharacter->Destroy();
}
