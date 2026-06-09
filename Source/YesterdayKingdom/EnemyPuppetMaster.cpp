// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPuppetMaster.h"
#include "EnemyElite.h"
#include "EngineUtils.h"

AEnemyPuppetMaster::AEnemyPuppetMaster(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyPuppetMaster::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimerForNextTick(
		this,
		&AEnemyPuppetMaster::FindAndRegisterPuppets
	);
}

void AEnemyPuppetMaster::RegisterPuppet(AEnemyElite* Puppet)
{
	if (!IsValid(Puppet)) return;
	if (Puppets.Contains(Puppet)) return;
	
	Puppets.Add(Puppet);
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
	
	Puppets.Empty();
	
	Super::HandleDeath_Implementation();
}

void AEnemyPuppetMaster::FindAndRegisterPuppets()
{
	for (TActorIterator<AEnemyElite> It(GetWorld()); It; ++It)
	{
		AEnemyElite* Puppet = *It;
		if (!IsValid(Puppet)) continue;

		const float Distance = FVector::Dist(
			GetActorLocation(),
			Puppet->GetActorLocation()
		);

		if (Distance > FindPuppetRadius) continue;

		RegisterPuppet(Puppet);
	}
}


