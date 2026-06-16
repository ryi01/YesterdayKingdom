// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "BossEnemy.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;
class UAudioComponent;
class UEnemyFSMControllerComponent;
class UChaseStateComponent;
class UIdleStatComponent;

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
	// 보스 페이즈 2 이펙트 
	//===============================================================================================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Phase FX")
	TObjectPtr<UNiagaraSystem> Phase2AuraFX;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Phase FX")
	FName Phase2AuraFXSocketName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Phase FX")
	FVector Phase2AuraFXOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Phase FX")
	FVector Phase2AuraFXScale = FVector(1.f, 1.f, 1.f);
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> Phase2AuraFXComponent;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Phase FX")
	TObjectPtr<UNiagaraSystem> Phase2CrackleFX;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Phase FX")
	FName Phase2CrackleFXSocketName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Phase FX")
	FVector Phase2CrackleFXOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Phase FX")
	FVector Phase2CrackleFXScale = FVector(1.f, 1.f, 1.f);

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> Phase2CrackleFXComponent;
	//===============================================================================================
	// Phase 2 Persistent Sound
	//===============================================================================================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Phase Sound")
	TObjectPtr<USoundBase> Phase2LoopSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Phase Sound")
	float Phase2LoopSoundVolume = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Phase Sound")
	float Phase2LoopSoundFadeInTime = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Phase Sound")
	float Phase2LoopSoundFadeOutTime = 0.3f;

	UPROPERTY()
	TObjectPtr<UAudioComponent> Phase2LoopAudioComponent;
	
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
	TObjectPtr<class UAttackBossStateComponent> AttackState;
	
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
	TObjectPtr<class UPhaseChangeStateComponent> PhaseChangeState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<class UHitStateComponent> HitState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM|State")
	TObjectPtr<class UDeadStateComponent> DeadState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Boss|Phase")
	bool bUsePhase2HitMontage = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	TObjectPtr<class UAnimMontage> Phase2HitMontage;
public:
	ABossEnemy(const FObjectInitializer& ObjectInitializer);

protected:
	void StartPhase2PersistentEffect();
	void StopPhase2PersistentEffect();
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void FinishPhaseChange() override;
	virtual void HandleDeath_Implementation() override;
};
