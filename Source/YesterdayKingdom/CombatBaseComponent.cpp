// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatBaseComponent.h"
#include "Damagable.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "BaseCharacter.h"

// Sets default values for this component's properties
UCombatBaseComponent::UCombatBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
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
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValidHitActor(HitActor)) continue;
		HitActors.Add(HitActor);
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

bool UCombatBaseComponent::IsValidHitActor(AActor* HitActor) const
{
	if (!HitActor) return false;
	if (HitActor == OwnerCharacter.Get()) return false;
	if (HitActors.Contains(HitActor)) return false;
	if (!HitActor->GetClass()->ImplementsInterface(UDamagable::StaticClass())) return false;
	return true;
}

void UCombatBaseComponent::ApplyAttackHit(AActor* HitActor, const FHitResult& HitResult)
{
	if (!HitActor || !OwnerCharacter) return;
	const FVector DamageImpulse = OwnerCharacter->GetActorForwardVector();
	IDamagable::Execute_ApplyDamage(HitActor, DefaultDamage, OwnerCharacter, HitResult.ImpactPoint, DamageImpulse);
}


