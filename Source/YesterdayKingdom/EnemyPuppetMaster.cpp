// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPuppetMaster.h"
#include "EnemyElite.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

AEnemyPuppetMaster::AEnemyPuppetMaster(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyPuppetMaster::BeginPlay()
{
	Super::BeginPlay();
	SetEnemyHPWidgetVisible(false);
	GetWorldTimerManager().SetTimerForNextTick(
		this,
		&AEnemyPuppetMaster::FindAndRegisterPuppets
	);
	GetWorldTimerManager().SetTimer(
		HPWidgetCheckTimerHandle,
		this,
		&AEnemyPuppetMaster::UpdateHPWidgetVisibility,
		HPWidgetCheckInterval,
		true
	);

	UpdateHPWidgetVisibility();
}

void AEnemyPuppetMaster::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(
	HPWidgetCheckTimerHandle);
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
	GetWorldTimerManager().ClearTimer(
	HPWidgetCheckTimerHandle
);
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

void AEnemyPuppetMaster::UpdateHPWidgetVisibility()
{
	if (IsDead())
	{
		SetEnemyHPWidgetVisible(false);
		return;
	}

	APawn* PlayerPawn =UGameplayStatics::GetPlayerPawn(this, 0);

	if (!PlayerPawn)
	{
		SetEnemyHPWidgetVisible(false);
		return;
	}

	const float DistanceSquared =FVector::DistSquared(GetActorLocation(),PlayerPawn->GetActorLocation());

	const float VisibleDistanceSquared =FMath::Square(HPWidgetVisibleDistance);

	SetEnemyHPWidgetVisible(DistanceSquared <= VisibleDistanceSquared);
}


