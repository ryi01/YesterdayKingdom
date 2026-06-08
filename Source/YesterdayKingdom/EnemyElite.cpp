// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyElite.h"

#include "EnemyFSMControllerComponent.h"
#include "IdleStatComponent.h"
#include "ChaseStateComponent.h"
#include "AttackStateComponent.h"
#include "ReturnStateComponent.h"
#include "ReviveStateComponent.h"
#include "DownStateComponent.h"
#include "EnemyFSMTypes.h"
#include "EnemyPuppetMaster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyFSMControllerComponent.h"

AEnemyElite::AEnemyElite(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	IdleState = CreateDefaultSubobject<UIdleStatComponent>(TEXT("IdleState"));
	ChaseState = CreateDefaultSubobject<UChaseStateComponent>(TEXT("ChaseState"));
	AttackState = CreateDefaultSubobject<UAttackStateComponent>(TEXT("AttackState"));
	ReturnState = CreateDefaultSubobject<UReturnStateComponent>(TEXT("ReturnState"));
	DownState = CreateDefaultSubobject<UDownStateComponent>(TEXT("DownState"));
	ReviveState = CreateDefaultSubobject<UReviveStateComponent>(TEXT("ReviveState"));
}

bool AEnemyElite::IsPuppetMasterDead() const
{
	if (!PuppetMaster)
	{
		return false;
	}

	return PuppetMaster->IsDead();
}

void AEnemyElite::BeginPlay()
{
	Super::BeginPlay();
	
	if (PuppetMaster)
	{
		PuppetMaster->RegisterPuppet(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Elite] BeginPlay PuppetMaster is null"));
	}
	
	if (!FSMController)
	{
		return;
	}

	FSMController->InitializeFSM(this);

	FSMController->RegisterState(EEnemyFSMStateType::Idle, IdleState);
	FSMController->RegisterState(EEnemyFSMStateType::Chase, ChaseState);
	FSMController->RegisterState(EEnemyFSMStateType::Attack, AttackState);
	FSMController->RegisterState(EEnemyFSMStateType::Return, ReturnState);
	FSMController->RegisterState(EEnemyFSMStateType::Down, DownState);
	FSMController->RegisterState(EEnemyFSMStateType::Revive, ReviveState);

	FSMController->StartFSM(EEnemyFSMStateType::Idle);
}

void AEnemyElite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bTrueDead)
	{
		return;
	}

	if (PuppetMaster && IsPuppetMasterDead())
	{
		bTrueDead = true;
		Super::HandleDeath_Implementation();
		return;
	}

	if (FSMController)
	{
		FSMController->TickFSM(DeltaTime);
	}
	
	if (PuppetMaster)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Elite] Master: %s / MasterDead: %d"),
			*GetNameSafe(PuppetMaster),
			PuppetMaster->IsDead());
	}
	
}

void AEnemyElite::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (bPendingDownAfterLanded)
	{
		bPendingDownAfterLanded = false;

		if (FSMController)
		{
			FSMController->ChangeState(EEnemyFSMStateType::Down);
		}
	}	
}

void AEnemyElite::ForceTrueDeath()
{
	if (bTrueDead)
	{
		return;
	}

	bTrueDead = true;

	if (FSMController)
	{
		FSMController->StopFSM();
	}
	
	Super::HandleDeath_Implementation();
}

void AEnemyElite::SetPuppetMaster(AEnemyPuppetMaster* InMaster)
{
	PuppetMaster = InMaster;
	
	UE_LOG(LogTemp, Warning, TEXT("[Elite] SetPuppetMaster: %s"),
		*GetNameSafe(PuppetMaster));
}

void AEnemyElite::HandleDeath_Implementation()
{
	// 나중에 본체 죽었는지 체크해서 진짜 Dead 처리
	const bool bMasterDead = IsPuppetMasterDead();

	if (!bMasterDead)
	{
		// 공중에서 죽었으면 바로 Down으로 가지 말고,
		// 착지 후 Down으로 전환
		if (UCharacterMovementComponent* Movement = GetCharacterMovement())
		{
			if (!Movement->IsMovingOnGround())
			{
				bPendingDownAfterLanded = true;
				return;
			}
		}

		if (FSMController)
		{
			FSMController->ChangeState(EEnemyFSMStateType::Down);
		}

		return;
	}

	Super::HandleDeath_Implementation();
}

