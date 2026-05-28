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
	//===============================================================================================
	// 공격 관련 DT
	//===============================================================================================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	TMap<EAttackType, FName> PlayerAttackRows;
	
	void FaceBestTarget();
	UFUNCTION()
	AActor* FindBestTarget();
public:
	virtual void BeginAttackTrace();
	
	void RequestAttack(EAttackType AttackType);
};
