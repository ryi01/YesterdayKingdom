// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpAttackStateComponent.h"

#include "CombatBaseComponent.h"
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
	if (!PlayerPawn)
	{
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;
	}
	JumpTargetLocation = PlayerPawn->GetActorLocation();
	if (!InitializeJumpAttackFromData())
	{
		FSMController->ChangeState(EEnemyFSMStateType::Chase);
		return;
	}
	UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent();
	if (!CombatComp)
	{
		FSMController->ChangeState(EEnemyFSMStateType::Chase);
		return;
	}
	StopMove();
	if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
	}
	
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) 
	{
		FSMController->ChangeState(EEnemyFSMStateType::Idle);
		return;
	}
	
	FacePlayerInstant();
	SetRootMotionFromMontage(false);
	
	OwnerCharacter->OnAttackCompleted.Unbind();
	OwnerCharacter->OnAttackCompleted.BindUObject(this, &UJumpAttackStateComponent::HandleAttackCompleted);

	CombatComp->RequestAttackByRow(CurrentAttackRowName);

	OwnerCharacter->LaunchCharacter(FVector(0.f, 0.f, CurrentJumpUpPower), false, true);

	OwnerCharacter->ClearSelectedAttackRowName();
	
	UE_LOG(LogTemp, Warning, TEXT("[FSM][JumpAttack] Enter : %s / Distance = %.2f"),
		*OwnerCharacter->GetName(),
		GetDistanceToPlayer());
}
bool UJumpAttackStateComponent::InitializeJumpAttackFromData()
{
	if (!OwnerCharacter) return false;

	UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent();
	if (!CombatComp) return false;

	CurrentAttackRowName = OwnerCharacter->GetSelectedAttackRowName();

	if (CurrentAttackRowName.IsNone()) return false;

	CurrentAttackDataRow = CombatComp->GetAttackDataByRow(CurrentAttackRowName);
	if (!CurrentAttackDataRow || !CurrentAttackDataRow->Montage) return false;
	
	CurrentJumpUpPower = CurrentAttackDataRow->JumpUpPower;
	CurrentJumpForwardPower = CurrentAttackDataRow->JumpForwardPower;
	CurrentAttackTriggerDistance = CurrentAttackDataRow->AttackTriggerDistance;
	CurrentAttackTriggerHeight = CurrentAttackDataRow->AttackTriggerHeight;
	CurrentMaxJumpAttackTime = CurrentAttackDataRow->MaxJumpAttackTime;

	if (CurrentMaxJumpAttackTime <= 0.f)
	{
		CurrentMaxJumpAttackTime = 3.f;
	}

	return true;
	
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

	if (ElapsedTime >= CurrentMaxJumpAttackTime)
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

	if (OwnerCharacter)
	{
		OwnerCharacter->OnAttackCompleted.Unbind();
		OwnerCharacter->ClearSelectedAttackRowName();

		if (UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent())
		{
			CombatComp->EndAttackTrace();
			CombatComp->ResetAttackState();
		}
	}

	ClearFocusTarget();

	ElapsedTime = 0.f;
	bDidJumpToPlayer = false;
	bStartedAttack = false;
	CurrentStep = EJumpAttackStep::None;

	CurrentAttackRowName = NAME_None;
	CurrentAttackDataRow = nullptr;
	JumpTargetLocation = FVector::ZeroVector;

	UE_LOG(LogTemp, Log, TEXT("[FSM][JumpAttack] Exit"));
}


void UJumpAttackStateComponent::TryStartAttackSection()
{
	if (bStartedAttack) return;
	if (!OwnerCharacter || !FSMController) return;

	const FVector OwnerLocation = OwnerCharacter->GetActorLocation();

	const float DistanceToTarget = FVector::Dist2D(OwnerLocation, JumpTargetLocation);
	const float HeightDiff = FMath::Abs(OwnerLocation.Z - JumpTargetLocation.Z);
	UE_LOG(LogTemp, Warning, TEXT("[FSM][JumpAttack] TryStartAttack / TargetDist = %.2f / Height = %.2f / TriggerDist = %.2f / TriggerHeight = %.2f"),
		DistanceToTarget,
		HeightDiff,
		CurrentAttackTriggerDistance,
		CurrentAttackTriggerHeight);
	if (DistanceToTarget <= CurrentAttackTriggerDistance && HeightDiff <= CurrentAttackTriggerHeight)
	{
		StartAttackSection();
	}
}

void UJumpAttackStateComponent::StartAttackSection()
{
	if (bStartedAttack) return;
	if (!OwnerCharacter || !FSMController) return;
	
	UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent();
	if (!CombatComp) return;
	
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	bStartedAttack = true;
	CurrentStep = EJumpAttackStep::Attack;
	UE_LOG(LogTemp, Warning, TEXT("[FSM][JumpAttack] StartAttackSection Called"));
	if (!CombatComp->JumpToNextAttackSection())
	{
		FSMController->ChangeState(NextState);
	}
	UE_LOG(LogTemp, Warning, TEXT("[FSM][JumpAttack] SpinMove -> Attack Success"));
}

void UJumpAttackStateComponent::HandleAttackCompleted()
{
	if (!FSMController) return;

	UE_LOG(LogTemp, Warning, TEXT("[FSM][JumpAttack] Attack Completed"));

	FSMController->ChangeState(NextState);
}
void UJumpAttackStateComponent::NotifyJumpToPlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("[FSM][JumpAttack] NotifyJumpToPlayer Called"));
	if (bDidJumpToPlayer) return;
	if (!OwnerCharacter || !FSMController) return;
	
	UCombatBaseComponent* CombatComp = OwnerCharacter->GetCombatComponent();
	if (!CombatComp) return;
	
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	bDidJumpToPlayer = true;
	CurrentStep = EJumpAttackStep::Flying;
	// JumpStart에서 spinMove로 변경
	if (!CombatComp->JumpToNextAttackSection())
	{
		UE_LOG(LogTemp, Warning, TEXT("[FSM][JumpAttack] Failed JumpStart -> SpinMove"));
		FSMController->ChangeState(NextState);
		return;
	}
	
	// 앞으로 이동은 RootMotion 끄고 코드로 제어
	SetRootMotionFromMontage(false);
	
	FVector JumpDirection = JumpTargetLocation - OwnerCharacter->GetActorLocation();
	JumpDirection.Z = 0.f;

	if (JumpDirection.IsNearlyZero())
	{
		JumpDirection = OwnerCharacter->GetActorForwardVector();
		JumpDirection.Z = 0.f;
	}

	JumpDirection.Normalize();

	const FVector LaunchVelocity = JumpDirection * CurrentJumpForwardPower;
	
	OwnerCharacter->LaunchCharacter(LaunchVelocity, true, false);

	ClearFocusTarget();
	UE_LOG(LogTemp, Warning, TEXT("[FSM][JumpAttack] Jump To Player / ForwardPower = %.2f"),
		CurrentJumpForwardPower);
	
}