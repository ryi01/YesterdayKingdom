// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_Player.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEnvQueryContext_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	UObject* QuerierObject = QueryInstance.Owner.Get();
	if (!QuerierObject)
	{
		return;
	}

	UWorld* World = QuerierObject->GetWorld();
	if (!World)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	if (!PlayerPawn)
	{
		return;
	}

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, PlayerPawn);
}
