// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSkillComponent.h"

#include "GoldComponent.h"
#include "PlayerCharacter.h"
#include "PlayerStatComponent.h"

// Sets default values for this component's properties
UPlayerSkillComponent::UPlayerSkillComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

// Called when the game starts
void UPlayerSkillComponent::BeginPlay()
{
	Super::BeginPlay();

}

bool UPlayerSkillComponent::TryUnlockSkill(FName SkillRowName)
{
	if (SkillRowName.IsNone()) return false;
	if (IsSkillUnlocked(SkillRowName))return false;
	const FSkillDataRow* SkillDataRow = FindSkillData(SkillRowName);
	if (!SkillDataRow) return false;
	
	if (!HasRequiredSkill(*SkillDataRow)) return false;
	
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
	if (!Player) return false;

	UGoldComponent* GoldComp = Player->GetGoldComponent();
	if (!GoldComp) return false;
	
	if (!GoldComp->SpendGold(SkillDataRow->GoldCost))
	{
		return false;
	}
	
	UnlockedSkillRowNames.Add(SkillRowName);
	
	ApplySkillEffect(*SkillDataRow);
	
	UE_LOG(LogTemp, Log, TEXT("[SkillTree] Unlocked Skill: %s"), *SkillRowName.ToString());

	OnSkillTreeChanged.Broadcast();

	return true;
}

bool UPlayerSkillComponent::IsSkillUnlocked(FName SkillRowName) const
{
	return UnlockedSkillRowNames.Contains(SkillRowName);
}

bool UPlayerSkillComponent::CanUnlockSkill(FName SkillRowName) const
{
	if (SkillRowName.IsNone() || IsSkillUnlocked(SkillRowName)) return false;
	const FSkillDataRow* SkillDataRow = FindSkillData(SkillRowName);
	if (!SkillDataRow) return false;
	if (!HasRequiredSkill(*SkillDataRow)) return false;
	const APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
	if (!Player) return false;

	const UGoldComponent* GoldComp = Player->GetGoldComponent();
	if (!GoldComp) return false;
	
	return GoldComp->GetGold() >= SkillDataRow->GoldCost;
}

bool UPlayerSkillComponent::GetSkillData(FName SkillRowName, FSkillDataRow& OutSkillData) const
{
	const FSkillDataRow* SkillDataRow = FindSkillData(SkillRowName);
	if (!SkillDataRow) return false;
	
	OutSkillData = *SkillDataRow;
	return true;
}
const FSkillDataRow* UPlayerSkillComponent::FindSkillData(FName SkillRowName) const
{
	if (!SkillDataTable) return nullptr;
	
	return SkillDataTable->FindRow<FSkillDataRow>(SkillRowName, TEXT("SkillData"));
	
}

bool UPlayerSkillComponent::HasRequiredSkill(const FSkillDataRow& SkillData) const
{
	if (SkillData.RequiredSkillRowName.IsNone()) return true;
	return UnlockedSkillRowNames.Contains(SkillData.RequiredSkillRowName);
}

void UPlayerSkillComponent::ApplySkillEffect(const FSkillDataRow& SkillData)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
	if (!Player) return;
	
	UPlayerStatComponent* StatComponent = Cast<UPlayerStatComponent>(Player->GetStatComponent());
	if (!StatComponent) return;
	
	switch (SkillData.EffectType)
	{
	case ESkillEffectType::AddMaxHP:
		StatComponent->AddSkillMaxHP(SkillData.EffectValue);
		break;;
	case ESkillEffectType::AddMaxMP:
		StatComponent->AddSkillMaxMP(SkillData.EffectValue);
		break;
	case ESkillEffectType::AddMaxST:
		StatComponent->AddSkillMaxST(SkillData.EffectValue);
		break;
	case ESkillEffectType::AddAttack:
		StatComponent->AddSkillAttack(SkillData.EffectValue);
		break;
	case ESkillEffectType::AddDefense:
		StatComponent->AddSkillDefense(SkillData.EffectValue);
		break;
	case ESkillEffectType::AddMoveSpeed:
		StatComponent->AddBonusMoveSpeed(SkillData.EffectValue);
		Player->RefreshMoveSpeed();
		break;

	case ESkillEffectType::UnlockChargeAttack:
		bCanUseChargeAttack = true;
		break;

	case ESkillEffectType::UnlockParry:
		bCanUseParry = true;
		break;

	default:
		break;
	}
	
}
const TSet<FName>& UPlayerSkillComponent::GetUnlockedSkillRowNames() const
{
	return UnlockedSkillRowNames;
}

bool UPlayerSkillComponent::CanUseChargeAttack() const
{
	return bCanUseChargeAttack;
}

bool UPlayerSkillComponent::CanUseParry() const
{
	return bCanUseParry;
}

