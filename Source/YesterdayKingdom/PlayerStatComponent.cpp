// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatComponent.h"

#include "PlayerDefinition.h"
UPlayerStatComponent::UPlayerStatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}
void UPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerStatComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	RecoverResources(DeltaTime);
}

void UPlayerStatComponent::RecoverResources(float DeltaTime)
{
	if (!PlayerDefinition || IsDead()) return;
	const float Delta = 0.1f;
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - GetLastSTConsumeTime() >= PlayerDefinition->StaminaRecoveryDelay)
	{
		RecoverST(PlayerDefinition->StaminaRecoveryPerSecond * Delta);
	}
	if (CurrentTime - GetLastMPConsumeTime() >= PlayerDefinition->ManaRecoveryDelay)
	{
		RecoverMP(PlayerDefinition->ManaRecoveryPerSecond * Delta);
	}
}

const UPlayerDefinition* UPlayerStatComponent::GetPlayerDefinition() const
{
	return PlayerDefinition;
}
