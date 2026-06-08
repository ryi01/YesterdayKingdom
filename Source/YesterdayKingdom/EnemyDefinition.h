// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCombatTypes.h"
#include "Engine/DataAsset.h"
#include "EnemyDefinition.generated.h"

class UAnimMontage;
class UStateTree;
class UStaticMesh;

USTRUCT(BlueprintType)
struct FEnemyAttackSet
{
	GENERATED_BODY()

	// 일반 공격 / 기본 콤보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	FName MainAttackRowName;

	// 엘리트용 보조 공격 / 강공격 / 단타 공격
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	FName SubAttackRowName;

	// SubAttack을 사용할 확률
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat", meta=(ClampMin="0.0", ClampMax="1.0"))
	float SubAttackChance = 0.f;
};


USTRUCT(BlueprintType)
struct FBossAttackPattern
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	FName AttackRowName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float Weight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float Cooldown = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float MinRange = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float MaxRange = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	int32 MinPhase = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	int32 MaxPhase = 1;
};
USTRUCT(BlueprintType)
struct FEnemyFSMStateTime
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Time", meta = (ClampMin = "0.0"))
	float Min = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Time", meta = (ClampMin = "0.0"))
	float Max = 0.8f;
	FEnemyFSMStateTime() {}
	FEnemyFSMStateTime(float InMin, float InMax) : Min(InMin), Max(InMax){}
	float GetRandomTime() const
	{
		return FMath::FRandRange(FMath::Min(Min, Max), FMath::Max(Min, Max));
	}
};
USTRUCT(BlueprintType)
struct FEnemyFSMTimeConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Time")
	FEnemyFSMStateTime IdleTime = FEnemyFSMStateTime(0.3f, 0.8f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Time")
	FEnemyFSMStateTime AttackEndDelayTime = FEnemyFSMStateTime(0.2f, 0.5f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Time")
	FEnemyFSMStateTime CooldownTime = FEnemyFSMStateTime(0.8f, 1.5f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Time")
	FEnemyFSMStateTime BackStepTime = FEnemyFSMStateTime(0.3f, 0.6f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Time")
	FEnemyFSMStateTime HitTime = FEnemyFSMStateTime(0.3f, 0.5f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Time")
	FEnemyFSMStateTime IntroTime = FEnemyFSMStateTime(1.5f, 1.5f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Time")
	FEnemyFSMStateTime PhaseChangeTime = FEnemyFSMStateTime(2.0f, 2.5f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Time")
	FEnemyFSMStateTime GroggyTime = FEnemyFSMStateTime(3.0f, 4.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Time")
	FEnemyFSMStateTime DeathDestroyDelayTime = FEnemyFSMStateTime(2.5f, 3.5f);
};

// 아이템 전용 구조체
USTRUCT(BlueprintType)
struct FEnemyItemReward
{
	GENERATED_BODY()
	
	// DT_ItemData의 RowName
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
	FName ItemRowName = NAME_None;

	// 최소 드랍 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward", meta = (ClampMin = "1"))
	int32 MinCount = 1;

	// 최대 드랍 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward", meta = (ClampMin = "1"))
	int32 MaxCount = 1;

	// 드랍 확률 0.0 ~ 1.0
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DropChance = 1.0f;
	
};
UCLASS()
class YESTERDAYKINGDOM_API UEnemyDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// 기본 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName EnemyID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText EnemyName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEnemyAttackType AttackType = EEnemyAttackType::Melee;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEnemyRole EnemyRole = EEnemyRole::Normal;
	
	// DataTable 연결
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName StatRowName;
	
	// 외형
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAnimInstance> AnimBP;
	
	// 무기
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> WeaponMesh;
	
	// 보상
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reward")
	int32 GoldReward = 0;
	// 아이템 보상 추가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
	TArray<FEnemyItemReward> ItemRewards;
	
	// AI
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStateTree> StateTree;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CombatMoveSpeed = 350.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RotationSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DetectRange = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackRange = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PatrolRadius = 800.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|AI")
	float ReturnRadius = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|AI|FSM")
	FEnemyFSMTimeConfig FSMTimeConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Puppet")
	float ReviveDelay = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Puppet", meta=(ClampMin="0.0", ClampMax="1.0"))
	float ReviveHPPercent = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Puppet")
	bool bReviveUntilMasterDead = true;
	
	// 전투
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Combat")
	TObjectPtr<UDataTable> AttackDataTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Combat")
	FEnemyAttackSet AttackSet;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Combat|Boss")
	TArray<FBossAttackPattern> BossAttackPatterns;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Animation")
	TObjectPtr<UAnimMontage> HitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Animation")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Animation")
	TObjectPtr<UAnimMontage> DownMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Animation")
	TObjectPtr<UAnimMontage> ReviveMontage;
	
};
