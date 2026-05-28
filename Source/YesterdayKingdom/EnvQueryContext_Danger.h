// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_Danger.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UEnvQueryContext_Danger : public UEnvQueryContext
{
	GENERATED_BODY()
	
public:
	
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
	
};
