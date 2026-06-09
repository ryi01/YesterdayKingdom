// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyElite.generated.h"

class UPatrolStateComponent;
class UEnemyFSMControllerComponent;
class UIdleStatComponent;
class UChaseStateComponent;
class UAttackStateComponent;
class UReturnStateComponent;
class UDownStateComponent;
class UReviveStateComponent;
class UDeadStateComponent;
class AEnemyPuppetMaster;

UCLASS()
class YESTERDAYKINGDOM_API AEnemyElite : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	AEnemyElite(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Puppet")
	TObjectPtr<AEnemyPuppetMaster> PuppetMaster;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Enemy|Puppet")
	bool IsPuppetMasterDead() const;
	
	UFUNCTION(BlueprintCallable, Category="Enemy|Puppet")
	void ForceTrueDeath();
	
	void SetPuppetMaster(AEnemyPuppetMaster* InMaster);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Landed(const FHitResult& Hit) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|Puppet")
	bool bPendingDownAfterLanded = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|Puppet")
	bool bTrueDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UIdleStatComponent> IdleState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UPatrolStateComponent> PatrolState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UChaseStateComponent> ChaseState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UAttackStateComponent> AttackState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UReturnStateComponent> ReturnState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UDownStateComponent> DownState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UReviveStateComponent> ReviveState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UDeadStateComponent> DeadState;
	
	virtual void HandleDeath_Implementation() override;
};
