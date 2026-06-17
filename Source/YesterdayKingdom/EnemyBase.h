// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyBase.generated.h"

class UWidgetComponent;
class UEnemyDefinition;
class UEnemyFSMControllerComponent;
DECLARE_DELEGATE(FOnEnemyAttackCompleted);
DECLARE_DELEGATE(FOnEnemyLanded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoEnemyDied);
//===============================================================================================
// Enemy 전용으로 빼는 용도
//===============================================================================================
UCLASS(Abstract)
class YESTERDAYKINGDOM_API AEnemyBase : public ABaseCharacter
{
	GENERATED_BODY()
	
	float PatternSelectBlockedUntilTime = 0.f;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|FSM")
	TObjectPtr<UEnemyFSMControllerComponent> FSMController;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Data")
	TObjectPtr<UEnemyDefinition> EnemyDefinition;
	
	UPROPERTY()
	TObjectPtr<AActor> LastDamageCauser;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName QuestTargetID = NAME_None;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Reward")
	bool bRewardGiven = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Combat")
	bool bIsAttacking = false;

	FVector LastDangerLocation = FVector::ZeroVector;
	float LastDangerTime = -1000.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|AI")
	FVector HomeLocation = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|Combat")
	FName SelectedAttackRowName = NAME_None;
	//===============================================================================================
	// 페이지 변경
	//===============================================================================================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	bool bUsePhaseSystem = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	int32 CurrentPhase = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	int32 MaxPhase = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	bool bIsPhaseChanging = false;
	//===============================================================================================
	// hp 바
	//===============================================================================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UWidgetComponent> EnemyHPWidgetComponent;
	UPROPERTY()
	TObjectPtr<class UEnemyHPWidget> EnemyHPWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hit")
	bool bHit = false;
	FTimerHandle HitTimerHandle;
	//===============================================================================================
	// 카메라 쉐이크
	//===============================================================================================
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Shake|Footstep")
	TSubclassOf<UCameraShakeBase> FootstepCameraShake;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Shake|Footstep",meta = (ClampMin = "0.0"))
	float FootstepShakeScale = 0.18f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Shake|Footstep",meta = (ClampMin = "0.0"))
	float FootstepInnerRadius = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Shake|Footstep",meta = (ClampMin = "0.0"))
	float FootstepOuterRadius = 1200.f;
public:
	AEnemyBase(const FObjectInitializer& ObjectInitializer);
	
	FOnMontageEnded OnAttackMontageEnded;
	FOnEnemyAttackCompleted OnAttackCompleted;
	FOnEnemyLanded OnEnemyLanded;

	UPROPERTY(BlueprintAssignable, Category = "Enemy|Events")
	FOnDoEnemyDied OnEnemyDied;

protected:

	virtual void InitializeFromDefinition();

	//===============================================================================================
	// 죽었을 경우 리워드 지급
	//===============================================================================================
	virtual void GiveRewardToKiller();
	void NotifyQuestKillToKiller();

public:
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;
	void SetHomeLocation(const FVector& InHomeLocation);
	//===============================================================================================
	// 데미지를 입는것
	//===============================================================================================
	virtual void ApplyDamage_Implementation(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse, EHitReactionType HitReactionType) override;
	virtual void NotifyDamage_Implementation(const FVector& DamageLocation, AActor* DamageSource) override;
	virtual void HandleDeath_Implementation() override;
	virtual UAnimMontage* GetHitMontage() const;
	
	//===============================================================================================
	// 죽는 상태
	//===============================================================================================
	virtual float GetDeathDestroyDelay() const override;
	
	//===============================================================================================
	// 전투관련
	//===============================================================================================
	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	virtual void DoAttackByRowName(FName AttackRowName);

	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	virtual void DoMainAttack();

	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	virtual void DoSubAttack();

	virtual void ClearAttackAnimation_Implementation() override;
	virtual void AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	

	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	void SetSelectedAttackRowName(FName InAttackRowName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy|Combat")
	FName GetSelectedAttackRowName() const;

	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	void ClearSelectedAttackRowName();
	
	//===============================================================================================
	// 속도 변경
	//===============================================================================================
	UFUNCTION(BlueprintCallable, Category="Enemy|Movement")
	void SetMoveSpeed(float NewSpeed);

	UFUNCTION(BlueprintCallable, Category="Enemy|Movement")
	void SetDefaultMoveSpeed();

	UFUNCTION(BlueprintCallable, Category="Enemy|Movement")
	void SetCombatMoveSpeed();
	
	//===============================================================================================
	// 꼭두각시 인형 관련
	//===============================================================================================
	UFUNCTION(BlueprintCallable, Category="Enemy|Animation")
	void DownMontage();

	UFUNCTION(BlueprintCallable, Category="Enemy|Animation")
	void ReviveMontage();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy|Animation")
	bool IsAnyMontagePlaying() const;

	//===============================================================================================
	// 페이지 변경
	//===============================================================================================
	int32 GetCurrentPhase() const { return CurrentPhase; }

	bool TryStartNextPhase();

	virtual void FinishPhaseChange();
	
	//===============================================================================================
	// hp 바
	//===============================================================================================
	
	void InitializeEnemyHPWidget();
	void RefreshEnemyHPWidget();

	UFUNCTION()
	void OnEnemyHPChanged(float CurrentHP, float MaxHP);
	void SetEnemyHPWidgetVisible(bool bVisible);
	
	void SetHit();
	//===============================================================================================
	// 카메라 쉐이크
	//===============================================================================================
	void PlayFootstepCameraShake();
	
	//===============================================================================================
	// Getter함수
	//===============================================================================================
	UEnemyDefinition* GetEnemyDefinition() const { return EnemyDefinition; }
	
	UEnemyFSMControllerComponent* GetFSMControllerComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy|Stat")
	float GetCurrentHP() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy|Stat")
	bool IsDead() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy|Stat")
	bool IsAttacking() const;
	
	const FVector& GetLastDangerLocation() const;
	
	float GetLastDangerTime() const;
	
	const FVector& GetHomeLocation() const;
	
	void BlockPatternSelect(float Duration);
	bool IsPatternSelectBlocked() const;
	
	bool GetIsHit() const;
};
