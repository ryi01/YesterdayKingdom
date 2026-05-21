// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatBaseComponent.h"
#include "Damagable.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "BaseCharacter.h"

UCombatBaseComponent::UCombatBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UCombatBaseComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
}

// AttackMontage 실행 시, 공격 대상 판별 시작
void UCombatBaseComponent::BeginAttackTrace()
{
	bIsAttackTracing = true;
	HitActors.Empty();
}
// AttackMontage 실행 시, 공격 대상 판별 로직
void UCombatBaseComponent::DoAttackTrace()
{
	if (!bIsAttackTracing || !OwnerCharacter) return;
	const FVector Start = OwnerCharacter->GetActorLocation() + FVector(0.f, 0.f, TraceHeight);
	const FVector End = Start + OwnerCharacter->GetActorForwardVector() * TraceDistance;
	
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	// 플레이어 기준으로 sphere를 만들어 피격 대상 확인
	const bool bHit = GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(TraceRadius), Params);
#if ENABLE_DRAW_DEBUG
	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		bHit ? FColor::Red : FColor::Green,
		false,
		0.1f,
		0,
		1.f
	);

	DrawDebugSphere(
	GetWorld(),
		End,
		TraceRadius,
		12,
		bHit ? FColor::Red : FColor::Green,
		false,
		0.1f
	);
#endif
	
	if (!bHit) return;
	// 피격 대상이 있다면
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValidHitActor(HitActor)) continue;
		// 이미 피격한 대상이 아니라면 
		HitActors.Add(HitActor);
		// 데미지를 입힌다
		ApplyAttackHit(HitActor, Hit);
	}
}
// AttackMontage 실행 시, 공격 대상 판별 로직 종료
void UCombatBaseComponent::EndAttackTrace()
{
	bIsAttackTracing = false;
	HitActors.Empty();
}

void UCombatBaseComponent::CheckCombo()
{
}

// 피격당한 대상이 무엇인지 판별하는 함수
bool UCombatBaseComponent::IsValidHitActor(AActor* HitActor) const
{
	// 피격 대상이 있는가?
	if (!HitActor) return false;
	// 피격 대상이 스스로 인가?
	if (HitActor == OwnerCharacter.Get()) return false;
	// 피격을 당한 대상인가?
	if (HitActors.Contains(HitActor)) return false;
	// 피격인터페이스(IDamgable)이 있는가?
	if (!HitActor->GetClass()->ImplementsInterface(UDamagable::StaticClass())) return false;
	return true;
}
// 실질적인 피격
void UCombatBaseComponent::ApplyAttackHit(AActor* HitActor, const FHitResult& HitResult)
{
	if (!HitActor || !OwnerCharacter) return;
	const FVector DamageImpulse = OwnerCharacter->GetActorForwardVector();
	IDamagable::Execute_ApplyDamage(HitActor, DefaultDamage, OwnerCharacter, HitResult.ImpactPoint, DamageImpulse);
}


