// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyElite.generated.h"

class UPatrolStateComponent;
class UEnemyFSMControllerComponent;
class UIdleStatComponent;
class UChaseStateComponent;
class UAttackStateComponent;
class UHitStateComponent;
class UReturnStateComponent;
class UDownStateComponent;
class UReviveStateComponent;
class UDeadStateComponent;
class AEnemyPuppetMaster;
class USkeletalMeshComponent;

UCLASS()
class YESTERDAYKINGDOM_API AEnemyElite : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	AEnemyElite(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Puppet")
	TObjectPtr<AEnemyPuppetMaster> PuppetMaster;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName StringSocketName = TEXT("Weapon_RSocket");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Puppet|String")
	TObjectPtr<USkeletalMesh> StringMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Puppet|String")
	TSubclassOf<UAnimInstance> StringAnimBP;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Enemy|Puppet")
	bool IsPuppetMasterDead() const;
	
	UFUNCTION(BlueprintCallable, Category="Enemy|Puppet")
	void ForceTrueDeath();
	
	void SetPuppetMaster(AEnemyPuppetMaster* InMaster);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|Puppet")
	bool bPendingDownAfterLanded = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|Puppet")
	bool bTrueDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UIdleStatComponent> IdleState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UPatrolStateComponent> PatrolState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UChaseStateComponent> ChaseState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UAttackStateComponent> AttackState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UHitStateComponent> HitState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UReturnStateComponent> ReturnState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UDownStateComponent> DownState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UReviveStateComponent> ReviveState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FSM|State")
	TObjectPtr<UDeadStateComponent> DeadState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Weapon")
	TObjectPtr<USkeletalMeshComponent> StringMeshComponent;
public:
	virtual void NotifyDamage_Implementation(const FVector& DamageLocation, AActor* DamageSource) override;
	virtual void ApplyDamage_Implementation(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse, EHitReactionType HitReactionType) override;
};
