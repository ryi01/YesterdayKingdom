// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_RotationAttack.h"

#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UANS_RotationAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		if (UCharacterMovementComponent* MovementComponent = Character->GetComponentByClass<UCharacterMovementComponent>())
		{
			MovementComponent->bAllowPhysicsRotationDuringAnimRootMotion = true;
		}
	}
}

void UANS_RotationAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		if (UCharacterMovementComponent* MovementComponent = Character->GetComponentByClass<UCharacterMovementComponent>())
		{
			MovementComponent->bAllowPhysicsRotationDuringAnimRootMotion = false;
		}
	}
}
