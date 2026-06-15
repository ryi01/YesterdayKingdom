// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attacker.h"
#include "CommonEnumTypes.h"
#include "Damagable.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UBaseStatComponent;
class UCombatBaseComponent;


UCLASS(Abstract)
class YESTERDAYKINGDOM_API ABaseCharacter : public ACharacter, public IDamagable, public IAttacker
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// 스테이터스 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat Component")
	TObjectPtr<UBaseStatComponent> StatComponent;
	// 전투 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat Component")
	TObjectPtr<UCombatBaseComponent> CombatBaseComponent;
	
	// ========================================================
	// 무기 관련
	// ========================================================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class USceneComponent> WeaponRoot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName WeaponSocketName = TEXT("Weapon_RSocket");
	// ========================================================
	// 패링
	// ========================================================
	UPROPERTY(EditDefaultsOnly, Category="Combat|Reaction")
	TObjectPtr<UAnimMontage> ParriedMontage;
	
	// ========================================================
	// 죽기
	// ========================================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Death")
	bool bIsDead = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	bool bDestroyOnDeath = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	float DestroyDelay = 3.0f;

	FTimerHandle DestroyTimerHandle;

protected:
	// ========================================================
	// 무기 세팅 초기화 함수
	// ========================================================
	void InitializeWeaponRoot();
	
	// ========================================================
	// 죽기
	// ========================================================
	virtual void OnDead();

	void DestroyAfterDeath();
	// ========================================================
	// Damageable 
	// ========================================================
	void HandleHitReaction(EHitReactionType HitReactionType, const FVector& DamageImpulse, AActor* DamageCauser);
	
public:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// ========================================================
	// Damageable Interface
	// ========================================================
	virtual void ApplyDamage_Implementation(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse, EHitReactionType HitReactionType) override;
	virtual void NotifyDamage_Implementation(const FVector& DamageLocation, AActor* DamageSource) override;
	virtual void HandleDeath_Implementation() override;
	
	// ========================================================
	// Attacker Interface
	// ========================================================
	virtual void BeginAttackTrace_Implementation() override;
	virtual void DoAttackTrace_Implementation() override;
	virtual void EndAttackTrace_Implementation() override;
	virtual void CheckCombo_Implementation() override;
	virtual void ClearAttackAnimation_Implementation() override;
	
	// ========================================================
	// 패링
	// ========================================================
	UFUNCTION(BlueprintCallable, Category="Combat|Reaction")
	void PlayParriedReaction();
	
	virtual float GetDeathDestroyDelay() const;
	
	// ========================================================
	// Getter 
	// ========================================================
	UFUNCTION(BlueprintPure)
	UBaseStatComponent* GetStatComponent() const;
	UFUNCTION(BlueprintPure)
	UCombatBaseComponent* GetCombatComponent() const;
	UFUNCTION(BlueprintPure)
	USceneComponent* GetWeaponRoot() const;

};
