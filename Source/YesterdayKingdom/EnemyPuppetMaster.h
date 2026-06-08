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
	
public:
	AEnemyPuppetMaster(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category="Enemy|Puppet")
	void RegisterPuppet(AEnemyElite* Puppet);

	UFUNCTION(BlueprintCallable, Category="Enemy|Puppet")
	void UnregisterPuppet(AEnemyElite* Puppet);

protected:
	virtual void HandleDeath_Implementation() override;
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Puppet")
	TArray<TObjectPtr<AEnemyElite>> Puppets;
};
