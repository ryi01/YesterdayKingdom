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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0.0"))
	float GuardMoveSpeedMultiplier = 0.5f;

public:
	UPlayerCombatComponent();
	
private:
	bool TryGetAttackRowName(EAttackType AttackType, FName& OutRowName) const;
protected:
	virtual void OnChargeAttackStarted() override;
	virtual void OnGuardStarted() override;
	virtual void OnGuardHit(AActor* DamageCauser) override;
	virtual void OnGuardEnded() override;
	//===============================================================================================
	// 공격 보정을 위한 함수
	//===============================================================================================
	void FaceBestTarget();
	UFUNCTION()
	AActor* FindBestTarget() const;
public:
	virtual void BeginAttackTrace() override;

	UFUNCTION(BlueprintCallable, Category="Combat|Player")
	void RequestAttack(EAttackType AttackType);

	UFUNCTION(BlueprintCallable, Category="Combat|Player")
	void StartChargeAttack();
	

	
};
