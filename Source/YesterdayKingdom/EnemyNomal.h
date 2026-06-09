// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyBase.h"
#include "EnemyNomal.generated.h"

class UEnemyDefinition;
class UAnimMontage;

UCLASS()
class YESTERDAYKINGDOM_API AEnemyNomal : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyNomal(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
	bool bIsAttacking = false;

	FOnMontageEnded OnAttackMontageEnded;
	FOnEnemyAttackCompleted OnAttackCompleted;
	FOnEnemyLanded OnEnemyLanded;

	FVector LastDangerLocation = FVector::ZeroVector;
	float LastDangerTime = -1000.f;

	void DoAIComboAttack();

	// 차지 공격은 아직 미사용이면 일단 선언만 유지
	void DoAIChargedAttack();
	
	UFUNCTION()
	void NotifyAttackEnded();

protected:
	virtual void Landed(const FHitResult& Hit) override;
	virtual void HandleDeath_Implementation() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMeshComponent;
};
