// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "BaseStatComponent.h"
#include "CombatBaseComponent.h"
#include "EnemyDefinition.h"
#include "EnemyFSMControllerComponent.h"
#include "GoldComponent.h"
#include "InventoryComponent.h"
#include "PlayerCharacter.h"
#include "QuestComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyBase::AEnemyBase(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	FSMController = CreateDefaultSubobject<UEnemyFSMControllerComponent>(TEXT("FSMController"));
	
	OnAttackMontageEnded.BindUObject(this, &AEnemyBase::AttackMontageEnded);
	
	bDestroyOnDeath = true;
	DestroyDelay = 3.0f;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	InitializeFromDefinition();
}

void AEnemyBase::SetHomeLocation(const FVector& InHomeLocation)
{
	HomeLocation = InHomeLocation;
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

		if (!EnemyDefinition->StatRowName.IsNone())
		{
			StatComponent->SetStatRowName(EnemyDefinition->StatRowName);
		}
		if (EnemyDefinition->AttackDataTable)
		{
			CombatBaseComponent->SetAttackDataTable(EnemyDefinition->AttackDataTable);
		}
		
	}

	SetDefaultMoveSpeed();
	
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
	if (IsDead())
	{
		if (FSMController)
		{
			FSMController->ChangeState(EEnemyFSMStateType::Dead);
		}
		return;
	}
	if (DamageSource && DamageSource->ActorHasTag(TEXT("Player")))
	{
		LastDangerLocation = DamageLocation;
		LastDangerTime = GetWorld()->GetTimeSeconds();
	}
	// enemy hit state에서 발생
	/*if (EnemyDefinition && EnemyDefinition->HitMontage)
	{
		PlayAnimMontage(EnemyDefinition->HitMontage);
	}*/
	if (FSMController)
	{
		FSMController->ChangeState(EEnemyFSMStateType::Hit);
	}
}

void AEnemyBase::HandleDeath_Implementation()
{
	if (bRewardGiven) return;
	bRewardGiven = true;
	
	Super::HandleDeath_Implementation();
	
	GiveRewardToKiller();
	NotifyQuestKillToKiller();
	OnEnemyDied.Broadcast();

	if (FSMController && EnemyDefinition->EnemyRole != EEnemyRole::Normal)
	{
		FSMController->ChangeState(EEnemyFSMStateType::Dead);
	}
}

float AEnemyBase::GetDeathDestroyDelay() const
{
	if (EnemyDefinition && EnemyDefinition->DeathMontage)
	{
		return EnemyDefinition->DeathMontage->GetPlayLength();
	}

	return Super::GetDeathDestroyDelay();
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


void AEnemyBase::NotifyQuestKillToKiller()
{
	if (QuestTargetID.IsNone()) return;
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(LastDamageCauser);
	if (!PlayerCharacter) return;
	UQuestComponent* QuestComponent = PlayerCharacter->GetQuestComponent();
	if (!QuestComponent) return;
	QuestComponent->AddProgress(EQuestObjectiveType::KillEnemy, QuestTargetID, 1);
}



void AEnemyBase::DoAttackByRowName(FName AttackRowName)
{
	if (!EnemyDefinition || AttackRowName.IsNone() || !CombatBaseComponent || IsDead()) return;
	
	bIsAttacking = true;
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

void AEnemyBase::ClearAttackAnimation_Implementation()
{
	Super::ClearAttackAnimation_Implementation();

	if (!bIsAttacking) return;
	
	bIsAttacking = false;
	
	UE_LOG(LogTemp, Warning, TEXT("[EnemyBase] ClearAttackAnimation -> AttackCompleted : %s"),
		*GetName());

	OnAttackCompleted.ExecuteIfBound();
}

void AEnemyBase::AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	OnAttackCompleted.ExecuteIfBound();
}

void AEnemyBase::SetSelectedAttackRowName(FName InAttackRowName)
{
	SelectedAttackRowName = InAttackRowName;
	UE_LOG(LogTemp, Log, TEXT("[EnemyBase] SetSelectedAttackRowName : %s / RowName = %s"),
		*GetName(),
		*SelectedAttackRowName.ToString());
}

FName AEnemyBase::GetSelectedAttackRowName() const
{
	return SelectedAttackRowName;
}

void AEnemyBase::ClearSelectedAttackRowName()
{
	SelectedAttackRowName = NAME_None;
}
//===============================================================================================
// 속도 변경
//===============================================================================================
void AEnemyBase::SetMoveSpeed(float NewSpeed)
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement) return;

	Movement->MaxWalkSpeed = NewSpeed;
}

void AEnemyBase::SetDefaultMoveSpeed()
{
	if (!EnemyDefinition) return;

	SetMoveSpeed(EnemyDefinition->MoveSpeed);
}

void AEnemyBase::SetCombatMoveSpeed()
{
	if (!EnemyDefinition) return;

	SetMoveSpeed(EnemyDefinition->CombatMoveSpeed);
}

void AEnemyBase::DownMontage()
{
	if (EnemyDefinition && EnemyDefinition->DownMontage)
	{
		PlayAnimMontage(EnemyDefinition->DownMontage);
	}
}

void AEnemyBase::ReviveMontage()
{
	if (EnemyDefinition && EnemyDefinition->ReviveMontage)
	{
		PlayAnimMontage(EnemyDefinition->ReviveMontage);
	}
}

bool AEnemyBase::IsAnyMontagePlaying() const
{
	if (!GetMesh())
	{
		return false;
	}

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		return AnimInstance->IsAnyMontagePlaying();
	}

	return false;
}

//===============================================================================================
// Getter함수
//===============================================================================================

UEnemyFSMControllerComponent* AEnemyBase::GetFSMControllerComponent() const
{
	return FSMController;
}

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

bool AEnemyBase::IsAttacking() const
{
	return bIsAttacking;
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

void AEnemyBase::BlockPatternSelect(float Duration)
{
	PatternSelectBlockedUntilTime = GetWorld()->GetTimeSeconds() + Duration;
	UE_LOG(LogTemp, Warning,
		TEXT("[EnemyBase] BlockPatternSelect / Duration = %.2f"),
		Duration);
}

bool AEnemyBase::IsPatternSelectBlocked() const
{
	return GetWorld()->GetTimeSeconds() < PatternSelectBlockedUntilTime;
}
