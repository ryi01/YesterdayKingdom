// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyNomal.h"
#include "EnemyDefinition.h"
#include "EnemyNomalAIController.h"
#include "BaseStatComponent.h"
#include "CombatBaseComponent.h"
#include "GoldComponent.h"
#include "InventoryComponent.h"
#include "PlayerCharacter.h"
#include "Animation/AnimInstance.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AEnemyNomal::AEnemyNomal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	OnAttackMontageEnded.BindUObject(this, &AEnemyNomal::AttackMontageEnded);

	AIControllerClass = AEnemyNomalAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	WeaponSocketName = TEXT("katana3");
	
	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComponent"));
	WeaponMeshComponent->SetupAttachment(GetWeaponRoot());
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
//CombatBaseComponent = CreateDefaultSubobject<UCombatBaseComponent>(TEXT("CombatBaseComponent"));
}

void AEnemyNomal::BeginPlay()
{
	Super::BeginPlay();

	InitializeFromDefinition();
	
	if (CombatBaseComponent)
	{
		CombatBaseComponent->OnAttackEnded.AddDynamic(
			this,
			&AEnemyNomal::NotifyAttackEnded
		);
	}
	if (EnemyDefinition->WeaponMesh) WeaponMeshComponent->SetStaticMesh(EnemyDefinition->WeaponMesh);
}

void AEnemyNomal::DoAIComboAttack()
{
	if (bIsAttacking)
	{
		return;
	}

	if (!EnemyDefinition || !CombatBaseComponent)
	{
		OnAttackCompleted.ExecuteIfBound();
		return;
	}

	bIsAttacking = true;

	CombatBaseComponent->RequestAttackByRow(
		EnemyDefinition->AttackSet.MainAttackRowName
	);
}

void AEnemyNomal::DoAIChargedAttack()
{
	// 아직 차지 공격 DA가 없으므로 임시로 일반 공격 처리
	// 나중에 UEnemyDefinition에 ChargedAttackMontage를 추가하면 여기만 교체
	DoAIComboAttack();
}

void AEnemyNomal::NotifyAttackEnded()
{
	bIsAttacking = false;

	OnAttackCompleted.ExecuteIfBound();
}

void AEnemyNomal::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	OnEnemyLanded.ExecuteIfBound();
}

void AEnemyNomal::HandleDeath_Implementation()
{
	Super::HandleDeath_Implementation();
	
	bIsAttacking = false;
	OnAttackCompleted.Unbind();
	OnEnemyLanded.Unbind();
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}

	
	if (EnemyDefinition && EnemyDefinition->DeathMontage)
	{
		PlayAnimMontage(EnemyDefinition->DeathMontage);
	}
}
