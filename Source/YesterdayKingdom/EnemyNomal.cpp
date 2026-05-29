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
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AEnemyNomal::AEnemyNomal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	OnAttackMontageEnded.BindUObject(this, &AEnemyNomal::AttackMontageEnded);

	AIControllerClass = AEnemyNomalAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
//CombatBaseComponent = CreateDefaultSubobject<UCombatBaseComponent>(TEXT("CombatBaseComponent"));
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
	if (CombatBaseComponent) CombatBaseComponent->SetAttackDataTable(EnemyDefinition->AttackDataTable);
	InitializeWeaponRoot();
}

void AEnemyNomal::DoAIComboAttack()
{
	if (!EnemyDefinition) return;
	
	if (CombatBaseComponent) CombatBaseComponent->RequestAttackByRow(EnemyDefinition->AttackSet.MainAttackRowName);
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
	if (DamageCauser)
	{
		LastDamageCauser = DamageCauser;
	}

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

void AEnemyNomal::GiveRewardToKiller()
{
	if (!EnemyDefinition)
	{
		return;
	}

	AActor* Killer = LastDamageCauser.Get();
	if (!Killer)
	{
		return;
	}

	APlayerCharacter* Player = Cast<APlayerCharacter>(Killer);

	if (!Player && Killer->GetOwner())
	{
		Player = Cast<APlayerCharacter>(Killer->GetOwner());
	}

	if (!Player)
	{
		return;
	}

	if (UGoldComponent* GoldComp = Player->GetGoldComponent())
	{
		if (EnemyDefinition->GoldReward > 0)
		{
			GoldComp->AddGold(EnemyDefinition->GoldReward);
		}
	}

	if (UInventoryComponent* InventoryComp = Player->GetInventoryComponent())
	{
		for (const FEnemyItemReward& Reward : EnemyDefinition->ItemRewards)
		{
			if (Reward.ItemRowName.IsNone())
			{
				continue;
			}

			if (FMath::FRand() > Reward.DropChance)
			{
				continue;
			}

			const int32 MinCount = FMath::Max(1, Reward.MinCount);
			const int32 MaxCount = FMath::Max(MinCount, Reward.MaxCount);
			const int32 DropCount = FMath::RandRange(MinCount, MaxCount);

			InventoryComp->AddItem(Reward.ItemRowName, DropCount);
		}
	}
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
