// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerDefinition.generated.h"


UCLASS(BlueprintType)
class YESTERDAYKINGDOM_API UPlayerDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// ST
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recovery")
	float StaminaRecoveryPerSecond = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recovery")
	float StaminaRecoveryDelay = 1.5f;

	// MP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recovery")
	float ManaRecoveryPerSecond = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recovery")
	float ManaRecoveryDelay = 3.f;

	// Dash
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cost")
	float DashSTCostPerSecond = 15.f;
};
