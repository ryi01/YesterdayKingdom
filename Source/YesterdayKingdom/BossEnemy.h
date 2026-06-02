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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM")
	TObjectPtr<UEnemyFSMControllerComponent> FSMController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<UIdleStatComponent> IdleState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<UChaseStateComponent> ChaseState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<class UReturnStateComponent> ReturnState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<class UPatrolStateComponent> PatrolState;
	
public:
	ABossEnemy(const FObjectInitializer& ObjectInitializer);

protected:
	

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

};
