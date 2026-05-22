// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyNomalAIController.generated.h"

class UStateTreeAIComponent;

UCLASS(Abstract)
class YESTERDAYKINGDOM_API AEnemyNomalAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyNomalAIController();
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
protected:
	// 스테이트 트리 AI 컴포넌트 참조
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStateTreeAIComponent* StateTreeAI;
};
