// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyFSMTypes.h"
#include "Components/ActorComponent.h"
#include "EnemyFSMControllerComponent.generated.h"

class UFSMStateComponent;
class AEnemyBase;
//===============================================================================================
// 상태를 관리하는 컨트롤러 역할 
//===============================================================================================
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API UEnemyFSMControllerComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<AEnemyBase> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UFSMStateComponent> CurrentStateComponent;
	UPROPERTY()
	TMap<EEnemyFSMStateType, TObjectPtr<UFSMStateComponent>> StateMap;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	EEnemyFSMStateType CurrentStateType = EEnemyFSMStateType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	EEnemyFSMStateType PreviousStateType = EEnemyFSMStateType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	bool bIsRunning = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	float StateElapsedTime = 0.f;
	
public:	
	// Sets default values for this component's properties
	UEnemyFSMControllerComponent();

protected:
	UFSMStateComponent* FindState(EEnemyFSMStateType StateType) const;

public:	
	// Called when the game starts
	virtual void BeginPlay() override;
	// ========================================================
	// 기본 State 컨트롤 관련
	// ========================================================
	virtual void InitializeFSM(AEnemyBase* InOwnerEnemy);

	UFUNCTION(BlueprintCallable, Category = "FSM")
	virtual void RegisterState(EEnemyFSMStateType StateType, UFSMStateComponent* StateComponent);

	UFUNCTION(BlueprintCallable, Category = "FSM")
	virtual void StartFSM(EEnemyFSMStateType StartStateType);
	
	virtual void TickFSM(float DeltaTime);
	
	UFUNCTION(BlueprintCallable, Category = "FSM")
	virtual void StopFSM();
	
	UFUNCTION(BlueprintCallable, Category = "FSM")
	virtual void ChangeState(EEnemyFSMStateType NewStateType);
	// ========================================================
	// Getter
	// ========================================================
	UFSMStateComponent* GetCurrentStateComponent() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FSM")
	EEnemyFSMStateType GetCurrentStateType() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FSM")
	EEnemyFSMStateType GetPreviousStateType() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FSM")
	bool IsRunning() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FSM")
	AEnemyBase* GetOwnerEnemy() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FSM")
	float GetStateElapsedTime() const;
	
};
