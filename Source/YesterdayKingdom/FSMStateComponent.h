// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FSMStateComponent.generated.h"

class AEnemyBase;
class UEnemyDefinition;
class UEnemyFSMControllerComponent;

//===============================================================================================
// State 한개의 BaseComponent
// Idle, Chase등의 state를 만드는 component
//===============================================================================================
UCLASS( Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API UFSMStateComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<AEnemyBase> OwnerCharacter;
	
	UPROPERTY()
	TObjectPtr<UEnemyFSMControllerComponent> FSMController;
	
	UPROPERTY()
	TObjectPtr<UEnemyDefinition> EnemyDefinition;
public:	
	// Sets default values for this component's properties
	UFSMStateComponent();

protected:
	APawn* GetTargetPlayer() const;
	float GetDistanceToTarget(AActor* TargetActor) const;
	float GetDistanceToPlayer() const;
	float GetDistance2DToPlayer() const;
	float GetDistanceToHomeLocation() const;
	
	bool IsPlayerValid() const;
	bool IsPlayerInDetectRange() const;
	bool IsPlayerInAttackRange() const;
	bool IsPlayerLost(float LoseTargetMultiplier = 1.5f) const;
	bool IsNearHomeLocation(float AcceptanceRadius) const;
	bool IsTooFarFromHome(float MaxDistance) const;
	
	// 상태 체크
	bool IsOwnerDead() const;
	
	// AI 이동
	void MoveToTarget(AActor* TargetActor, float AcceptanceRadius = 80.f);
	void MoveToPlayer(float AcceptanceRadius = 80.f);
	void MoveToLocation(const FVector& TargetLocation, float AcceptanceRadius, bool bCanStrafe = false);
	void StopMove();
	// AI 초점
	UFUNCTION(BlueprintCallable, Category="Enemy|AI")
	void SetFocusToPlayer();

	UFUNCTION(BlueprintCallable, Category="Enemy|AI")
	void SetFocusTarget(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category="Enemy|AI")
	void ClearFocusTarget();
public:	
	// Called when the game starts
	virtual void BeginPlay() override;
	// ========================================================
	// 초기화
	// ========================================================
	virtual void InitializeState(AEnemyBase* InOwnerEnemy, UEnemyFSMControllerComponent* InFSMOwner);
	
	// ========================================================
	// 상태 변경 관련 메소드
	// ========================================================
	// 상태 초기화 처리 메소드 (현재 상태 전이시 1회 호출)
	virtual void OnStateEnter();

	// 상태 진행 처리 메소드 (현재 상태 전이 후 Tick과 동일하게 호출)
	virtual void OnStateUpdate(float);

	// 상태 종료 처리 메소드 (현재 상태에서 다른 상태 전이시 1회 호출)
	virtual void OnStateExit();
	
	// ========================================================
	// Getter
	// ========================================================
	AEnemyBase* GetOwnerEnemy() const;
	UEnemyFSMControllerComponent* GetFSMController() const;

};
