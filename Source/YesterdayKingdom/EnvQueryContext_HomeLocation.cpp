// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_HomeLocation.h"

#include "EnemyBase.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

void UEnvQueryContext_HomeLocation::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();

	AEnemyBase* Enemy = Cast<AEnemyBase>(QueryOwner);
	if (!Enemy)
	{
		APawn* Pawn = Cast<APawn>(QueryOwner);
		if (Pawn) Enemy = Cast<AEnemyBase>(Pawn);
	}
	if (!Enemy)
	{
		return;
	}

	UEnvQueryItemType_Point::SetContextHelper(ContextData, Enemy->GetHomeLocation());
}
