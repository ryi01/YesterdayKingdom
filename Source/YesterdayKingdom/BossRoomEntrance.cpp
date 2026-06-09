// Fill out your copyright notice in the Description page of Project Settings.


#include "BossRoomEntrance.h"

// Sets default values
ABossRoomEntrance::ABossRoomEntrance()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ABossRoomEntrance::RequestCloseEntrance()
{
	OnCloseRequested.Broadcast();
}

