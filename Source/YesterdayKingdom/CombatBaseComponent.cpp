// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatBaseComponent.h"
#include "Damagable.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "BaseCharacter.h"
#include "Animation/AnimMontage.h"
#include "CommonEnumTypes.h"
#include "Kismet/GameplayStatics.h"

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
//=====================================================================================================
// 입력값이 들어오면 실행되는 함수 
//=====================================================================================================
void UCombatBaseComponent::RequestAttackByRow(FName AttackRowName)
{
	if (!OwnerCharacter || !AttackDataTable) return;
	if (!CurrentAttackRowName.IsNone())
	{
		if (CurrentAttackRowName == AttackRowName)
		{
			bComboInputBuffered = true;
		}
		return;
	}
	const FAttackDataRow* AttackDataRow = AttackDataTable->FindRow<FAttackDataRow>(AttackRowName, TEXT("RequestAttack"));
	
	if (!AttackDataRow || !AttackDataRow->Montage || AttackDataRow->Nodes.Num() <= 0) return;
	
	CurrentAttackRowName = AttackRowName;
	CurrentAttackNodeIndex = 0;
	bComboInputBuffered = false;
	
	const FAttackNodeData* FirstNode =  AttackDataRow->Nodes.IsValidIndex(CurrentAttackNodeIndex) ? &AttackDataRow->Nodes[CurrentAttackNodeIndex] : nullptr;
	
	if (FirstNode)
	{
		UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		if (!AnimInstance) return;
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &UCombatBaseComponent::OnAttackMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackDataRow->Montage);
		OwnerCharacter->PlayAnimMontage(AttackDataRow->Montage, 1.f, FirstNode->SectionName);
	}
}
// 애니메이션 종료
void UCombatBaseComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ResetAttackState();
	OnAttackEnded.Broadcast();
}

void UCombatBaseComponent::ResetAttackState()
{
	CurrentAttackRowName = NAME_None;
	CurrentAttackNodeIndex = INDEX_NONE;

	bComboInputBuffered = false;
	bIsAttackTracing = false;
	
	HitActors.Empty();
}
//=====================================================================================================
// 공격 관련 함수
//=====================================================================================================
void UCombatBaseComponent::CheckCombo()
{
	if (!bAutoCombo && !bComboInputBuffered) return;
	bComboInputBuffered = false;
	
	JumpToNextAttackSection();
}
void UCombatBaseComponent::ChargedAttack()
{
	if (!bComboInputBuffered) return;

	bComboInputBuffered = false;
	JumpToNextAttackSection();
}
bool UCombatBaseComponent::JumpToNextAttackSection()
{
	if (!OwnerCharacter) return false;
	// DT에서 필요한 열 추출
	const FAttackDataRow* AttackDataRow = GetAttackDataByRow(CurrentAttackRowName);
	if (!AttackDataRow || !AttackDataRow->Montage) return false;
	// 사용되는 Row에서 NodeData 추출
	const FAttackNodeData* CurrentNode = GetCurrentAttackNodeData();
	if (!CurrentNode) return false;

	if (CurrentNode->NextIndex == INDEX_NONE || !AttackDataRow->Nodes.IsValidIndex(CurrentNode->NextIndex))
	{
		CurrentAttackRowName = NAME_None;
		CurrentAttackNodeIndex = INDEX_NONE;
		bComboInputBuffered = false;
		return false;
	}
	// 현재 실행되는 콤보의 인덱스를 올리고
	CurrentAttackNodeIndex = CurrentNode->NextIndex;
	
	const FAttackNodeData* NextNode = GetCurrentAttackNodeData();
	if (!NextNode) return false;

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return false;
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UCombatBaseComponent::OnAttackMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackDataRow->Montage);
	// 애니메이션을 실행한다
	AnimInstance->Montage_JumpToSection(NextNode->SectionName, AttackDataRow->Montage);
	
	return true;
}
//=====================================================================================================
// 공격시, 공격 대상 탐지 => 공격을 맞은 대상이 있는가?
//=====================================================================================================
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
	Params.AddIgnoredActor(OwnerCharacter.Get());
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
//=====================================================================================================
// 실질적인 피격 관련 함수
//=====================================================================================================
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
	const FAttackNodeData* NodeData = GetCurrentAttackNodeData();

	const float Damage = NodeData ? NodeData->Damage : DefaultDamage;
	const FVector DamageImpulse = OwnerCharacter->GetActorForwardVector();
	
	IDamagable::Execute_ApplyDamage(HitActor, Damage, OwnerCharacter.Get(), HitResult.ImpactPoint, DamageImpulse);
	
	if (NodeData && OwnerCharacter->IsPlayerControlled())
	{
		ApplyHitFeedback(NodeData->HitFeedback, HitActor);
	}
}
//=====================================================================================================
// 피격시 카메라 효과
//=====================================================================================================
// 카메라 쉐이크 적용
void UCombatBaseComponent::ApplyHitFeedback(const FHitFeedbackData& Feedback, AActor* HitActor)
{
	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (Feedback.CameraShake && PC)
	{
		PC->ClientStartCameraShake(Feedback.CameraShake, Feedback.ShakeScale);
	}
	const FVector HitLocation = HitActor ? HitActor->GetActorLocation() : OwnerCharacter->GetActorLocation();
	if (Feedback.HitEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, Feedback.HitEffect, HitLocation);
	}
	if (Feedback.HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(World, Feedback.HitSound, HitLocation);
	}
	if (Feedback.bUseHitStop && Feedback.HitStopDuration > 0.f)
	{
		World->GetTimerManager().ClearTimer(HitStopTimerHandle);
		UGameplayStatics::SetGlobalTimeDilation(World, Feedback.HitStopTimeScale);
		World->GetTimerManager().SetTimer(HitStopTimerHandle, this, &UCombatBaseComponent::ResetHitStop, Feedback.HitStopDuration, false);
	}
}
void UCombatBaseComponent::ResetHitStop()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::SetGlobalTimeDilation(World, 1.f);
	}
}
//=====================================================================================================
// 데이터 테이블 row 빼는 함수
//=====================================================================================================
const FAttackNodeData* UCombatBaseComponent::GetCurrentAttackNodeData() const
{
	const FAttackDataRow* AttackDataRow = GetAttackDataByRow(CurrentAttackRowName);
	if (!AttackDataRow || !AttackDataRow->Nodes.IsValidIndex(CurrentAttackNodeIndex)) return nullptr;
	return &AttackDataRow->Nodes[CurrentAttackNodeIndex];
}
// 적 전용 dt 셋팅 
const FAttackDataRow* UCombatBaseComponent::GetAttackDataByRow(FName AttackRowName) const
{
	if (!AttackDataTable || AttackRowName.IsNone()) return nullptr;
	return  AttackDataTable->FindRow<FAttackDataRow>(AttackRowName, TEXT("GetAttackData"));
}

void UCombatBaseComponent::SetAttackDataTable(UDataTable* NewTable)
{
	AttackDataTable = NewTable;
}

