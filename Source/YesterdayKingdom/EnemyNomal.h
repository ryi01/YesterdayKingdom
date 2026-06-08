// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyNomal.generated.h"

class UEnemyDefinition;
class UAnimMontage;

DECLARE_DELEGATE(FOnEnemyAttackCompleted);
DECLARE_DELEGATE(FOnEnemyLanded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDied);

UCLASS()
class YESTERDAYKINGDOM_API AEnemyNomal : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemyNomal(const FObjectInitializer& ObjectInitializer);
	
	UEnemyDefinition* GetEnemyDefinition() const { return EnemyDefinition; }

	bool bIsAttacking = false;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EnemyNomal|Stat")
	float GetCurrentHP() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EnemyNomal|Stat")
	bool IsDead() const;

	FOnMontageEnded OnAttackMontageEnded;
	FOnEnemyAttackCompleted OnAttackCompleted;
	FOnEnemyLanded OnEnemyLanded;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemyDied OnEnemyDied;

	FVector LastDangerLocation = FVector::ZeroVector;
	float LastDangerTime = -1000.f;

	void DoAIComboAttack();

	// 차지 공격은 아직 미사용이면 일단 선언만 유지
	void DoAIChargedAttack();

	void AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	const FVector& GetLastDangerLocation() const;
	float GetLastDangerTime() const;
	
	UFUNCTION()
	void NotifyAttackEnded();

protected:
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;

	void InitializeFromDefinition();

	virtual void ApplyDamage_Implementation(
		float Damage,
		AActor* DamageCauser,
		const FVector& DamageLocation,
		const FVector& DamageImpulse
	) override;

	virtual void NotifyDamage_Implementation(
		const FVector& DamageLocation,
		AActor* DamageSource
	) override;

	virtual void HandleDeath_Implementation() override;

	virtual void BeginAttackTrace_Implementation() override;
	virtual void DoAttackTrace_Implementation() override;
	virtual void EndAttackTrace_Implementation() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Data")
	TObjectPtr<UEnemyDefinition> EnemyDefinition;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMeshComponent;

	UPROPERTY()
	TObjectPtr<AActor> LastDamageCauser;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Reward")
	bool bRewardGiven = false;

protected:
	void GiveRewardToKiller();
};
