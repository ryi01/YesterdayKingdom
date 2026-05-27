// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCombatComponent.h"

#include "BaseCharacter.h"

void UPlayerCombatComponent::BeginAttackTrace()
{
	Super::BeginAttackTrace();
	
}

// Hit 판별
// Hit 판별
void UPlayerCombatComponent::DoAttackTrace()
{
	Super::DoAttackTrace();
}

void UPlayerCombatComponent::EndAttackTrace()
{
	Super::EndAttackTrace();

}

//
void UPlayerCombatComponent::CheckCombo()
{
	Super::CheckCombo();
	
	if (ComboMontages.IsValidIndex(AttackIndex))
	{
		UAnimMontage* SelectedMontage = ComboMontages[AttackIndex];
		
		AttackIndex = (AttackIndex + 1) % ComboMontages.Num();
	}
}

void UPlayerCombatComponent::RequestAttack(FName AttackRowName)
{
	Super::RequestAttack(AttackRowName);
	const FAttackDataRow* AttackData = GetCurrentAttackData();
	if (!AttackData || !AttackData->Montage || !OwnerCharacter) return;
	OwnerCharacter->PlayAnimMontage(AttackData->Montage.Get());
}
