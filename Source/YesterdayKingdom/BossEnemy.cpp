// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"

#include "AttackStateComponent.h"
#include "BackStepStateComponent.h"
#include "ChaseStateComponent.h"
#include "CooldownStateComponent.h"
#include "EnemyFSMControllerComponent.h"
#include "FlankingStateComponent.h"
#include "IdleStatComponent.h"
#include "JumpAttackStateComponent.h"
#include "PatrolStateComponent.h"
#include "PatternSelectStateComponent.h"
#include "ReturnStateComponent.h"

ABossEnemy::ABossEnemy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(WeaponRoot);
	
	IdleState = CreateDefaultSubobject<UIdleStatComponent>(TEXT("IdleState"));
	ChaseState = CreateDefaultSubobject<UChaseStateComponent>(TEXT("ChaseState"));
	ReturnState = CreateDefaultSubobject<UReturnStateComponent>(TEXT("ReturnState"));
	PatrolState = CreateDefaultSubobject<UPatrolStateComponent>(TEXT("PatrolState"));
	PatternSelectState = CreateDefaultSubobject<UPatternSelectStateComponent>(TEXT("PatternSelectState"));
	AttackState = CreateDefaultSubobject<UAttackStateComponent>(TEXT("AttackState"));
	CooldownState = CreateDefaultSubobject<UCooldownStateComponent>(TEXT("CooldownState"));
	FlankingState = CreateDefaultSubobject<UFlankingStateComponent>(TEXT("FlankingState"));
	BackStepState = CreateDefaultSubobject<UBackStepStateComponent>(TEXT("BackStepState"));
	JumpAttackState = CreateDefaultSubobject<UJumpAttackStateComponent>(TEXT("JumpAttackState"));
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (WeaponMesh) WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
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
	if (PatternSelectState)
	{
		FSMController->RegisterState(EEnemyFSMStateType::PatternSelect, PatternSelectState);
	}
	if (AttackState)
	{
		FSMController->RegisterState(EEnemyFSMStateType::Attack, AttackState);
	}
	if (CooldownState)
	{
		FSMController->RegisterState(EEnemyFSMStateType::Cooldown, CooldownState);
	}
	if (FlankingState)
	{
		FSMController->RegisterState(EEnemyFSMStateType::Flanking, FlankingState);
	}	
	if (BackStepState)
	{
		FSMController->RegisterState(EEnemyFSMStateType::BackStep, BackStepState);
	}	
	if (JumpAttackState)
	{
		FSMController->RegisterState(EEnemyFSMStateType::JumpAttack, JumpAttackState);
	}

	FSMController->StartFSM(EEnemyFSMStateType::Idle);
}

void ABossEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (FSMController) FSMController->TickFSM(DeltaSeconds);
}

