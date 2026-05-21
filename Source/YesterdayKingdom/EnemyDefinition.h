// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCombatTypes.h"
#include "Engine/DataAsset.h"
#include "EnemyDefinition.generated.h"

class UAnimMontage;
class UStateTree;
class USkeletalMesh;
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
	
	// 전투
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> HitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> DeathMontage;
};
