// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyNomal.generated.h"

UCLASS()
class YESTERDAYKINGDOM_API AEnemyNomal : public ABaseCharacter
{
	GENERATED_BODY()
public:
	AEnemyNomal();
	
	class UEnemyDefinition* GetEnemyDefinition() const { return EnemyDefinition; }
	
	virtual void ApplyDamage_Implementation(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;
	virtual void HandleDeath_Implementation() override;
	virtual void NotifyDamage_Implementation(const FVector& DamageLocation, AActor* DamageSource) override;
	
	virtual void BeginAttackTrace_Implementation() override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Enemy|Data")
	TObjectPtr<class UEnemyDefinition> EnemyDefinition;
}; 
