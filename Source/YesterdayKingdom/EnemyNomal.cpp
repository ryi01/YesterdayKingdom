// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyNomal.h"
#include "EnemyDefinition.h"
#include "EnemyNomalAIController.h"
#include "BaseStatComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyNomal::AEnemyNomal()
{
	PrimaryActorTick.bCanEverTick = false;

	OnAttackMontageEnded.BindUObject(this, &AEnemyNomal::AttackMontageEnded);

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

void AEnemyNomal::DoAIComboAttack()
{
	if (bIsAttacking)
	{
		return;
	}

	if (!EnemyDefinition || !EnemyDefinition->AttackMontage)
	{
		OnAttackCompleted.ExecuteIfBound();
		return;
	}

	bIsAttacking = true;

	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		bIsAttacking = false;
		OnAttackCompleted.ExecuteIfBound();
		return;
	}

	const float MontageLength = AnimInstance->Montage_Play(
		EnemyDefinition->AttackMontage,
		1.0f,
		EMontagePlayReturnType::MontageLength,
		0.0f,
		true
	);

	if (MontageLength > 0.0f)
	{
		AnimInstance->Montage_SetEndDelegate(
			OnAttackMontageEnded,
			EnemyDefinition->AttackMontage
		);
	}
	else
	{
		bIsAttacking = false;
		OnAttackCompleted.ExecuteIfBound();
	}
}

void AEnemyNomal::DoAIChargedAttack()
{
	// 아직 차지 공격 DA가 없으므로 임시로 일반 공격 처리
	// 나중에 UEnemyDefinition에 ChargedAttackMontage를 추가하면 여기만 교체
	DoAIComboAttack();
}

void AEnemyNomal::AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = false;

	OnAttackCompleted.ExecuteIfBound();
}

const FVector& AEnemyNomal::GetLastDangerLocation() const
{
	return LastDangerLocation;
}

float AEnemyNomal::GetLastDangerTime() const
{
	return LastDangerTime;
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

	if (DamageSource && DamageSource->ActorHasTag(TEXT("Player")))
	{
		LastDangerLocation = DamageLocation;

		if (UWorld* World = GetWorld())
		{
			LastDangerTime = World->GetTimeSeconds();
		}
	}

	if (EnemyDefinition && EnemyDefinition->HitMontage)
	{
		PlayAnimMontage(EnemyDefinition->HitMontage);
	}
}

void AEnemyNomal::HandleDeath_Implementation()
{
	OnEnemyDied.Broadcast();

	if (EnemyDefinition && EnemyDefinition->DeathMontage)
	{
		PlayAnimMontage(EnemyDefinition->DeathMontage);
	}

	Super::HandleDeath_Implementation();
}

void AEnemyNomal::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	OnEnemyLanded.ExecuteIfBound();
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

float AEnemyNomal::GetCurrentHP() const
{
	if (!GetStatComponent())
	{
		return 0.f;
	}

	return GetStatComponent()->GetCurrentHP();
}

bool AEnemyNomal::IsDead() const
{
	if (!GetStatComponent())
	{
		return true;
	}
    
	return GetStatComponent()->IsDead();
}
