// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatBaseComponent.h"
#include "PlayerCombatComponent.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UPlayerCombatComponent : public UCombatBaseComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<UAnimMontage*> ComboMontages;
	UPROPERTY()
	int32 AttackIndex = 0;
	
public:
	virtual void BeginAttackTrace();
	virtual void DoAttackTrace();
	virtual void EndAttackTrace();
	virtual void CheckCombo();
	
	virtual void RequestAttack(FName AttackRowName) override;

	
};
