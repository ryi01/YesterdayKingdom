// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "BaseStatComponent.h"
#include "CombatBaseComponent.h"
#include "EnemyDefinition.h"
#include "GoldComponent.h"
#include "InventoryComponent.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyBase::AEnemyBase(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	OnAttackMontageEnded.BindUObject(this, &AEnemyBase::AttackMontageEnded);
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	HomeLocation = GetActorLocation();
	InitializeFromDefinition();
}
//===============================================================================================
// 초기화
//===============================================================================================
void AEnemyBase::InitializeFromDefinition()
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
	
	if (StatComponent)
	{
		StatComponent->InitializeStat(StatComponent->GetStatusDT(), EnemyDefinition->StatRowName);
	}
	InitializeWeaponRoot();
}
void AEnemyBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	OnEnemyLanded.ExecuteIfBound();
}

//===============================================================================================
// 전투관련
//===============================================================================================
void AEnemyBase::ApplyDamage_Implementation(float Damage, AActor* DamageCauser, const FVector& DamageLocation,
	const FVector& DamageImpulse)
{
	if (DamageCauser) LastDamageCauser = DamageCauser;
	Super::ApplyDamage_Implementation(Damage, DamageCauser, DamageLocation, DamageImpulse);
}

void AEnemyBase::NotifyDamage_Implementation(const FVector& DamageLocation, AActor* DamageSource)
{
	Super::NotifyDamage_Implementation(DamageLocation, DamageSource);
	if (DamageSource && DamageSource->ActorHasTag(TEXT("Player")))
	{
		LastDangerLocation = DamageLocation;
		LastDangerTime = GetWorld()->GetTimeSeconds();
	}
	if (EnemyDefinition && EnemyDefinition->HitMontage)
	{
		PlayAnimMontage(EnemyDefinition->HitMontage);
	}
}

void AEnemyBase::HandleDeath_Implementation()
{
	if (bRewardGiven) return;
	bRewardGiven = true;
	GiveRewardToKiller();
	OnEnemyDied.Broadcast();
	if (EnemyDefinition && EnemyDefinition->DeathMontage)
	{
		PlayAnimMontage(EnemyDefinition->DeathMontage);
	}
	Super::HandleDeath_Implementation();
}

void AEnemyBase::GiveRewardToKiller()
{
	if (!EnemyDefinition) return;
	AActor* Killer = LastDamageCauser.Get();
	if (!Killer) return;
	APlayerCharacter* Player = Cast<APlayerCharacter>(Killer);

	if (!Player && Killer->GetOwner())
	{
		Player = Cast<APlayerCharacter>(Killer->GetOwner());
	}
	if (!Player) return;
	
	if (UGoldComponent* GoldComponent = Player->GetGoldComponent())
	{
		if (EnemyDefinition->GoldReward > 0)
		{
			GoldComponent->AddGold(EnemyDefinition->GoldReward);
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

void AEnemyBase::DoAttackByRowName(FName AttackRowName)
{
	if (!EnemyDefinition || AttackRowName.IsNone() || !CombatBaseComponent || IsDead()) return;
	
	CombatBaseComponent->RequestAttackByRow(AttackRowName);
}

void AEnemyBase::DoMainAttack()
{
	if (!EnemyDefinition)
	{
		return;
	}

	DoAttackByRowName(EnemyDefinition->AttackSet.MainAttackRowName);
}

void AEnemyBase::DoSubAttack()
{
	if (!EnemyDefinition) return;
	DoAttackByRowName(EnemyDefinition->AttackSet.SubAttackRowName);
}

void AEnemyBase::AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	OnAttackCompleted.ExecuteIfBound();
}
//===============================================================================================
// Getter함수
//===============================================================================================
float AEnemyBase::GetCurrentHP() const
{
	if (!GetStatComponent())
	{
		return 0.f;
	}

	return GetStatComponent()->GetCurrentHP();
}

bool AEnemyBase::IsDead() const
{
	if (!GetStatComponent())
	{
		return true;
	}

	return GetStatComponent()->IsDead();
}

const FVector& AEnemyBase::GetLastDangerLocation() const
{
	return LastDangerLocation;
}

float AEnemyBase::GetLastDangerTime() const
{
	return LastDangerTime;
}

const FVector& AEnemyBase::GetHomeLocation() const
{
	return HomeLocation;
}
