// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossRoomEntrance.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossRoomEntranceCloseRequested);
UCLASS()
class YESTERDAYKINGDOM_API ABossRoomEntrance : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossRoomEntrance();

	UFUNCTION(BlueprintCallable, Category = "BossRoom")
	void RequestCloseEntrance();

	UPROPERTY(BlueprintAssignable, Category = "BossRoom")
	FOnBossRoomEntranceCloseRequested OnCloseRequested;

};
