// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"

#include "ChaseStateComponent.h"
#include "EnemyFSMControllerComponent.h"
#include "IdleStatComponent.h"
#include "PatrolStateComponent.h"
#include "ReturnStateComponent.h"

ABossEnemy::ABossEnemy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	FSMController = CreateDefaultSubobject<UEnemyFSMControllerComponent>(TEXT("FSMController"));
	IdleState = CreateDefaultSubobject<UIdleStatComponent>(TEXT("IdleState"));
	ChaseState = CreateDefaultSubobject<UChaseStateComponent>(TEXT("ChaseState"));
	ReturnState = CreateDefaultSubobject<UReturnStateComponent>(TEXT("ReturnState"));
	PatrolState = CreateDefaultSubobject<UPatrolStateComponent>(TEXT("PatrolState"));
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	FSMController->InitializeFSM(this);

	if (IdleState)
	{
		IdleState->SetNextIdleState(EEnemyFSMStateType::Patrol);
		FSMController->RegisterState(EEnemyFSMStateType::Idle, IdleState);
	}

	if (ChaseState)
	{
		FSMController->RegisterState(EEnemyFSMStateType::Chase, ChaseState);
	}

	if (ReturnState)
	{
		FSMController->RegisterState(EEnemyFSMStateType::Return, ReturnState);
	}
	
	if (PatrolState)
	{
		FSMController->RegisterState(EEnemyFSMStateType::Patrol, PatrolState);
	}

	FSMController->StartFSM(EEnemyFSMStateType::Idle);
}

void ABossEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (FSMController) FSMController->TickFSM(DeltaSeconds);
}

