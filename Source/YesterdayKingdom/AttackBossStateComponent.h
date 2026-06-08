// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSMStateComponent.h"
#include "AttackBossStateComponent.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UAttackBossStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()
	
public:

	virtual void OnStateEnter() override;

	virtual void OnStateUpdate(float) override;

	virtual void OnStateExit() override;
};
