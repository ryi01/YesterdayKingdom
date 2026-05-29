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

	void RecoverResources();
public:
	virtual void BeginPlay() override;
	const UPlayerDefinition* GetPlayerDefinition() const;
};
