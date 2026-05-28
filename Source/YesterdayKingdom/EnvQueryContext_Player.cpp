// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_Player.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEnvQueryContext_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(QueryInstance.Owner.Get(), 0);
	check(PlayerPawn);
	
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, PlayerPawn);
}
