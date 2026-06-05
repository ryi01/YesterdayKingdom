// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpAttackStateComponent.h"

#include "EnemyBase.h"
#include "EnemyFSMControllerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UJumpAttackStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	
	ElapsedTime = 0.f;
	bDidJumpToPlayer = false;
	bStartedAttack = false;
	CurrentStep = EJumpAttackStep::JumpStart;
	

	if (!OwnerCharacter || !FSMController) return;
	
	APawn* PlayerPawn = GetTargetPlayer();
	if (!PlayerPawn || !JumpAttackMontage)
	{
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;
	}
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) 
	{
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;
	}
	
	StopMove();

	if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
	}

	SetFocusToPlayer();
	SetMontageRootMotionEnabled(false);
	
	OwnerCharacter->PlayAnimMontage(JumpAttackMontage);
	AnimInstance->Montage_JumpToSection(JumpStartSectionName, JumpAttackMontage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UJumpAttackStateComponent::HandleMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, JumpAttackMontage);
	
	OwnerCharacter->LaunchCharacter(FVector(0.f, 0.f, JumpUpPower), false, true);
	
	UE_LOG(LogTemp, Warning, TEXT("[FSM][JumpAttack] Enter : %s / Distance = %.2f"),
		*OwnerCharacter->GetName(),
		GetDistanceToPlayer());
}

void UJumpAttackStateComponent::OnStateUpdate(float DeltaTime)
{
	Super::OnStateUpdate(DeltaTime);
	if (!FSMController || !OwnerCharacter) return;

	if (IsOwnerDead())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Dead);
		return;
	}

	ElapsedTime += DeltaTime;

	if (ElapsedTime >= MaxJumpAttackTime)
	{
		FSMController->ChangeState(NextState);
		return;
	}

	if (CurrentStep == EJumpAttackStep::Flying && !bStartedAttack)
	{
		TryStartAttackSection();
	}
	
}

void UJumpAttackStateComponent::OnStateExit()
{
	Super::OnStateExit();
	
	SetMontageRootMotionEnabled(false);
	ClearFocusTarget();

	ElapsedTime = 0.f;
	bDidJumpToPlayer = false;
	bStartedAttack = false;
	CurrentStep = EJumpAttackStep::None;
}

void UJumpAttackStateComponent::TryStartAttackSection()
{
	if (bStartedAttack) return;
	if (!OwnerCharacter || !FSMController) return;

	APawn* PlayerPawn = GetTargetPlayer();
	
	const float DistanceToPlayer = GetDistance2DToPlayer();
	const float HeightDiff = FMath::Abs(OwnerCharacter->GetActorLocation().Z - PlayerPawn->GetActorLocation().Z);

	if (DistanceToPlayer <= AttackTriggerDistance && HeightDiff <= AttackTriggerHeight)
	{
		StartAttackSection();
	}
}

void UJumpAttackStateComponent::StartAttackSection()
{
	if (bStartedAttack) return;
	if (!OwnerCharacter || !FSMController || !JumpAttackMontage) return;
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	const int32 SectionIndex = JumpAttackMontage->GetSectionIndex(AttackSectionName);
	const bool bIsPlaying = AnimInstance->Montage_IsPlaying(JumpAttackMontage);

	if (SectionIndex == INDEX_NONE) return;
	if (!bIsPlaying)
	{
		bStartedAttack = true;
		FSMController->ChangeState(NextState);
		return;
	}
	bStartedAttack = true;
	CurrentStep = EJumpAttackStep::Attack;

	AnimInstance->Montage_JumpToSection(AttackSectionName, JumpAttackMontage);
}

void UJumpAttackStateComponent::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != JumpAttackMontage) return;
	if (!FSMController) return;

	FSMController->ChangeState(NextState);
}

void UJumpAttackStateComponent::SetMontageRootMotionEnabled(bool bEnabled)
{
	if (!OwnerCharacter) return;
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	AnimInstance->SetRootMotionMode(bEnabled ? ERootMotionMode::RootMotionFromMontagesOnly : ERootMotionMode::IgnoreRootMotion);
	
}
void UJumpAttackStateComponent::NotifyJumpToPlayer()
{
	if (bDidJumpToPlayer) return;
	if (!OwnerCharacter || !FSMController || !JumpAttackMontage) return;

	APawn* PlayerPawn = GetTargetPlayer();
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	bDidJumpToPlayer = true;
	CurrentStep = EJumpAttackStep::Flying;
	// JumpStart에서 spinMove로 변경
	AnimInstance->Montage_JumpToSection(SpinMoveSectionName, JumpAttackMontage);
	
	// 앞으로 이동은 RootMotion 끄고 코드로 제어
	SetMontageRootMotionEnabled(false);
	
	FVector JumpDirection = PlayerPawn->GetActorLocation() - OwnerCharacter->GetActorLocation();
	JumpDirection.Z = 0.f;

	if (JumpDirection.IsNearlyZero())
	{
		JumpDirection = OwnerCharacter->GetActorForwardVector();
		JumpDirection.Z = 0.f;
	}

	JumpDirection.Normalize();

	const FVector LaunchVelocity = JumpDirection * JumpForwardPower;
	
	OwnerCharacter->LaunchCharacter(LaunchVelocity, true, false);

	ClearFocusTarget();
	
}