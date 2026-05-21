// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatBaseComponent.generated.h"

class ABaseCharacter;

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
	TSet<TObjectPtr<AActor>> HitActors;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Trace")
	float TraceDistance = 180.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Trace")
	float TraceRadius = 45.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Trace")
	float TraceHeight = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Damage")
	float DefaultDamage = 10.f;

public:	
	// Sets default values for this component's properties
	UCombatBaseComponent();
protected:
	virtual bool IsValidHitActor(AActor* HitActor) const;
	virtual void ApplyAttackHit(AActor* HitActor, const FHitResult& HitResult);

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
};
