// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"

#include "ChaseStateComponent.h"
#include "EnemyFSMControllerComponent.h"
#include "IdleStatComponent.h"
#include "ReturnStateComponent.h"

ABossEnemy::ABossEnemy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	FSMController = CreateDefaultSubobject<UEnemyFSMControllerComponent>(TEXT("FSMController"));
	IdleState = CreateDefaultSubobject<UIdleStatComponent>(TEXT("IdleState"));
	ChaseState = CreateDefaultSubobject<UChaseStateComponent>(TEXT("ChaseState"));
	ReturnState = CreateDefaultSubobject<UReturnStateComponent>(TEXT("ReturnState"));
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	FSMController->InitializeFSM(this);

	if (IdleState)
	{
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

	FSMController->StartFSM(EEnemyFSMStateType::Idle);
}

void ABossEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (FSMController) FSMController->TickFSM(DeltaSeconds);
}

