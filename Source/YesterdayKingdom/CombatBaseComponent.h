// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CommonEnumTypes.h" 
#include "CombatBaseComponent.generated.h"

class ABaseCharacter;
class UDataTable;

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
	float TraceHeight = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Damage")
	float DefaultDamage = 10.f;
	
	//===============================================================================
	// 공격 애니메이션 DT
	//===============================================================================
	UPROPERTY(EditDefaultsOnly, Category="Combat|Data")
	TObjectPtr<UDataTable> AttackDataTable;

	UPROPERTY(VisibleInstanceOnly, Category="Combat|Runtime")
	FName CurrentAttackRowName;

	FTimerHandle HitStopTimerHandle;
	
public:	
	// Sets default values for this component's properties
	UCombatBaseComponent();
protected:
	virtual bool IsValidHitActor(AActor* HitActor) const;
	virtual void ApplyAttackHit(AActor* HitActor, const FHitResult& HitResult);
	const FAttackDataRow* GetCurrentAttackData() const;
	void ApplyHitFeedback(const FHitFeedbackData& Feedback, AActor* HitActor);
	void ResetHitStop();
public:	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void BeginAttackTrace();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void DoAttackTrace();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void EndAttackTrace();
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void CheckCombo();
	
	void SetCurrentAttack(FName AttackRowName);
	
};
