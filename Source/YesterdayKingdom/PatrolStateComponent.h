// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSMStateComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "PatrolStateComponent.generated.h"

class UEnvQuery;

UCLASS()
class YESTERDAYKINGDOM_API UPatrolStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Patrol")
	TObjectPtr<UEnvQuery> PatrolQuery;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Patrol", meta = (ClampMin = "0.0"))
	float PatrolAcceptanceRadius = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM|Patrol")
	FVector PatrolTargetLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM|Patrol")
	bool bHasPatrolTarget = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM|Patrol")
	FVector LastPatrolTargetLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Patrol")
	float SameTargetRejectDistance = 150.f;

protected:
	void RunPatrolEQS();

	void OnPatrolQueryFinished(TSharedPtr<FEnvQueryResult> Result);
public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate(float DeltaTime) override;
	virtual void OnStateExit() override;
};
