// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSMStateComponent.h"
#include "ReturnStateComponent.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UReturnStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Return", meta = (ClampMin = "0.0"))
	float ReturnAcceptanceRadius = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Return")
	float ReChaseAllowedRadius = 300.f;
public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate(float DeltaTime) override;
	virtual void OnStateExit() override;


};
