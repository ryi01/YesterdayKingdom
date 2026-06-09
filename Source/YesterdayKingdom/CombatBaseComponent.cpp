// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatBaseComponent.h"
#include "Damagable.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "BaseCharacter.h"
#include "BaseStatComponent.h"
#include "Animation/AnimMontage.h"
#include "CommonEnumTypes.h"
#include "Kismet/GameplayStatics.h"

UCombatBaseComponent::UCombatBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(false);
}

// Called when the game starts
void UCombatBaseComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
}

void UCombatBaseComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsCharging)
	{
		UpdateCharge(DeltaTime);
	}
}

//=====================================================================================================
// 입력값이 들어오면 실행되는 함수 
//=====================================================================================================
bool UCombatBaseComponent::RequestAttackByRow(FName AttackRowName)
{
	if (!OwnerCharacter || !AttackDataTable) return false;
	if (AttackRowName.IsNone()) return false;
	if (!CurrentAttackRowName.IsNone())
	{
		if (CurrentAttackRowName == AttackRowName)
		{
			bComboInputBuffered = true;
			return true;
		}
		return false;
	}
	const FAttackDataRow* AttackDataRow = AttackDataTable->FindRow<FAttackDataRow>(AttackRowName, TEXT("RequestAttack"));
	if (!AttackDataRow || !AttackDataRow->Montage || AttackDataRow->Nodes.Num() <= 0) return false;
	
	CurrentAttackRowName = AttackRowName;
	CurrentAttackNodeIndex = 0;
	bComboInputBuffered = false;
	
	const FAttackNodeData* FirstNode = AttackDataRow->Nodes.IsValidIndex(CurrentAttackNodeIndex) ? &AttackDataRow->Nodes[CurrentAttackNodeIndex] : nullptr;
	
	if (!FirstNode)
	{
		ResetAttackState();
		return false;
	}
	
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		ResetAttackState();
		return false;
	}
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UCombatBaseComponent::OnAttackMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackDataRow->Montage);
	const float PlayedLength = OwnerCharacter->PlayAnimMontage(AttackDataRow->Montage, 1.f, FirstNode->SectionName);
	if (PlayedLength <= 0.f)
	{
		ResetAttackState();
		return false;
	}
	return true;
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
	
	bIsCharging = false;
	ChargeStartTime = 0.f;
	CurrentChargeRatio = 1.f;
	CurrentChargeRowName = NAME_None;
	
	SetComponentTickEnabled(false);
	
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

bool UCombatBaseComponent::JumpToNextAttackSection()
{
	if (!OwnerCharacter) return false;
	const FAttackDataRow* AttackDataRow = GetAttackDataByRow(CurrentAttackRowName);
	if (!AttackDataRow || !AttackDataRow->Montage) return false;
	// 사용되는 Row에서 NodeData 추출
	const FAttackNodeData* CurrentNode = GetCurrentAttackNodeData();
	if (!CurrentNode)
	{
		return false;
	}
	if (CurrentNode->NextIndex == INDEX_NONE || !AttackDataRow->Nodes.IsValidIndex(CurrentNode->NextIndex))
	{
		bComboInputBuffered = false;
		return false;
	}
	// 현재 실행되는 콤보의 인덱스를 올리고
	CurrentAttackNodeIndex = CurrentNode->NextIndex;
	const int32 PreviousIndex = CurrentAttackNodeIndex;
	const int32 NextIndex = CurrentNode->NextIndex;
	const FAttackNodeData* NextNode = GetCurrentAttackNodeData();
	if (!NextNode)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Combat][JumpToNext] Failed / NextNode None / PrevIndex=%d / NextIndex=%d"),
			PreviousIndex,
			NextIndex);
		return false;
	}

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return false;
	const bool bIsPlaying = AnimInstance->Montage_IsPlaying(AttackDataRow->Montage);
	const bool bHasNextSection = AttackDataRow->Montage->IsValidSectionName(NextNode->SectionName);

	UE_LOG(LogTemp, Warning,
		TEXT("[Combat][JumpToNext] NextNode / PrevIndex=%d / NextIndex=%d / NextSection=%s / Montage=%s / IsPlaying=%d / HasSection=%d"),
		PreviousIndex,
		NextIndex,
		*NextNode->SectionName.ToString(),
		*AttackDataRow->Montage->GetName(),
		bIsPlaying,
		bHasNextSection);

	if (!bHasNextSection)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Combat][JumpToNext] Failed / Montage Section Not Found / Section=%s / Montage=%s"),
			*NextNode->SectionName.ToString(),
			*AttackDataRow->Montage->GetName());
		return false;
	}

	if (!bIsPlaying)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Combat][JumpToNext] Warning / Montage Not Playing Before Jump / Montage=%s"),
			*AttackDataRow->Montage->GetName());
	}
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UCombatBaseComponent::OnAttackMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackDataRow->Montage);
	// 애니메이션을 실행한다
	AnimInstance->Montage_JumpToSection(NextNode->SectionName, AttackDataRow->Montage);
	const FName CurrentSectionAfterJump = AnimInstance->Montage_GetCurrentSection(AttackDataRow->Montage);

	UE_LOG(LogTemp, Warning,
		TEXT("[Combat][JumpToNext] Success / JumpedTo=%s / CurrentSectionAfterJump=%s / CurrentIndex=%d"),
		*NextNode->SectionName.ToString(),
		*CurrentSectionAfterJump.ToString(),
		CurrentAttackNodeIndex);

	return true;
}
//=====================================================================================================
// 차지 공격 관련 함수
//=====================================================================================================
void UCombatBaseComponent::OnChargeAttackStarted()
{
}

void UCombatBaseComponent::OnChargeAttackReleased()
{
}
bool UCombatBaseComponent::StartChargeAttackByRow(FName AttackRowName)
{
	if (!OwnerCharacter || !AttackDataTable) return false;
	if (AttackRowName.IsNone()) return false;
	if (bIsGuarding) return false;
	if (!CurrentAttackRowName.IsNone()) return false;
	
	const FAttackDataRow* AttackDataRow = GetAttackDataByRow(AttackRowName);
	if (!AttackDataRow || !AttackDataRow->Montage || AttackDataRow->Nodes.Num() <= 0) return false;
	
	if (AttackDataRow->StaminaCost > 0.f)
	{
		if (!OwnerCharacter->GetStatComponent() || !OwnerCharacter->GetStatComponent()->ConsumeST(AttackDataRow->StaminaCost)) return false;
	}
	if (AttackDataRow->MPCost > 0.f)
	{
		if (!OwnerCharacter->GetStatComponent() || !OwnerCharacter->GetStatComponent()->ConsumeMP(AttackDataRow->MPCost)) return false;
	}
	
	bIsCharging = true;
	ChargeStartTime = GetWorld()->GetTimeSeconds();
	CurrentChargeRatio = 0.f;
	CurrentChargeRowName = AttackRowName;
	
	SetComponentTickEnabled(true);
	
	OnChargeAttackStarted();
	
	const bool bRequested = RequestAttackByRow(AttackRowName);
	if (!bRequested)
	{
		bIsCharging = false;
		ChargeStartTime = 0.f;
		CurrentChargeRatio = 1.f;
		CurrentChargeRowName = NAME_None;
		SetComponentTickEnabled(false);
		return false;
	}

	return true;
}

void UCombatBaseComponent::CancelChargeAttack()
{
	if (!OwnerCharacter || !bIsCharging) return;
	const FAttackDataRow* AttackDataRow = GetAttackDataByRow(CurrentChargeRowName);

	if (AttackDataRow && AttackDataRow->Montage)
	{
		if (UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Stop(0.15f, AttackDataRow->Montage);
		}
	}
	
	bIsCharging = false;
	ChargeStartTime = 0.f;
	CurrentChargeRatio = 1.f;
	CurrentChargeRowName = NAME_None;
	
	SetComponentTickEnabled(false);
	
	ResetAttackState();
	
}
void UCombatBaseComponent::ReleaseChargeAttack()
{
	if (!OwnerCharacter || !bIsCharging) return;
	if (CurrentChargeRowName.IsNone()) return;

	const FAttackDataRow* AttackDataRow = GetAttackDataByRow(CurrentChargeRowName);
	if (!AttackDataRow || !AttackDataRow->Montage) return;

	CurrentChargeRatio = CalculateChargeRatio(AttackDataRow);

	bIsCharging = false;
	SetComponentTickEnabled(false);

	OnChargeAttackReleased();
	
	JumpToNextAttackSection();
}

void UCombatBaseComponent::UpdateCharge(float DeltaTime)
{
	if (!OwnerCharacter || CurrentChargeRowName.IsNone()) return;
	const FAttackDataRow* AttackDataRow = GetAttackDataByRow(CurrentChargeRowName);
	if (!AttackDataRow) return;

	UBaseStatComponent* StatComponent = OwnerCharacter->GetStatComponent();
	if (!StatComponent) return;
	
	CurrentChargeRatio = CalculateChargeRatio(AttackDataRow);
	const float STCost = AttackDataRow->StaminaCost * DeltaTime;
	const float MPCost = AttackDataRow->MPCost * DeltaTime;
	
	if (STCost > 0.f)
	{
		if (!StatComponent->ConsumeST(STCost))
		{
			if (AttackDataRow->bReleaseWhenChargeResourceEmpty)
			{
				ReleaseChargeAttack();
			}
			else
			{
				CancelChargeAttack();
			}
			return;
		}
	}
	if (MPCost > 0.f)
	{
		if (!StatComponent->ConsumeMP(MPCost))
		{
			if (AttackDataRow->bReleaseWhenChargeResourceEmpty)
			{
				ReleaseChargeAttack();
			}
			else
			{
				CancelChargeAttack();
			}
			return;
		}
	}
}

float UCombatBaseComponent::CalculateChargeRatio(const FAttackDataRow* AttackDataRow) const
{
	if (!AttackDataRow || !GetWorld()) return 0.f;
	const float MaxChargeTime = FMath::Max(AttackDataRow->MaxChargeTime, 0.01f);
	const float HeldTime = GetWorld()->GetTimeSeconds() - ChargeStartTime;
	
	return FMath::Clamp(HeldTime / MaxChargeTime, 0.f, 1.f);
}

//=====================================================================================================
// 가드 관련
//=====================================================================================================

void UCombatBaseComponent::ApplyParryHitStop()
{
	if (!bUseParryHitStop) return;
	if (ParryHitStopDuration <= 0.f) return;
	GetWorld()->GetTimerManager().ClearTimer(HitStopTimerHandle);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), ParryHitStopTimeScale);
	GetWorld()->GetTimerManager().SetTimer(HitStopTimerHandle, this, &UCombatBaseComponent::ResetHitStop, ParryHitStopDuration, false);
}

void UCombatBaseComponent::StartGuard()
{
	if (!CanStartGuard()) return;
	bIsGuarding = true;
	bCanParry = true;
	OnGuardStarted();

	GetWorld()->GetTimerManager().ClearTimer(ParryTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(ParryTimerHandle, this, &UCombatBaseComponent::CloseParryWindow, ParryWindow, false);
}
void UCombatBaseComponent::CloseParryWindow()
{
	bCanParry = false;
}

void UCombatBaseComponent::EndGuard()
{
//
	if (!bIsGuarding) return;
	
	bIsGuarding = false;
	bCanParry = false;
	
	GetWorld()->GetTimerManager().ClearTimer(ParryTimerHandle);

	OnGuardEnded();
}
void UCombatBaseComponent::OnGuardStarted()
{
}

void UCombatBaseComponent::OnGuardEnded()
{
}

void UCombatBaseComponent::OnParrySuccess(AActor* DamageCauser)
{
	if (!OwnerCharacter) return;
	UE_LOG(LogTemp, Warning, TEXT("Success Parry"));
	ApplyParryHitStop();

	if (ParrySuccessMontage)
	{
		OwnerCharacter->PlayAnimMontage(ParrySuccessMontage);
	}
}


bool UCombatBaseComponent::TryHandleGuardOrParry(float& InOutDamage, AActor* DamageCauser)
{
	if (!OwnerCharacter || !bIsGuarding) return false;
	if (!IsGuardDirectionValid(DamageCauser)) return false;
	UBaseStatComponent* StatComponent = OwnerCharacter->GetStatComponent();
	if (!StatComponent) return false;
	
	if (bCanParry)
	{
		InOutDamage = 0.f;
		
		if (ParrySTRecover > 0.f) StatComponent->RecoverST(ParrySTRecover);
		
		EndGuard();
		OnParrySuccess(DamageCauser);
		
		return true;
	}
	
	if (GuardSTCostPerHit > 0.f)
	{
		if (!StatComponent->ConsumeST(GuardSTCostPerHit))
		{
			EndGuard();
			return false;
		}
	}

	InOutDamage *= GuardDamageRate;

	return true;
}

bool UCombatBaseComponent::CanStartGuard() const
{
	if (!OwnerCharacter) return false;
	if (bIsGuarding) return false;
	if (!CurrentAttackRowName.IsNone()) return false;
	if (bIsCharging) return false;
	
	return true;
}

bool UCombatBaseComponent::IsGuardDirectionValid(AActor* DamageCauser) const
{
	if (!OwnerCharacter || !DamageCauser) return false;
	const FVector ToAttacker = DamageCauser->GetActorLocation() - OwnerCharacter->GetActorLocation();
	const FVector ToAttacker2D = FVector(ToAttacker.X, ToAttacker.Y, 0.f).GetSafeNormal();
	
	const FVector Forward = OwnerCharacter->GetActorForwardVector();
	const FVector Forward2D = FVector(Forward.X, Forward.Y, 0.f).GetSafeNormal();
	
	const float Dot = FVector::DotProduct(Forward2D, ToAttacker2D);
	
	return Dot >= 0.3f;
}

bool UCombatBaseComponent::IsGuarding() const
{
	return bIsGuarding;
}

bool UCombatBaseComponent::CanParry() const
{
	return bCanParry;
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
	HitResults.Sort([&Start](const FHitResult& A, const FHitResult& B)
	{
		return FVector::DistSquared(Start, A.ImpactPoint) < FVector::DistSquared(Start, B.ImpactPoint);
	});
	
	const float HalfAngle = TraceAngle * 0.5f;
	const float MinDot = FMath::Cos(FMath::DegreesToRadians(HalfAngle));
	const FVector Forward = OwnerCharacter->GetActorForwardVector();
	
	// 피격 대상이 있다면
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValidHitActor(HitActor)) continue;
		const FVector ToTarget = HitActor->GetActorLocation() - OwnerCharacter->GetActorLocation();
		const FVector ToTarget2D = FVector(ToTarget.X, ToTarget.Y, 0.f).GetSafeNormal();
		
		const FVector Forward2D = FVector(Forward.X, Forward.Y, 0.f).GetSafeNormal();
		
		const float Dot = FVector::DotProduct(ToTarget2D, Forward2D );
		if (Dot < MinDot) continue;;
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
	
	const float BaseDamage = NodeData ? NodeData->Damage : DefaultDamage;
	
	float AttackBonus = 0.f;
	if (UBaseStatComponent* StatComponent = OwnerCharacter->GetStatComponent())
	{
		AttackBonus = StatComponent->GetFinalAttack();
	}
	
	float Damage = BaseDamage + AttackBonus;
	if (!CurrentChargeRowName.IsNone() && CurrentAttackRowName == CurrentChargeRowName)
	{
		const FAttackDataRow* AttackDataRow = GetAttackDataByRow(CurrentChargeRowName);
		if (AttackDataRow)
		{
			const float Multiplier = FMath::Lerp(AttackDataRow->MinChargeDamageMultiplier, AttackDataRow->MaxChargeDamageMultiplier, CurrentChargeRatio);
			Damage *= Multiplier;
		}
	}

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
//=====================================================================================================
// Getter
//=====================================================================================================
bool UCombatBaseComponent::IsAttacking() const
{
	return !CurrentAttackRowName.IsNone();
}

bool UCombatBaseComponent::IsCharging() const
{
	return bIsCharging;
}

const FAttackDataRow* UCombatBaseComponent::GetCurrentAttackDataRow() const
{
	if (CurrentAttackRowName.IsNone()) return nullptr;

	return GetAttackDataByRow(CurrentAttackRowName);
}


FName UCombatBaseComponent::GetCurrentAttackRowName() const
{
	return CurrentAttackRowName;
}

