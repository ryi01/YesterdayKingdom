// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatComponent.h"

#include "PlayerDefinition.h"

void UPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(RecoveryTimerHandle, this, &UPlayerStatComponent::RecoverResources, 0.1f, true);
}
void UPlayerStatComponent::RecoverResources()
{
	if (!PlayerDefinition || IsDead()) return;
	const float Delta = 0.1f;
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - GetLastMPConsumeTime() >= PlayerDefinition->StaminaRecoveryDelay)
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
