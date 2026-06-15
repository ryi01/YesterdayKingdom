// Fill out your copyright notice in the Description page of Project Settings.


#include "GoldComponent.h"

// Sets default values for this component's properties
UGoldComponent::UGoldComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

// Called when the game starts
void UGoldComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UGoldComponent::LoadGold(int32 InGold)
{
	SetGold(InGold);
}

void UGoldComponent::AddGold(int32 Amount)
{
	if (Amount <= 0) return;
	SetGold(CurrentGold + Amount);
}

bool UGoldComponent::SpendGold(int32 Amount)
{
	if (Amount <= 0) return false;
	if (!CanAfford(Amount)) return false;
	SetGold(CurrentGold - Amount);
	return true;
}

bool UGoldComponent::CanAfford(int32 Amount) const
{
	return CurrentGold >= Amount;
}

int32 UGoldComponent::GetGold() const
{
	return CurrentGold; 
}

void UGoldComponent::SetGold(int32 NewGold)
{
	CurrentGold = FMath::Max(0, NewGold);

	OnGoldChanged.Broadcast(CurrentGold);
}