// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseStatComponent.generated.h"

#pragma region Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSTChanged, float, CurrentST, float, MaxST);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMPChanged, float, CurrentMP, float, MaxMP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStunChanged, float, CurrentStun, float, MaxStun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStunned);
#pragma endregion
class UDataTable;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API UBaseStatComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	TObjectPtr<UDataTable> StatDT;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	FName StatRowName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float MaxHP = 100.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentHP = 100.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float MaxST = 100.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentST = 100.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float MaxMP = 100.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentMP = 100.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float Attack = 10.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float Defense = 5.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float MoveSpeed = 450.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float RunSpeed = 800.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CrouchSpeed = 350.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float MaxStun = 50.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentStun = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	bool bIsDead = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	bool bIsStun = false;
	
	UPROPERTY()
	float LastSTConsumeTime = -999.f;
	UPROPERTY()
	float LastMPConsumeTime = -999.f;

	// ========================================================
	// 버프 관련
	// ========================================================
	UPROPERTY(BlueprintReadOnly, Category="Stat|Buff")
	float BuffAttackBonus = 0.f;

	// 방어력 버프
	UPROPERTY(BlueprintReadOnly, Category="Stat|Buff")
	float BuffDefenseBonus = 0.f;

	// ========================================================
	// Equipment Bonus
	// ========================================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Equipment")
	float EquipmentAttackBonus = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Equipment")
	float EquipmentDefenseBonus = 0.f;

	// ========================================================
	// 스킬트리 관련
	// ========================================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stat|Skill")
	float SkillBonusAttack = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stat|Skill")
	float SkillBonusDefense = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stat|Skill")
	float SkillBonusMaxHP = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stat|Skill")
	float SkillBonusMaxMP = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stat|Skill")
	float SkillBonusMaxST = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stat|Skill")
	float SkillBonusMoveSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stat|Skill")
	float SkillBonusRunSpeed = 0.f;
	
public:	
	// Sets default values for this component's properties
	UBaseStatComponent();

protected:


public:	
	virtual void BeginPlay() override;
	// ========================================================
	// Event
	// ========================================================
	UPROPERTY(BlueprintAssignable, Category = "Stat")
	FOnHPChanged OnHPChanged;
	UPROPERTY(BlueprintAssignable, Category = "Stat")
	FOnSTChanged OnSTChanged;
	UPROPERTY(BlueprintAssignable, Category = "Stat")
	FOnMPChanged OnMPChanged;
	UPROPERTY(BlueprintAssignable, Category = "Stat")
	FOnStunChanged OnStunChanged;
	UPROPERTY(BlueprintAssignable, Category = "Stat")
	FOnDead OnDead;
	UPROPERTY(BlueprintAssignable, Category = "Stat")
	FOnStunned OnStunned;
	
	// ========================================================
	// 기타 함수들
	// ========================================================
	UFUNCTION(BlueprintCallable)
	void SetCurrentHP(float NewHP);
	UFUNCTION(BlueprintCallable)
	void InitializeStat(UDataTable* InStatTable, FName InRowName);
	UFUNCTION(BlueprintCallable)
	float ApplyDamage(float Amount);
	UFUNCTION(BlueprintCallable)
	void Heal(float Amount);
	UFUNCTION(BlueprintCallable)
	bool ConsumeST(float Amount);
	UFUNCTION(BlueprintCallable)
	void RecoverST(float Amount);
	UFUNCTION(BlueprintCallable)
	bool ConsumeMP(float Amount);
	UFUNCTION(BlueprintCallable)
	void RecoverMP(float Amount);
	UFUNCTION(BlueprintCallable)
	void AddStun(float Amount);
	UFUNCTION(BlueprintCallable)
	void ResetStun();
	UFUNCTION(BlueprintCallable)
	bool IsDead() const;
	UFUNCTION(BlueprintCallable)
	bool IsStunned() const;

	// ========================================================
	// 버프 관련
	// ========================================================
	UFUNCTION(BlueprintCallable, Category="Stat|Buff")
	void AddBuffAttack(float Value);

	UFUNCTION(BlueprintCallable, Category="Stat|Buff")
	void AddBuffDefense(float Value);
	
	UFUNCTION(BlueprintCallable, Category="Stat|Buff")
	void AddBonusMoveSpeed(float Value);

	UFUNCTION(BlueprintCallable, Category="Stat|Buff")
	void ClearBuffAttack();

	UFUNCTION(BlueprintCallable, Category="Stat|Buff")
	void ClearBuffDefense();

	UFUNCTION(BlueprintCallable, Category="Stat|Buff")
	void ClearAllBuffStats();
	
	// ========================================================
	// Equipment Bonus
	// ========================================================
	UFUNCTION(BlueprintCallable, Category = "Stat|Equipment")
	void SetEquipmentBonus(float InAttackBonus, float InDefenseBonus);
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetFinalAttack() const;
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetFinalDefense() const;
	
	// ========================================================
	// Skill Stat
	// ========================================================
	UFUNCTION(BlueprintCallable, Category="Stat|Skill")
	void AddSkillAttack(float Amount);

	UFUNCTION(BlueprintCallable, Category="Stat|Skill")
	void AddSkillDefense(float Amount);

	UFUNCTION(BlueprintCallable, Category="Stat|Skill")
	void AddSkillMaxHP(float Amount);

	UFUNCTION(BlueprintCallable, Category="Stat|Skill")
	void AddSkillMaxMP(float Amount);

	UFUNCTION(BlueprintCallable, Category="Stat|Skill")
	void AddSkillMaxST(float Amount);

	UFUNCTION(BlueprintCallable, Category="Stat|Skill")
	void AddSkillMoveSpeed(float Amount);

	UFUNCTION(BlueprintCallable, Category="Stat|Skill")
	void AddSkillRunSpeed(float Amount);

	UFUNCTION(BlueprintCallable, Category="Stat|Skill")
	void ClearAllSkillStats();
	// ========================================================
	// Setter 
	// ========================================================
	UFUNCTION()
	void SetStatRowName(FName NewName);
	// ========================================================
	// Getter 
	// ========================================================
	UDataTable* GetStatusDT() const;
	UFUNCTION(BlueprintPure)
	float GetCurrentHP() const;
	UFUNCTION(BlueprintPure)
	float GetMaxHP() const;
	UFUNCTION(BlueprintPure)
	float GetCurrentST() const;
	UFUNCTION(BlueprintPure)
	float GetMaxST() const;
	UFUNCTION(BlueprintPure)
	float GetCurrentMP() const;
	UFUNCTION(BlueprintPure)
	float GetMaxMP() const;
	UFUNCTION(BlueprintPure)
	float GetAttack() const;
	UFUNCTION(BlueprintPure)
	float GetDefense() const;
	UFUNCTION(BlueprintPure)
	float GetMoveSpeed() const;
	UFUNCTION(BlueprintPure)
	float GetCrouchMoveSpeed() const;
	UFUNCTION(BlueprintPure)
	float GetRunSpeed() const;
	UFUNCTION(BlueprintPure)
	float GetCurrentStun() const;
	UFUNCTION(BlueprintPure)
	float GetLastSTConsumeTime() const;
	UFUNCTION(BlueprintPure)
	float GetLastMPConsumeTime() const;
};
