// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyFSMTypes.h"
#include "FSMStateComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "FlankingStateComponent.generated.h"

class UEnvQuery;

UCLASS()
class YESTERDAYKINGDOM_API UFlankingStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()
private:
	float ElapsedTime = 0.f;
	float CurrentMaxFlankingDuration = 2.5f;
	
	FVector MoveTargetLocation = FVector::ZeroVector;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Next")
	EEnemyFSMStateType NextState = EEnemyFSMStateType::Chase;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|EQS")
	TObjectPtr<UEnvQuery> FlankingEQS;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Flanking")
	float FlankingMoveSpeed = 350.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Flanking")
	float MinFlankingDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Flanking")
	float MaxFlankingDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Flanking")
	float AcceptanceRadius = 80.f;
	
private:
	void RunFlankingEQS();
	void HandleFlankingEQSFinished(TSharedPtr<FEnvQueryResult> Result);
public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate(float DeltaTime) override;
	virtual void OnStateExit() override;
};
