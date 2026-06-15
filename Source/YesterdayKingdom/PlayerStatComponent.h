// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseStatComponent.h"
#include "PlayerStatComponent.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UPlayerStatComponent : public UBaseStatComponent
{
	GENERATED_BODY()
protected:
	//===============================================================================================
	// 회복
	//===============================================================================================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recovery")
	TObjectPtr<class UPlayerDefinition> PlayerDefinition;
	
	FTimerHandle RecoveryTimerHandle;

public:
	UPlayerStatComponent();
protected:
	void RecoverResources(float DeltaTime);
public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//===============================================================================================
	// Getter
	//===============================================================================================
	const UPlayerDefinition* GetPlayerDefinition() const;
};
