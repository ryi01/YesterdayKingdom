// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyElite.h"

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
#include "PatrolStateComponent.h"
#include "Components/SkeletalMeshComponent.h"

AEnemyElite::AEnemyElite(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	IdleState = CreateDefaultSubobject<UIdleStatComponent>(TEXT("IdleState"));
	PatrolState = CreateDefaultSubobject<UPatrolStateComponent>(TEXT("PatrolState"));
	ChaseState = CreateDefaultSubobject<UChaseStateComponent>(TEXT("ChaseState"));
	AttackState = CreateDefaultSubobject<UAttackStateComponent>(TEXT("AttackState"));
	HitState = CreateDefaultSubobject<UHitStateComponent>(TEXT("HitState"));
	ReturnState = CreateDefaultSubobject<UReturnStateComponent>(TEXT("ReturnState"));
	DownState = CreateDefaultSubobject<UDownStateComponent>(TEXT("DownState"));
	ReviveState = CreateDefaultSubobject<UReviveStateComponent>(TEXT("ReviveState"));
	DeadState = CreateDefaultSubobject<UDeadStateComponent>(TEXT("DeadState"));
	
	StringSocketName = TEXT("StringSocket");
	
	StringMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StringMeshComponent"));
	StringMeshComponent->SetupAttachment(GetMesh(),StringSocketName);
	StringMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	FSMController->RegisterState(EEnemyFSMStateType::Attack, AttackState);
	FSMController->RegisterState(EEnemyFSMStateType::Hit, HitState);
	FSMController->RegisterState(EEnemyFSMStateType::Return, ReturnState);
	FSMController->RegisterState(EEnemyFSMStateType::Down, DownState);
	FSMController->RegisterState(EEnemyFSMStateType::Revive, ReviveState);
	FSMController->RegisterState(EEnemyFSMStateType::Dead, DeadState);
	
	FSMController->StartFSM(EEnemyFSMStateType::Idle);
	
	if (StringMeshComponent)
	{
		if (StringMesh)
		{
			StringMeshComponent->SetSkeletalMesh(StringMesh);
		}

		StringMeshComponent->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			StringSocketName
		);

		StringMeshComponent->SetLeaderPoseComponent(GetMesh());
	}
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

void AEnemyElite::NotifyDamage_Implementation(const FVector& DamageLocation, AActor* DamageSource)
{
	Super::NotifyDamage_Implementation(DamageLocation, DamageSource);
}

void AEnemyElite::ApplyDamage_Implementation(float Damage, AActor* DamageCauser, const FVector& DamageLocation,
	const FVector& DamageImpulse)
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
