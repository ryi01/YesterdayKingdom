// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "BossEnemy.generated.h"

class UEnemyFSMControllerComponent;
class UChaseStateComponent;
class UIdleStatComponent;
/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API ABossEnemy : public AEnemyBase
{
	GENERATED_BODY()
protected:
	//===============================================================================================
	// 무기
	//===============================================================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;
	//===============================================================================================
	// FSM
	//===============================================================================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<UIdleStatComponent> IdleState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<UChaseStateComponent> ChaseState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<class UReturnStateComponent> ReturnState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<class UPatrolStateComponent> PatrolState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<class UPatternSelectStateComponent> PatternSelectState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<class UAttackStateComponent> AttackState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<class UCooldownStateComponent> CooldownState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<class UFlankingStateComponent> FlankingState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<class UBackStepStateComponent> BackStepState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<class UJumpAttackStateComponent> JumpAttackState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	TObjectPtr<class URotationAttackStateComponent> RotationAttackState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<class UHitStateComponent> HitState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<class UDeadStateComponent> DeadState;
public:
	ABossEnemy(const FObjectInitializer& ObjectInitializer);

protected:
	

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

};
