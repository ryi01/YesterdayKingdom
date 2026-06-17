// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"

#include "AttackBossStateComponent.h"
#include "BackStepStateComponent.h"
#include "ChaseStateComponent.h"
#include "CooldownStateComponent.h"
#include "DeadStateComponent.h"
#include "EnemyFSMControllerComponent.h"
#include "FlankingStateComponent.h"
#include "HitStateComponent.h"
#include "IdleStatComponent.h"
#include "JumpAttackStateComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PatrolStateComponent.h"
#include "PatternSelectStateComponent.h"
#include "PhaseChangeStateComponent.h"
#include "ReturnStateComponent.h"
#include "RotationAttackStateComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

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
	AttackState = CreateDefaultSubobject<UAttackBossStateComponent>(TEXT("AttackState"));
	CooldownState = CreateDefaultSubobject<UCooldownStateComponent>(TEXT("CooldownState"));
	FlankingState = CreateDefaultSubobject<UFlankingStateComponent>(TEXT("FlankingState"));
	BackStepState = CreateDefaultSubobject<UBackStepStateComponent>(TEXT("BackStepState"));
	JumpAttackState = CreateDefaultSubobject<UJumpAttackStateComponent>(TEXT("JumpAttackState"));
	RotationAttackState = CreateDefaultSubobject<URotationAttackStateComponent>(TEXT("RotationAttackState"));
	PhaseChangeState = CreateDefaultSubobject<UPhaseChangeStateComponent>(TEXT("PhaseChangeState"));
	HitState = CreateDefaultSubobject<UHitStateComponent>(TEXT("HitState"));
	DeadState = CreateDefaultSubobject<UDeadStateComponent>(TEXT("DeadState"));
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
	if (RotationAttackState)
	{
		FSMController->RegisterState(EEnemyFSMStateType::RotationAttack, RotationAttackState);
	}
	if (PhaseChangeState)
	{
		FSMController->RegisterState(EEnemyFSMStateType::PhaseChange, PhaseChangeState);
	}
	if (HitState)
	{
		FSMController->RegisterState(EEnemyFSMStateType::Hit, HitState);
	}
	if (DeadState)
	{
		FSMController->RegisterState(EEnemyFSMStateType::Dead, DeadState);
	}

	FSMController->StartFSM(EEnemyFSMStateType::Idle);
}

void ABossEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (FSMController) FSMController->TickFSM(DeltaSeconds);
}
//===============================================================================================
// 보스 페이즈 2 이펙트 
//===============================================================================================
void ABossEnemy::FinishPhaseChange()
{
	Super::FinishPhaseChange();
	
	if (GetCurrentPhase() >= 2)
	{
		StartPhase2PersistentEffect();
	}
}

void ABossEnemy::HandleDeath_Implementation()
{
	StopPhase2PersistentEffect();

	Super::HandleDeath_Implementation();
}

void ABossEnemy::StartPhase2PersistentEffect()
{
	if (!GetMesh()) return;
	// ========================================================
	// Aura FX
	// ========================================================
	if (!Phase2AuraFXComponent && Phase2AuraFX)
	{
		Phase2AuraFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(Phase2AuraFX,GetMesh(),Phase2AuraFXSocketName,Phase2AuraFXOffset,FRotator::ZeroRotator,EAttachLocation::KeepRelativeOffset,false);

		if (Phase2AuraFXComponent)
		{
			Phase2AuraFXComponent->SetRelativeScale3D(Phase2AuraFXScale);
			Phase2AuraFXComponent->Activate(true);
		}
	}
	// ========================================================
	// Crackle FX
	// ========================================================
	if (!Phase2CrackleFXComponent && Phase2CrackleFX)
	{
		Phase2CrackleFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(Phase2CrackleFX,GetMesh(),Phase2CrackleFXSocketName,Phase2CrackleFXOffset,FRotator::ZeroRotator,EAttachLocation::KeepRelativeOffset,false);

		if (Phase2CrackleFXComponent)
		{
			Phase2CrackleFXComponent->SetRelativeScale3D(Phase2CrackleFXScale);
			Phase2CrackleFXComponent->Activate(true);
		}
	}
	// ========================================================
	// Loop Sound
	// ========================================================
	if (!Phase2LoopAudioComponent && Phase2LoopSound)
	{
		Phase2LoopAudioComponent = UGameplayStatics::SpawnSoundAttached(Phase2LoopSound,GetMesh(),NAME_None,FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::KeepRelativeOffset,false,Phase2LoopSoundVolume);

		if (Phase2LoopAudioComponent)
		{
			Phase2LoopAudioComponent->bAutoDestroy = false;
			Phase2LoopAudioComponent->FadeIn(Phase2LoopSoundFadeInTime,Phase2LoopSoundVolume);
		}
	}
}

void ABossEnemy::StopPhase2PersistentEffect()
{
	if (Phase2AuraFXComponent)
	{
		Phase2AuraFXComponent->Deactivate();
		Phase2AuraFXComponent = nullptr;
	}

	if (Phase2CrackleFXComponent)
	{
		Phase2CrackleFXComponent->Deactivate();
		Phase2CrackleFXComponent = nullptr;
	}

	if (Phase2LoopAudioComponent)
	{
		Phase2LoopAudioComponent->FadeOut(
			Phase2LoopSoundFadeOutTime,
			0.f
		);

		Phase2LoopAudioComponent = nullptr;
	}
}

