// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_Danger.h"
#include "EnemyNomal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

void UEnvQueryContext_Danger::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	if (AEnemyNomal* QuerierActor = Cast<AEnemyNomal>(QueryInstance.Owner.Get()))
	{
		UEnvQueryItemType_Point::SetContextHelper(ContextData, QuerierActor->GetLastDangerLocation());
	}
}
