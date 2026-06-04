// Fill out your copyright notice in the Description page of Project Settings.


#include "PatternSelectStateComponent.h"

#include "EnemyBase.h"
#include "EnemyFSMControllerComponent.h"

void UPatternSelectStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	if (!OwnerCharacter || !FSMController || !EnemyDefinition) return;
	const FName SelectedPattern = SelectBossAttackPattern();
	
	if (SelectedPattern.IsNone())
	{
		if (!EnemyDefinition->AttackSet.MainAttackRowName.IsNone())
		{
			OwnerCharacter->SetSelectedAttackRowName(EnemyDefinition->AttackSet.MainAttackRowName);
			FSMController->ChangeState(NextState);
		}
		return;
	}
	
	OwnerCharacter->SetSelectedAttackRowName(SelectedPattern);
	
	UE_LOG(LogTemp, Warning, TEXT("[FSM][PatternSelect] Selected Pattern : %s"),
		*SelectedPattern.ToString());

	FSMController->ChangeState(NextState);
}

FName UPatternSelectStateComponent::SelectBossAttackPattern() const
{
	if (!EnemyDefinition) return NAME_None;
	const float DistanceToPlayer = GetDistanceToPlayer();
	// 사용 가능한 공격 패턴 후보 
	TArray<const FBossAttackPattern*> Candidates;
	float TotalWeight = 0.f;
	
	for (const FBossAttackPattern& Pattern : EnemyDefinition->BossAttackPatterns)
	{
		// 거리 기반으로 가능한 패턴만 후보에 추가 
		if (!CanUseBossPattern(Pattern, DistanceToPlayer)) continue;
		// 공격 확률을 확인해서 추가
		const float SafeWeight = FMath::Max(0.f, Pattern.Weight);
		if (SafeWeight <= 0.f) continue;
		Candidates.Add(&Pattern);
		// 전체 weight 누적 => 랜덤 확률 계산을 위함 
		TotalWeight += SafeWeight;
	}
	if (Candidates.IsEmpty() || TotalWeight <= 0.f) return NAME_None;
	
	// weight 총합에서 랜덤값 뽑기 
	float RandomValue = FMath::FRandRange(0.f, TotalWeight);
	for (const FBossAttackPattern* Pattern : Candidates)
	{
		// 구간을 만들기 위해 뽑힌 랜덤 값에서 Weight를 제거 
		RandomValue -= FMath::Max(0.f, Pattern->Weight);
		if (RandomValue <= 0.f) return Pattern->AttackRowName;
	}
	return Candidates.Last()->AttackRowName;
}

bool UPatternSelectStateComponent::CanUseBossPattern(const FBossAttackPattern& Pattern, float DistanceToPlayer) const
{
	if (Pattern.AttackRowName.IsNone() || DistanceToPlayer < Pattern.MinRange || DistanceToPlayer > Pattern.MaxRange) return false;
	const int32 CurrentPhase = GetCurrentPhase();
	if (CurrentPhase < Pattern.MinPhase || CurrentPhase > Pattern.MaxPhase) return false;
	return true;
}

int32 UPatternSelectStateComponent::GetCurrentPhase() const
{
	return 1;
}


