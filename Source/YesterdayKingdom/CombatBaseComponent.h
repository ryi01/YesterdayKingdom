// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CommonEnumTypes.h" 
#include "CombatBaseComponent.generated.h"

class ABaseCharacter;
class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnded);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API UCombatBaseComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<ABaseCharacter> OwnerCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttackTracing = false;
	
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> HitActors;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Trace")
	float TraceDistance = 180.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Trace")
	float TraceRadius = 45.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Trace")
	float TraceAngle = 70.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Trace")
	float TraceHeight = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Damage")
	float DefaultDamage = 10.f;
	
	UPROPERTY()
	bool bComboInputBuffered = false;
	// enemy 전용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat|Combo")
	bool bAutoCombo = false;
	
	//===============================================================================
	// 공격 DT
	//===============================================================================
	UPROPERTY(EditAnywhere, Category="Combat|Data")
	TObjectPtr<UDataTable> AttackDataTable;

	UPROPERTY(VisibleInstanceOnly, Category="Combat|Runtime")
	FName CurrentAttackRowName;
	
	UPROPERTY()
	int32 CurrentAttackNodeIndex = INDEX_NONE;

	FTimerHandle HitStopTimerHandle;
	
	//===============================================================================
	// 차지 공격 
	//===============================================================================

	UPROPERTY(BlueprintReadOnly, Category="Combat|Charge")
	bool bIsCharging = false;

	UPROPERTY(BlueprintReadOnly, Category="Combat|Charge")
	float ChargeStartTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Combat|Charge")
	float CurrentChargeRatio = 1.f;

	UPROPERTY(BlueprintReadOnly, Category="Combat|Charge")
	FName CurrentChargeRowName = NAME_None;
	//===============================================================================
	// 가드 
	//===============================================================================
	UPROPERTY(EditDefaultsOnly, Category="Combat|Guard")
	TObjectPtr<UAnimMontage> ParrySuccessMontage;
	
	UPROPERTY(BlueprintReadOnly, Category="Combat|Guard")
	bool bIsGuarding = false;

	UPROPERTY(BlueprintReadOnly, Category="Combat|Guard")
	bool bCanParry = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Guard")
	float ParryWindow = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Guard")
	float GuardDamageRate = 0.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Guard")
	float GuardSTCostPerHit = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Guard")
	float ParrySTRecover = 10.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Guard|Parry")
	bool bUseParryHitStop = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Guard|Parry")
	float ParryHitStopDuration = 0.02f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Guard|Parry")
	float ParryHitStopTimeScale = 0.25f;

	FTimerHandle ParryTimerHandle;
	
public:	
	// Sets default values for this component's properties
	UCombatBaseComponent();
	
	UPROPERTY(BlueprintAssignable, Category = "Combat|Event")
	FOnAttackEnded OnAttackEnded;
	
protected:
	//=====================================================================================================
	// 차지 공격
	//=====================================================================================================
	virtual void OnChargeAttackStarted();
	virtual void OnChargeAttackReleased();
	
	void UpdateCharge(float DeltaTime);
	
	float CalculateChargeRatio(const FAttackDataRow* AttackDataRow) const;
	
	//===============================================================================
	// 가드 
	//===============================================================================
	void ApplyParryHitStop();
	
	virtual bool CanStartGuard() const;
	virtual bool IsGuardDirectionValid(AActor* DamageCauser) const;

	virtual void OnGuardStarted();
	virtual void OnGuardEnded();
	virtual void OnParrySuccess(AActor* DamageCauser);

	void CloseParryWindow();
	
	//=====================================================================================================
	// 기타 함수
	//=====================================================================================================
	virtual bool IsValidHitActor(AActor* HitActor) const;
	const FAttackNodeData* GetCurrentAttackNodeData() const;
	
	//=====================================================================================================
	// Hit시 발생되는 효과
	//=====================================================================================================
	virtual void ApplyAttackHit(AActor* HitActor, const FHitResult& HitResult);
	void ApplyHitFeedback(const FHitFeedbackData& Feedback, AActor* HitActor);
	void ResetHitStop();
	
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
public:	
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//=====================================================================================================
	// 공격 시 타겟 탐색
	//=====================================================================================================
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void BeginAttackTrace();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void DoAttackTrace();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void EndAttackTrace();
	
	//=====================================================================================================
	// 콤보 공격
	//=====================================================================================================
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void CheckCombo();
	
	//=====================================================================================================
	// 차지 공격
	//=====================================================================================================
	UFUNCTION(BlueprintCallable, Category="Combat|Charge")
	virtual bool StartChargeAttackByRow(FName AttackRowName);
	UFUNCTION(BlueprintCallable, Category="Combat|Charge")
	virtual void CancelChargeAttack();
	UFUNCTION(BlueprintCallable, Category = "Combat|Charge")
	virtual void ReleaseChargeAttack();
	
	//===============================================================================
	// 가드 
	//===============================================================================
	UFUNCTION(BlueprintCallable, Category="Combat|Guard")
	virtual void StartGuard();
	UFUNCTION(BlueprintCallable, Category="Combat|Guard")
	virtual void EndGuard();
	UFUNCTION(BlueprintCallable, Category="Combat|Guard")
	bool TryHandleGuardOrParry(float& InOutDamage, AActor* DamageCauser);
	UFUNCTION(BlueprintCallable, Category="Combat|Guard")
	bool IsGuarding() const;
	UFUNCTION(BlueprintCallable, Category="Combat|Guard")
	bool CanParry() const;
	
	//=====================================================================================================
	// AttackRow 탐색
	//=====================================================================================================
	UFUNCTION(BlueprintCallable, Category="Combat")
	virtual void RequestAttackByRow(FName AttackRowName);
	
	//=====================================================================================================
	// 리셋 및 셋팅
	//=====================================================================================================
	void ResetAttackState();
	void SetAttackDataTable(UDataTable* NewTable);
	const FAttackDataRow* GetAttackDataByRow(FName AttackRowName) const;
	bool JumpToNextAttackSection();
	
	//=====================================================================================================
	// Getter
	//=====================================================================================================
	UFUNCTION(BlueprintPure, Category="Combat")
	bool IsAttacking() const;
	UFUNCTION(BlueprintPure, Category="Combat")
	bool IsCharging() const;
	const FAttackDataRow* GetCurrentAttackDataRow() const;
	FName GetCurrentAttackRowName() const;

};
