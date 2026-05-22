// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyNomal.h"
#include "EnemyDefinition.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyNomalAIController.h"
#include "Components/SkeletalMeshComponent.h"

AEnemyNomal::AEnemyNomal()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AEnemyNomalAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyNomal::BeginPlay()
{
	Super::BeginPlay();

	InitializeFromDefinition();
}

void AEnemyNomal::InitializeFromDefinition()
{
	if (!EnemyDefinition)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s : EnemyDefinition is null"), *GetName());
		return;
	}

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (EnemyDefinition->Mesh)
		{
			MeshComp->SetSkeletalMesh(EnemyDefinition->Mesh);
		}

		if (EnemyDefinition->AnimBP)
		{
			MeshComp->SetAnimInstanceClass(EnemyDefinition->AnimBP);
		}
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = EnemyDefinition->CombatMoveSpeed;
	}

	InitializeWeaponRoot();
}

void AEnemyNomal::PlayAttackMontage()
{
	if (!EnemyDefinition || !EnemyDefinition->AttackMontage)
	{
		return;
	}

	PlayAnimMontage(EnemyDefinition->AttackMontage);
}

void AEnemyNomal::PlayHitMontage()
{
	if (!EnemyDefinition || !EnemyDefinition->HitMontage)
	{
		return;
	}

	PlayAnimMontage(EnemyDefinition->HitMontage);
}

void AEnemyNomal::PlayDeathMontage()
{
	if (!EnemyDefinition || !EnemyDefinition->DeathMontage)
	{
		return;
	}

	PlayAnimMontage(EnemyDefinition->DeathMontage);
}

void AEnemyNomal::ApplyDamage_Implementation(
	float Damage,
	AActor* DamageCauser,
	const FVector& DamageLocation,
	const FVector& DamageImpulse
)
{
	Super::ApplyDamage_Implementation(Damage, DamageCauser, DamageLocation, DamageImpulse);
}

void AEnemyNomal::NotifyDamage_Implementation(
	const FVector& DamageLocation,
	AActor* DamageSource
)
{
	Super::NotifyDamage_Implementation(DamageLocation, DamageSource);
}

void AEnemyNomal::HandleDeath_Implementation()
{
	Super::HandleDeath_Implementation();
}

void AEnemyNomal::BeginAttackTrace_Implementation()
{
	Super::BeginAttackTrace_Implementation();
}

void AEnemyNomal::DoAttackTrace_Implementation()
{
	Super::DoAttackTrace_Implementation();
}

void AEnemyNomal::EndAttackTrace_Implementation()
{
	Super::EndAttackTrace_Implementation();
}
