// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPuppetMaster.h"
#include "EnemyElite.h"

AEnemyPuppetMaster::AEnemyPuppetMaster(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyPuppetMaster::BeginPlay()
{
	Super::BeginPlay();
	
	for (AEnemyElite* Puppet : Puppets)
	{
		if (Puppet)
		{
			RegisterPuppet(Puppet);
		}
	}
}

void AEnemyPuppetMaster::RegisterPuppet(AEnemyElite* Puppet)
{
	if (!Puppet)
	{
		return;
	}

	Puppets.AddUnique(Puppet);
	Puppet->SetPuppetMaster(this);
	
	UE_LOG(LogTemp, Warning, TEXT("[Master] Register Puppet: %s"),
		*GetNameSafe(Puppet));
}

void AEnemyPuppetMaster::UnregisterPuppet(AEnemyElite* Puppet)
{
	Puppets.Remove(Puppet);
}

void AEnemyPuppetMaster::HandleDeath_Implementation()
{
	for (AEnemyElite* Puppet : Puppets)
	{
		if (Puppet)
		{
			Puppet->ForceTrueDeath();
		}
	}

	Super::HandleDeath_Implementation();
}


