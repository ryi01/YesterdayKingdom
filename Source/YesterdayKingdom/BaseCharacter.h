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
	ABaseCharacter();

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
private:
protected:
	// ========================================================
	// 무기 세팅 초기화 함수
	// ========================================================
	void InitializeWeaponRoot();
	
public:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// ========================================================
	// Damageable Interface
	// ========================================================
	virtual void ApplyDamage_Implementation(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;
	virtual void NotifyDamage_Implementation(const FVector& DamageLocation, AActor* DamageSource) override;
	virtual void HandleDeath_Implementation() override;
	
	// ========================================================
	// Attacker Interface
	// ========================================================
	virtual void BeginAttackTrace() override;
	virtual void DoAttackTrace() override;
	virtual void EndAttackTrace() override;
	virtual void CheckCombo() override;
	
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
