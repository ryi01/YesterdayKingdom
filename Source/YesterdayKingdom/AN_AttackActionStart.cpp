// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_AttackActionStart.h"

#include "EnemyBase.h"
#include "EnemyFSMControllerComponent.h"
#include "JumpAttackStateComponent.h"


void UAN_AttackActionStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AN_AttackActionStart] MeshComp None"));
		return;
	}

	AEnemyBase* EnemyBase = Cast<AEnemyBase>(MeshComp->GetOwner());
	if (!EnemyBase)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AN_AttackActionStart] Owner Cast Failed / Owner=%s"),
			MeshComp->GetOwner()
				? *MeshComp->GetOwner()->GetName()
				: TEXT("None"));
		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("[AN_AttackActionStart] Enemy Found / Enemy=%s"),
		*EnemyBase->GetName());

	UEnemyFSMControllerComponent* FSMController =
		EnemyBase->GetFSMControllerComponent();

	if (!FSMController)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AN_AttackActionStart] FSMController None"));
		return;
	}

	UFSMStateComponent* CurrentState =
		FSMController->GetCurrentStateComponent();

	UE_LOG(LogTemp, Warning,
		TEXT("[AN_AttackActionStart] CurrentState=%s / Class=%s"),
		CurrentState
			? *CurrentState->GetName()
			: TEXT("None"),
		CurrentState
			? *CurrentState->GetClass()->GetName()
			: TEXT("None"));

	UAttackActionStateComponent* AttackActionState =
		Cast<UAttackActionStateComponent>(CurrentState);

	if (!AttackActionState)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AN_AttackActionStart] Cast Failed / CurrentStateClass=%s"),
			CurrentState
				? *CurrentState->GetClass()->GetName()
				: TEXT("None"));
		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("[AN_AttackActionStart] NotifyAttackActionStart Call"));

	AttackActionState->NotifyAttackActionStart();
}
