// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyNomal.generated.h"

class UEnemyDefinition;

UCLASS()
class YESTERDAYKINGDOM_API AEnemyNomal : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemyNomal();

	UEnemyDefinition* GetEnemyDefinition() const { return EnemyDefinition; }

	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage();

	UFUNCTION(BlueprintCallable)
	void PlayHitMontage();

	UFUNCTION(BlueprintCallable)
	void PlayDeathMontage();

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
	virtual void BeginPlay() override;

	void InitializeFromDefinition();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Data")
	TObjectPtr<UEnemyDefinition> EnemyDefinition;
};
