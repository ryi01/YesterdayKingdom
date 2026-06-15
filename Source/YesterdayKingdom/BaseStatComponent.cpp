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

void UBaseStatComponent::SetCurrentHP(float NewHP)
{
	CurrentHP = FMath::Clamp(NewHP, 0.f, MaxHP);

	bIsDead = CurrentHP <= 0.f;

	OnHPChanged.Broadcast(CurrentHP, MaxHP);
}

//===============================================================================
// 초기화
//===============================================================================
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

	MaxMP = StatRow->MaxMP;
	CurrentMP = MaxMP;

	Attack = StatRow->Attack;
	Defense = StatRow->Defense;
	
	MoveSpeed = StatRow->MoveSpeed;
	GuardSpeed = StatRow->GuardSpeed;
	RunSpeed = StatRow->RunSpeed;
	
	MaxStun = StatRow->MaxStun;
	CurrentStun = 0.f;
	
	bIsDead = false;
	bIsStun = false;

	OnHPChanged.Broadcast(CurrentHP, MaxHP);
	OnSTChanged.Broadcast(CurrentST, MaxST);
	OnMPChanged.Broadcast(CurrentMP, MaxMP);
	OnStunChanged.Broadcast(CurrentStun, MaxStun);
}
//===============================================================================
// HP 관련
//===============================================================================
float UBaseStatComponent::ApplyDamage(float Amount)
{
	if (bIsDead) return 0.f;
	const float FinalDamage = FMath::Max(Amount - GetFinalDefense(), 1.f);
	const float FinalMaxHP = GetMaxHP();
	CurrentHP = FMath::Clamp(CurrentHP - FinalDamage, 0.f, FinalMaxHP);
	OnHPChanged.Broadcast(CurrentHP, FinalMaxHP);
	UE_LOG(LogTemp, Warning, TEXT("%s HP : %f"), *GetOwner()->GetName(), CurrentHP);
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
	const float FinalMaxHP = GetMaxHP();
	CurrentHP = FMath::Clamp(CurrentHP + Amount, 0, FinalMaxHP);
	OnHPChanged.Broadcast(CurrentHP, FinalMaxHP);
}
//===============================================================================
// 스테미나 관련
//===============================================================================
bool UBaseStatComponent::ConsumeST(float Amount)
{
	if (IsDead()) return false;
	if (CurrentST < Amount)
	{
		UE_LOG(LogTemp, Warning, TEXT("LOWST"));
		return false;
	}
	const float FinalMaxST = GetMaxST();
	CurrentST = FMath::Clamp(CurrentST - Amount, 0, FinalMaxST);
	LastSTConsumeTime = GetWorld()->GetTimeSeconds();
	
	OnSTChanged.Broadcast(CurrentST, FinalMaxST);
	return true;
}

void UBaseStatComponent::RecoverST(float Amount)
{
	if (IsDead()) return;
	const float FinalMaxST = GetMaxST();
	CurrentST = FMath::Clamp(CurrentST + Amount, 0.f, FinalMaxST);
	OnSTChanged.Broadcast(CurrentST, FinalMaxST);
}
//===============================================================================
// MP 관련
//===============================================================================
bool UBaseStatComponent::ConsumeMP(float Amount)
{
	if (IsDead()) return false;
	if (CurrentMP < Amount) 
	{
		UE_LOG(LogTemp, Warning, TEXT("LOW MANA"));
		return false;
	}
		
	const float FinalMaxMP = GetMaxMP();
	CurrentMP = FMath::Clamp(CurrentMP - Amount, 0, FinalMaxMP);
	LastMPConsumeTime = GetWorld()->GetTimeSeconds();
	OnMPChanged.Broadcast(CurrentMP, FinalMaxMP);
	return true;
}

void UBaseStatComponent::RecoverMP(float Amount)
{
	if (IsDead()) return;
	const float FinalMaxMP = GetMaxMP();
	CurrentMP = FMath::Clamp(CurrentMP + Amount, 0.f, FinalMaxMP);
	OnMPChanged.Broadcast(CurrentMP, FinalMaxMP);
}
//===============================================================================
// Stun 관련
//===============================================================================
void UBaseStatComponent::AddStun(float Amount)
{
	if (bIsDead || bIsStun) return;
	CurrentStun = FMath::Clamp(CurrentStun + Amount, 0.f, MaxStun);
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
// ========================================================
// 버프 
// ========================================================
void UBaseStatComponent::AddBuffAttack(float Value)
{
	BuffAttackBonus += Value;
}

void UBaseStatComponent::AddBuffDefense(float Value)
{
	BuffDefenseBonus += Value;
}

void UBaseStatComponent::AddBonusMoveSpeed(float Value)
{
	if (FMath::IsNearlyZero(Value)) return;
	
	SkillBonusMoveSpeed += Value;
	SkillBonusRunSpeed += Value;
}

void UBaseStatComponent::ClearBuffAttack()
{
	BuffAttackBonus = 0.f;
}

void UBaseStatComponent::ClearBuffDefense()
{
	BuffDefenseBonus = 0.f;
}

void UBaseStatComponent::ClearAllBuffStats()
{
	BuffAttackBonus = 0.f;
	BuffDefenseBonus = 0.f;
}

// ========================================================
// 장비 장착 
// ========================================================
void UBaseStatComponent::SetEquipmentBonus(float InAttackBonus, float InDefenseBonus)
{
	EquipmentAttackBonus = InAttackBonus;
	EquipmentDefenseBonus = InDefenseBonus;
}

// ========================================================
// Skill Stat
// ========================================================
void UBaseStatComponent::AddSkillAttack(float Amount)
{
	SkillBonusAttack += Amount;
}

void UBaseStatComponent::AddSkillDefense(float Amount)
{
	SkillBonusDefense += Amount;
}

void UBaseStatComponent::AddSkillMaxHP(float Amount)
{
	const float OldMaxHP = GetMaxHP();
	SkillBonusMaxHP += Amount;
	
	const float NewMaxHP = GetMaxHP();
	CurrentHP += NewMaxHP - OldMaxHP;
	
	CurrentHP = FMath::Clamp(CurrentHP, 0.f, NewMaxHP);

	OnHPChanged.Broadcast(CurrentHP, NewMaxHP);
}

void UBaseStatComponent::AddSkillMaxMP(float Amount)
{
	const float OldMaxMP = GetMaxMP();

	SkillBonusMaxMP += Amount;

	const float NewMaxMP = GetMaxMP();
	CurrentMP += NewMaxMP - OldMaxMP;
	CurrentMP = FMath::Clamp(CurrentMP, 0.f, NewMaxMP);

	OnMPChanged.Broadcast(CurrentMP, NewMaxMP);
}

void UBaseStatComponent::AddSkillMaxST(float Amount)
{
	const float OldMaxST = GetMaxST();

	SkillBonusMaxST += Amount;

	const float NewMaxST = GetMaxST();
	CurrentST += NewMaxST - OldMaxST;
	CurrentST = FMath::Clamp(CurrentST, 0.f, NewMaxST);

	OnSTChanged.Broadcast(CurrentST, NewMaxST);
}

void UBaseStatComponent::AddSkillMoveSpeed(float Amount)
{
	SkillBonusMoveSpeed += Amount;
}

void UBaseStatComponent::AddSkillRunSpeed(float Amount)
{
	SkillBonusRunSpeed += Amount;
}

void UBaseStatComponent::ClearAllSkillStats()
{
	SkillBonusAttack = 0.f;
	SkillBonusDefense = 0.f;
	SkillBonusMaxHP = 0.f;
	SkillBonusMaxMP = 0.f;
	SkillBonusMaxST = 0.f;
	SkillBonusMoveSpeed = 0.f;
	SkillBonusRunSpeed = 0.f;

	CurrentHP = FMath::Clamp(CurrentHP, 0.f, GetMaxHP());
	CurrentMP = FMath::Clamp(CurrentMP, 0.f, GetMaxMP());
	CurrentST = FMath::Clamp(CurrentST, 0.f, GetMaxST());

	OnHPChanged.Broadcast(CurrentHP, GetMaxHP());
	OnMPChanged.Broadcast(CurrentMP, GetMaxMP());
	OnSTChanged.Broadcast(CurrentST, GetMaxST());
}
// ========================================================
// Setter
// ========================================================
void UBaseStatComponent::SetStatRowName(FName NewName)
{
	StatRowName = NewName;
}

void UBaseStatComponent::LoadCurrentStats(float InCurrentHP, float InCurrentST, float InCurrentMP)
{
	const float FinalMaxHP = GetMaxHP();
	const float FinalMaxST = GetMaxST();
	const float FinalMaxMP = GetMaxMP();

	CurrentHP = FMath::Clamp(InCurrentHP, 0.f, FinalMaxHP);
	CurrentST = FMath::Clamp(InCurrentST, 0.f, FinalMaxST);
	CurrentMP = FMath::Clamp(InCurrentMP, 0.f, FinalMaxMP);

	bIsDead = CurrentHP <= 0.f;

	OnHPChanged.Broadcast(CurrentHP, FinalMaxHP);
	OnSTChanged.Broadcast(CurrentST, FinalMaxST);
	OnMPChanged.Broadcast(CurrentMP, FinalMaxMP);
}

// ========================================================
// Getter 
// ========================================================
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
	return MaxHP + SkillBonusMaxHP;
}

float UBaseStatComponent::GetCurrentST() const
{
	return CurrentST;
}

float UBaseStatComponent::GetMaxST() const
{
	return MaxST + SkillBonusMaxST;
}

float UBaseStatComponent::GetCurrentMP() const
{
	return CurrentMP;
}

float UBaseStatComponent::GetMaxMP() const
{
	return MaxMP + SkillBonusMaxMP;
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
	return MoveSpeed + SkillBonusMoveSpeed;
}

float UBaseStatComponent::GetGuardMoveSpeed() const
{
	return GuardSpeed;
}

float UBaseStatComponent::GetRunSpeed() const
{
	return RunSpeed + SkillBonusRunSpeed;
}

float UBaseStatComponent::GetCurrentStun() const
{
	return CurrentStun;
}

float UBaseStatComponent::GetLastSTConsumeTime() const
{
	return LastSTConsumeTime;
}

float UBaseStatComponent::GetLastMPConsumeTime() const
{
	return LastMPConsumeTime;
}
float UBaseStatComponent::GetFinalAttack() const
{
	return Attack + EquipmentAttackBonus + BuffAttackBonus + SkillBonusAttack;
}

float UBaseStatComponent::GetFinalDefense() const
{
	return Defense + EquipmentDefenseBonus + BuffDefenseBonus + SkillBonusDefense;
}

#pragma endregion


