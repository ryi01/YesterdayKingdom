// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseStatComponent.h"

#include "CharacterStatusDataRow.h"

// Sets default values for this component's properties
UBaseStatComponent::UBaseStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UBaseStatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (StatDT && !StatRowName.IsNone())
	{
		InitializeStat(StatDT, StatRowName);
	}
}

void UBaseStatComponent::InitializeStat(UDataTable* InStatTable, FName InRowName)
{
	if (!InStatTable || InRowName.IsNone()) return;
	const FCharacterStatRow* StatRow = InStatTable->FindRow<FCharacterStatRow>(InRowName, TEXT("InitializeStat"));
	if (!StatRow) return;
	
	StatDT = InStatTable;
	StatRowName = InRowName;
	
	MaxHP = StatRow->MaxHP;
	CurrentHP = MaxHP;
	
	MaxST = StatRow->MaxST;
	CurrentST = MaxST;
	
	Attack = StatRow->Attack;
	Defense = StatRow->Defense;
	MoveSpeed = StatRow->MoveSpeed;
	RunSpeed = StatRow->RunSpeed;
	
	MaxStun = StatRow->MaxStun;
	CurrentStun = 0.f;
	
	bIsDead = false;
	bIsStun = false;

	OnHPChanged.Broadcast(CurrentHP, MaxHP);
	OnSTChanged.Broadcast(CurrentST, MaxST);
	OnStunChanged.Broadcast(CurrentStun, MaxStun);
}

float UBaseStatComponent::ApplyDamage(float Amount)
{
	if (bIsDead) return 0.f;
	const float FinalDamage = FMath::Max(Amount - Defense, 1.f);
	CurrentHP = FMath::Clamp(CurrentHP - FinalDamage, 0.f, MaxHP);
	OnHPChanged.Broadcast(CurrentHP, MaxHP);
	if (CurrentHP <= 0.f)
	{
		bIsDead = true;
		OnDead.Broadcast();
	}
	return FinalDamage;
}

void UBaseStatComponent::Heal(float Amount)
{
	if (bIsDead) return;
	CurrentHP = FMath::Clamp(CurrentHP + Amount, 0, MaxHP);
	OnHPChanged.Broadcast(CurrentHP, MaxHP);
}

bool UBaseStatComponent::ConsumeST(float Amount)
{
	if (IsDead()) return false;
	if (CurrentST < Amount) return false;
	CurrentST = FMath::Clamp(CurrentST - Amount, 0, MaxST);
	OnStunChanged.Broadcast(CurrentST, MaxST);
	return true;
}

void UBaseStatComponent::RecoverST(float Amount)
{
	if (IsDead()) return;
	CurrentST = FMath::Clamp(CurrentST + Amount, 0.f, MaxST);
	OnStunChanged.Broadcast(CurrentST, MaxST);
}

bool UBaseStatComponent::ConsumeMP(float Amount)
{
	if (IsDead()) return false;
	if (CurrentMP < Amount) return false;
	CurrentMP = FMath::Clamp(CurrentMP - Amount, 0, MaxMP);
	OnMPChanged.Broadcast(CurrentMP, MaxMP);
	return true;
}

void UBaseStatComponent::RecoverMP(float Amount)
{
	if (IsDead()) return;
	CurrentMP = FMath::Clamp(CurrentMP + Amount, 0.f, MaxMP);
	OnMPChanged.Broadcast(CurrentMP, MaxMP);
}

float UBaseStatComponent::GetCurrentMP() const
{
	return CurrentMP;
}

float UBaseStatComponent::GetMaxMP() const
{
	return MaxMP;
}

void UBaseStatComponent::AddStun(float Amount)
{
	if (bIsDead || bIsStun) return;
	CurrentStun = FMath::Clamp(CurrentStun, 0.f, MaxStun);
	OnStunChanged.Broadcast(CurrentStun, MaxStun);
	if (CurrentStun >= MaxStun)
	{
		bIsStun = true;
		OnStunned.Broadcast();
	}
}

void UBaseStatComponent::ResetStun()
{
	CurrentStun = 0.f;
	bIsStun = false;

	OnStunChanged.Broadcast(CurrentStun, MaxStun);
}

#pragma region Getter

bool UBaseStatComponent::IsDead() const
{
	return bIsDead;
}

bool UBaseStatComponent::IsStunned() const
{
	return bIsStun;
}
UDataTable* UBaseStatComponent::GetStatusDT() const
{
	return StatDT;
}
float UBaseStatComponent::GetCurrentHP() const
{
	return CurrentHP;
}

float UBaseStatComponent::GetMaxHP() const
{
	return MaxHP;
}

float UBaseStatComponent::GetCurrentST() const
{
	return CurrentST;
}

float UBaseStatComponent::GetMaxST() const
{
	return MaxST;
}

float UBaseStatComponent::GetAttack() const
{
	return Attack;
}

float UBaseStatComponent::GetDefense() const
{
	return Defense;
}

float UBaseStatComponent::GetMoveSpeed() const
{
	return MoveSpeed;
}

float UBaseStatComponent::GetRunSpeed() const
{
	return RunSpeed;
}

float UBaseStatComponent::GetCurrentStun() const
{
	return CurrentST;
}
#pragma endregion


