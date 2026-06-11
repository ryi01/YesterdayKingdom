// Fill out your copyright notice in the Description page of Project Settings.


#include "BackStepStateComponent.h"

#include "EnemyBase.h"
#include "EnemyDefinition.h"
#include "EnemyFSMControllerComponent.h"


void UBackStepStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	ElapsedTime = 0.f;
	bHasLanded = false;
	if (!OwnerCharacter || !FSMController) return;
	APawn* PlayerPawn = GetTargetPlayer();
	if (!PlayerPawn)
	{
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;
	}
	
	OwnerCharacter->OnEnemyLanded.Unbind();
	OwnerCharacter->OnEnemyLanded.BindUObject(this, &UBackStepStateComponent::HandleEnemyLanded);
	
	const float BackStepDistance = EnemyDefinition->FSMActionConfig.BackStepDistance;
	CurrentBackStepDuration = EnemyDefinition->FSMTimeConfig.BackStepTime.GetRandomTime();
	
	StopMove();
	SetFocusToPlayer();
	
	FVector AwayDirection = OwnerCharacter->GetActorLocation() - PlayerPawn->GetActorLocation();
	AwayDirection.Z = 0.f;
	
	if (AwayDirection.IsNearlyZero())
	{
		AwayDirection = -OwnerCharacter->GetActorForwardVector();
		AwayDirection.Z = 0.f;
	}
	AwayDirection.Normalize();
	const FVector LaunchVelocity = AwayDirection * BackStepDistance + FVector::UpVector * BackStepVerticalPower;
	
	OwnerCharacter->LaunchCharacter(LaunchVelocity, true, true);

	if (BackStepMontage)
	{
		OwnerCharacter->PlayAnimMontage(BackStepMontage);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[FSM][BackStep] Enter : %s / Distance = %.2f / Duration = %.2f / Launch = %s"),
		*OwnerCharacter->GetName(),
		BackStepDistance,
		CurrentBackStepDuration,
		*LaunchVelocity.ToString());
}

void UBackStepStateComponent::OnStateUpdate(float DeltaTime)
{
	Super::OnStateUpdate(DeltaTime);
	if (IsOwnerDead())
	{
		return;
	}

	ElapsedTime += DeltaTime;

	if (ElapsedTime >= CurrentBackStepDuration)
	{
		FSMController->ChangeState(NextState);
	}
}

void UBackStepStateComponent::OnStateExit()
{
	Super::OnStateExit();
	ClearFocusTarget();

	ElapsedTime = 0.f;
	CurrentBackStepDuration = 0.f;

	UE_LOG(LogTemp, Log, TEXT("[FSM][BackStep] Exit : %s"),
		OwnerCharacter ? *OwnerCharacter->GetName() : TEXT("None"));
}

void UBackStepStateComponent::HandleEnemyLanded()
{
	bHasLanded = true;

	UE_LOG(LogTemp, Warning, TEXT("[FSM][BackStep] Landed"));
}