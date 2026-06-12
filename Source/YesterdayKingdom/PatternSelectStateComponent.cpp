// Fill out your copyright notice in the Description page of Project Settings.


#include "PatternSelectStateComponent.h"

#include "BaseStatComponent.h"
#include "EnemyBase.h"
#include "EnemyFSMControllerComponent.h"

void UPatternSelectStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	if (!OwnerCharacter || !FSMController || !EnemyDefinition)
	{
		if (FSMController)
		{
			FSMController->ChangeState(EEnemyFSMStateType::Chase);
		}
		return;
	}
	StopMove();
	const float DistanceToPlayer = GetDistanceToPlayer();
	const int32 CurrentPhase = GetCurrentPhase();
	
	UE_LOG(LogTemp, Warning,
	TEXT("[FSM][PatternSelect] Enter / Distance = %.2f / PatternCount = %d / Phase = %d"),
	GetDistanceToPlayer(),
	EnemyDefinition ? EnemyDefinition->BossAttackPatterns.Num() : -1,
	CurrentPhase);
	
	TArray<const FBossAttackPattern*> ValidPatterns;
	float TotalWeight = 0.f;
	
	for (const FBossAttackPattern& Pattern : EnemyDefinition->BossAttackPatterns)
	{
		if (Pattern.AttackRowName.IsNone()) continue;
		if (DistanceToPlayer < Pattern.MinRange || DistanceToPlayer > Pattern.MaxRange) continue;
		if (CurrentPhase < Pattern.MinPhase || CurrentPhase > Pattern.MaxPhase) continue;
		if (!bHasSelectedOpeningPattern && !Pattern.bCanUseAsOpeningPattern) continue;
		if (IsPatternOnCooldown(Pattern)) continue;
		if (Pattern.Weight <= 0.f) continue;
		ValidPatterns.Add(&Pattern);
		TotalWeight += Pattern.Weight;
	}
	if (ValidPatterns.Num() <= 0 || TotalWeight <= 0.f)
	{
		OwnerCharacter->BlockPatternSelect(NoValidPatternChaseTime);
		FSMController->ChangeState(EEnemyFSMStateType::Chase);
		return;
	}
	const float RandomValue = FMath::FRandRange(0.f, TotalWeight);
	float AccumulatedWeight = 0.f;

	const FBossAttackPattern* SelectedPattern = nullptr;

	for (const FBossAttackPattern* Pattern : ValidPatterns)
	{
		AccumulatedWeight += Pattern->Weight;

		if (RandomValue <= AccumulatedWeight)
		{
			SelectedPattern = Pattern;
			break;
		}
	}
	if (!SelectedPattern)
	{
		SelectedPattern = ValidPatterns[0];
	}

	OwnerCharacter->SetSelectedAttackRowName(SelectedPattern->AttackRowName);
	MarkPatternUsed(*SelectedPattern);
	
	bHasSelectedOpeningPattern = true;
	
	UE_LOG(LogTemp, Warning,
		TEXT("[FSM][PatternSelect] Selected / Row=%s / State=%d / Distance=%.2f / Cooldown=%.2f"),
		*SelectedPattern->AttackRowName.ToString(),
		static_cast<uint8>(SelectedPattern->ExecuteState),
		DistanceToPlayer,
		SelectedPattern->Cooldown);

	FSMController->ChangeState(SelectedPattern->ExecuteState);
}

bool UPatternSelectStateComponent::IsPatternOnCooldown(const FBossAttackPattern& Pattern) const
{
	if (!OwnerCharacter) return true;
	if (Pattern.AttackRowName.IsNone()) return true;
	if (Pattern.Cooldown <= 0.f)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[FSM][PatternSelect] Cooldown Disabled / Row=%s / Cooldown=%.2f"),
			*Pattern.AttackRowName.ToString(),
			Pattern.Cooldown);

		return false;
	}
	const float* LastUsedTime = LastPatternUsedTimeMap.Find(Pattern.AttackRowName);
	
	if (!LastUsedTime) return false;
	
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float ElapsedTime = CurrentTime - *LastUsedTime;
	const bool bOnCooldown = ElapsedTime < Pattern.Cooldown;

	UE_LOG(LogTemp, Warning,
		TEXT("[FSM][PatternSelect] Cooldown Check / Row=%s / Elapsed=%.2f / Cooldown=%.2f / OnCooldown=%d"),
		*Pattern.AttackRowName.ToString(),
		ElapsedTime,
		Pattern.Cooldown,
		bOnCooldown);

	return bOnCooldown;
}

void UPatternSelectStateComponent::MarkPatternUsed(const FBossAttackPattern& Pattern)
{
	if (!OwnerCharacter || Pattern.AttackRowName.IsNone()) return;
	
	LastPatternUsedTimeMap.Add(Pattern.AttackRowName, GetWorld()->GetTimeSeconds());
	
	UE_LOG(LogTemp, Warning,
	TEXT("[FSM][PatternSelect] Mark Used / Row=%s / Time=%.2f / Cooldown=%.2f"),
	*Pattern.AttackRowName.ToString(),
	GetWorld()->GetTimeSeconds(),
	Pattern.Cooldown);
}

int32 UPatternSelectStateComponent::GetCurrentPhase() const
{
	if (!OwnerCharacter) return 1;
	
	return OwnerCharacter->GetCurrentPhase();
}



