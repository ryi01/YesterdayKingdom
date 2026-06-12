// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyPuppetMaster.generated.h"

class AEnemyElite;

UCLASS()
class YESTERDAYKINGDOM_API AEnemyPuppetMaster : public AEnemyBase
{
	GENERATED_BODY()
	FTimerHandle HPWidgetCheckTimerHandle;
public:
	AEnemyPuppetMaster(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category="Enemy|Puppet")
	void RegisterPuppet(AEnemyElite* Puppet);

	UFUNCTION(BlueprintCallable, Category="Enemy|Puppet")
	void UnregisterPuppet(AEnemyElite* Puppet);

protected:
	virtual void HandleDeath_Implementation() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UPROPERTY(EditAnywhere, Category="Puppet")
	float FindPuppetRadius = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI",meta = (ClampMin = "0.1"))
	float HPWidgetCheckInterval = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI",meta = (ClampMin = "0.0"))
	float HPWidgetVisibleDistance = 1800.f;
	
	FTimerHandle FindPuppetTimerHandle;

	void FindAndRegisterPuppets();
	void UpdateHPWidgetVisibility();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Puppet")
	TArray<TObjectPtr<AEnemyElite>> Puppets;
};
