// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCombatComponent.h"

#include "BaseCharacter.h"
#include "BaseStatComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"

//===============================================================================================
// 플레이어 시선 보정
//===============================================================================================
void UPlayerCombatComponent::FaceBestTarget()
{
	AActor* Target = FindBestTarget();
	if (!OwnerCharacter || !Target) return;
	
	FVector ToTarget = Target->GetActorLocation() - OwnerCharacter->GetActorLocation();
	ToTarget.Z = 0.f;
	
	if (ToTarget.IsNearlyZero()) return;
	const FRotator TargetRot = ToTarget.Rotation();
	OwnerCharacter->SetActorRotation(TargetRot);
}
//===============================================================================================
// 적합한 target 찾기
//===============================================================================================
AActor* UPlayerCombatComponent::FindBestTarget() const
{
	if (!OwnerCharacter) return nullptr;

	TArray<FOverlapResult> Overlaps;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter.Get());
	const FVector Origin = OwnerCharacter->GetActorLocation();
	
	const bool bHit = GetWorld()->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(500.f), Params);
	if (!bHit) return nullptr;
	AActor* BestTarget = nullptr;
	float BestScore = -9999.f;

	const FVector Forward = OwnerCharacter->GetActorForwardVector();
	
	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* Target = Result.GetActor();
		if (!Target || Target == OwnerCharacter.Get()) continue;
		if (!Target->GetClass()->ImplementsInterface(UDamagable::StaticClass())) continue;
		
		FVector ToTarget = Target->GetActorLocation() - Origin;
		ToTarget.Z = 0.f;
		
		const float Distance = ToTarget.Size();
		const FVector Direction = ToTarget.GetSafeNormal();
		
		// 플레이어의 정면 위치를 확인후 
		const float Dot = FVector::DotProduct(Forward, Direction);
		if (Dot < 0.5f) continue;
		
		// Dot * 2.f : 앞이면 2점 옆이면 1점 뒤면 0점을 부여
		// Distance / 500.f : 거리 확인
		const float Score = Dot * 2.f - Distance / 500.f;
		// 종합 점수에 따라 target을 갱신 
		if (Score > BestScore)
		{
			BestScore = Score;
			BestTarget = Target;
		}
	}
	return BestTarget;
}


//===============================================================================================
// 위치 보정을 위해 override
//===============================================================================================
void UPlayerCombatComponent::BeginAttackTrace() 
{
	// 위치 보정
	FaceBestTarget();
	Super::BeginAttackTrace();
}

//===============================================================================================
// 플레이어 입력과 연결된 함수
//===============================================================================================
void UPlayerCombatComponent::RequestAttack(EAttackType AttackType)
{
	if (AttackType == EAttackType::Charge)
	{
		StartChargeAttack();
		return;
	}
	
	FName AttackRowName = NAME_None;
	if (!TryGetAttackRowName(AttackType, AttackRowName)) return;
	
	const FAttackDataRow* AttackData = GetAttackDataByRow(AttackRowName);
	if (!AttackData) return;
	
	if (AttackData->StaminaCost > 0 && !OwnerCharacter->GetStatComponent()->ConsumeST(AttackData->StaminaCost)) return;
	if (AttackData->MPCost > 0 && !OwnerCharacter->GetStatComponent()->ConsumeMP(AttackData->MPCost)) return;
	
	RequestAttackByRow(AttackRowName);
}
void UPlayerCombatComponent::OnChargeAttackStarted()
{
	FaceBestTarget();
}

void UPlayerCombatComponent::OnGuardStarted()
{
	Super::OnGuardStarted();
	if (!OwnerCharacter) return;

	if (UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement())
	{
		MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
		MoveComp->MaxWalkSpeedCrouched = OwnerCharacter->GetStatComponent()->GetCrouchMoveSpeed();
	}
	OwnerCharacter->Crouch();
}

void UPlayerCombatComponent::OnGuardEnded()
{
	Super::OnGuardEnded();
	if (!OwnerCharacter) return;

	OwnerCharacter->UnCrouch();

	if (UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = OwnerCharacter->GetStatComponent()->GetMoveSpeed();
	}
}

void UPlayerCombatComponent::StartChargeAttack()
{
	FName ChargeRowName = NAME_None;
	if (!TryGetAttackRowName(EAttackType::Charge, ChargeRowName)) return;

	StartChargeAttackByRow(ChargeRowName);
}
bool UPlayerCombatComponent::TryGetAttackRowName(EAttackType AttackType, FName& OutRowName) const
{
	OutRowName = NAME_None;

	if (!PlayerAttackRows.Contains(AttackType))
	{
		return false;
	}

	OutRowName = PlayerAttackRows[AttackType];

	return !OutRowName.IsNone();
}

