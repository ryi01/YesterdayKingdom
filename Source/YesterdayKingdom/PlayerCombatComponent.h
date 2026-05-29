// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatBaseComponent.h"
#include "PlayerCombatComponent.generated.h"

UCLASS()
class YESTERDAYKINGDOM_API UPlayerCombatComponent : public UCombatBaseComponent
{
	GENERATED_BODY()

protected:
	//===============================================================================================
	// 공격 관련 데이터
	//===============================================================================================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	TMap<EAttackType, FName> PlayerAttackRows;
	
	//===============================================================================================
	// 공격 보정을 위한 함수
	//===============================================================================================
	void FaceBestTarget();
	UFUNCTION()
	AActor* FindBestTarget() const;
	
public:
	virtual void BeginAttackTrace() override;
	
	void RequestAttack(EAttackType AttackType);
};
