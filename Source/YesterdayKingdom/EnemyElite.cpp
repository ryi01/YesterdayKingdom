// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyElite.h"

#include "AttackActionStateComponent.h"
#include "AttackBossStateComponent.h"
#include "EnemyFSMControllerComponent.h"
#include "IdleStatComponent.h"
#include "ChaseStateComponent.h"
#include "AttackStateComponent.h"
#include "HitStateComponent.h"
#include "ReturnStateComponent.h"
#include "ReviveStateComponent.h"
#include "DownStateComponent.h"
#include "DeadStateComponent.h"
#include "EnemyFSMTypes.h"
#include "EnemyPuppetMaster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyFSMControllerComponent.h"
#include "BaseStatComponent.h"
#include "CooldownStateComponent.h"
#include "PatrolStateComponent.h"
#include "PatternSelectStateComponent.h"
#include "Components/SkeletalMeshComponent.h"

AEnemyElite::AEnemyElite(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	IdleState = CreateDefaultSubobject<UIdleStatComponent>(TEXT("IdleState"));
	PatrolState = CreateDefaultSubobject<UPatrolStateComponent>(TEXT("PatrolState"));
	ChaseState = CreateDefaultSubobject<UChaseStateComponent>(TEXT("ChaseState"));
	CooldownState = CreateDefaultSubobject<UCooldownStateComponent>(TEXT("CooldownState"));
	PatternSelectState = CreateDefaultSubobject<UPatternSelectStateComponent>(TEXT("PatternSelectState"));
	AttackState = CreateDefaultSubobject<UAttackStateComponent>(TEXT("AttackState"));
	HitState = CreateDefaultSubobject<UHitStateComponent>(TEXT("HitState"));
	ReturnState = CreateDefaultSubobject<UReturnStateComponent>(TEXT("ReturnState"));
	DownState = CreateDefaultSubobject<UDownStateComponent>(TEXT("DownState"));
	ReviveState = CreateDefaultSubobject<UReviveStateComponent>(TEXT("ReviveState"));
	DeadState = CreateDefaultSubobject<UDeadStateComponent>(TEXT("DeadState"));
	
	PuppetLoopAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("PuppetLoopAudioComp"));
	PuppetLoopAudioComp->SetupAttachment(GetMesh());
	PuppetLoopAudioComp->bAutoActivate = false;
}

void AEnemyElite::StatePuppetLoopSound(USoundBase* Sound)
{
	if (!Sound || !PuppetLoopAudioComp)
	{
		return;
	}
	
	if (PuppetLoopAudioComp->IsPlaying())
	{
		PuppetLoopAudioComp->Stop();
	}
	
	PuppetLoopAudioComp->SetSound(Sound);
	PuppetLoopAudioComp->Play();
}

void AEnemyElite::StopPuppetLoopSound(float FadeOutTime)
{
	if (!PuppetLoopAudioComp)
	{
		return;
	}
	
	if (PuppetLoopAudioComp->IsPlaying())
	{
		PuppetLoopAudioComp->FadeOut(FadeOutTime, 0.0f);
	}
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
	if (!FSMController)
	{
		return;
	}
	SetHomeLocation(GetActorLocation());

	FSMController->InitializeFSM(this);

	FSMController->RegisterState(EEnemyFSMStateType::Idle, IdleState);
	FSMController->RegisterState(EEnemyFSMStateType::Patrol, PatrolState);
	FSMController->RegisterState(EEnemyFSMStateType::Chase, ChaseState);
	FSMController->RegisterState(EEnemyFSMStateType::Cooldown, CooldownState);
	FSMController->RegisterState(EEnemyFSMStateType::Attack, AttackState);
	FSMController->RegisterState(EEnemyFSMStateType::Hit, HitState);
	FSMController->RegisterState(EEnemyFSMStateType::PatternSelect, PatternSelectState);
	FSMController->RegisterState(EEnemyFSMStateType::Return, ReturnState);
	FSMController->RegisterState(EEnemyFSMStateType::Down, DownState);
	FSMController->RegisterState(EEnemyFSMStateType::Revive, ReviveState);
	FSMController->RegisterState(EEnemyFSMStateType::Dead, DeadState);
	
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
		ForceTrueDeath();
		return;
	}

	if (FSMController)
	{
		FSMController->TickFSM(DeltaTime);
	}
	
}

void AEnemyElite::ForceTrueDeath()
{
	if (bTrueDead)
	{
		return;
	}

	bTrueDead = true;
	
	Super::HandleDeath_Implementation();
}

void AEnemyElite::SetPuppetMaster(AEnemyPuppetMaster* InMaster)
{
	PuppetMaster = InMaster;
	
	UE_LOG(LogTemp, Warning, TEXT("[Elite] SetPuppetMaster: %s"),
		*GetNameSafe(PuppetMaster));
}

void AEnemyElite::RequestReviveEffect()
{
	if (PuppetMaster)
	{
		PuppetMaster->PlayReviveEffect(this);
	}
}

void AEnemyElite::StopReviveEffect()
{
	if (PuppetMaster)
	{
		PuppetMaster->StopReviveEffect();
	}
}

void AEnemyElite::NotifyDamage_Implementation(const FVector& DamageLocation, AActor* DamageSource)
{
	Super::NotifyDamage_Implementation(DamageLocation, DamageSource);
}

void AEnemyElite::ApplyDamage_Implementation(float Damage, AActor* DamageCauser, const FVector& DamageLocation,
	const FVector& DamageImpulse, EHitReactionType HitReactionType)
{
	if (DamageCauser)
	{
		LastDamageCauser = DamageCauser;
	}

	UBaseStatComponent* StatComp = GetStatComponent();
	if (!StatComp)
	{
		return;
	}

	StatComp->ApplyDamage(Damage);

	if (StatComp->IsDead())
	{
		if (bTrueDead)
		{
			ForceTrueDeath();
			return;
		}

		if (FSMController)
		{
			FSMController->ChangeState(EEnemyFSMStateType::Down);
		}

		return;
	}

	Super::NotifyDamage_Implementation(DamageLocation, DamageCauser);
}

