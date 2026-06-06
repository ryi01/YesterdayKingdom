// Fill out your copyright notice in the Description page of Project Settings.


#include "FSMStateComponent.h"

#include "AIController.h"
#include "EnemyBase.h"
#include "EnemyDefinition.h"
#include "EnemyFSMControllerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UFSMStateComponent::UFSMStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UFSMStateComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFSMStateComponent::InitializeState(AEnemyBase* InOwnerEnemy, UEnemyFSMControllerComponent* InFSMOwner)
{
	OwnerCharacter = InOwnerEnemy;
	
	FSMController = InFSMOwner;
	
	EnemyDefinition = OwnerCharacter->GetEnemyDefinition();
}

void UFSMStateComponent::OnStateEnter()
{
}

void UFSMStateComponent::OnStateUpdate(float)
{
}

void UFSMStateComponent::OnStateExit()
{
	
}

APawn* UFSMStateComponent::GetTargetPlayer() const
{
	if (!OwnerCharacter) return nullptr;
	return UGameplayStatics::GetPlayerPawn(OwnerCharacter, 0);
}

float UFSMStateComponent::GetDistanceToPlayer() const
{
	APawn* PlayerPawn = GetTargetPlayer();
	if (!PlayerPawn) return TNumericLimits<float>::Max();
	return GetDistanceToTarget(PlayerPawn);
}

float UFSMStateComponent::GetDistance2DToPlayer() const
{
	APawn* PlayerPawn = GetTargetPlayer();
	if (!PlayerPawn) return TNumericLimits<float>::Max();

	return FVector::Dist2D(
		OwnerCharacter->GetActorLocation(),
		PlayerPawn->GetActorLocation()
	);
}

float UFSMStateComponent::GetDistanceToHomeLocation() const
{
	if (!OwnerCharacter) return TNumericLimits<float>::Max();
	return FVector::Dist(OwnerCharacter->GetActorLocation(), OwnerCharacter->GetHomeLocation());
}

float UFSMStateComponent::GetDistanceToTarget(AActor* TargetActor) const
{
	if (!OwnerCharacter || !TargetActor) return TNumericLimits<float>::Max();
	return FVector::Dist(OwnerCharacter->GetActorLocation(), TargetActor->GetActorLocation());
}


bool UFSMStateComponent::IsPlayerValid() const
{
	return GetTargetPlayer() != nullptr;
}

bool UFSMStateComponent::IsPlayerInDetectRange() const
{
	if (!EnemyDefinition) return false;
	const float Distance = GetDistanceToPlayer();
	
	return Distance <= EnemyDefinition->DetectRange;
}

bool UFSMStateComponent::IsPlayerInAttackRange() const
{
	if (!EnemyDefinition) return false;
	const float Distance = GetDistanceToPlayer();
	return Distance <= EnemyDefinition->AttackRange;
}

bool UFSMStateComponent::IsPlayerLost(float LoseTargetMultiplier) const
{
	if (!EnemyDefinition) return true;
	const float Distance = GetDistanceToPlayer();
	const float LoseTargetRange = EnemyDefinition->DetectRange * LoseTargetMultiplier;
	return Distance > LoseTargetRange;
}

bool UFSMStateComponent::IsNearHomeLocation(float AcceptanceRadius) const
{
	return GetDistanceToHomeLocation() <= AcceptanceRadius;
}

bool UFSMStateComponent::IsTooFarFromHome(float MaxDistance) const
{
	return GetDistanceToHomeLocation() > MaxDistance;
}

bool UFSMStateComponent::IsOwnerDead() const
{
	if (!OwnerCharacter) return true;
	return OwnerCharacter->IsDead();
}

void UFSMStateComponent::MoveToTarget(AActor* TargetActor, float AcceptanceRadius)
{
	if (!OwnerCharacter || !TargetActor) return;
	AAIController* AIController = Cast<AAIController>(OwnerCharacter->GetController());
	if (!AIController) return;
	AIController->MoveToActor(TargetActor, AcceptanceRadius);
}

void UFSMStateComponent::MoveToPlayer(float AcceptanceRadius)
{
	APawn* PlayerPawn = GetTargetPlayer();
	if (!PlayerPawn) return;
	MoveToTarget(PlayerPawn, AcceptanceRadius);
}

void UFSMStateComponent::MoveToLocation(const FVector& TargetLocation, float AcceptanceRadius, bool bCanStrafe)
{
	if (!OwnerCharacter) return;
	AAIController* AIController = Cast<AAIController>(OwnerCharacter->GetController());
	if (!AIController) return;
	AIController->MoveToLocation(TargetLocation, AcceptanceRadius, false, true, true, bCanStrafe, nullptr, true);
}

void UFSMStateComponent::StopMove()
{
	if (!OwnerCharacter) return;
	AAIController* AIController = Cast<AAIController>(OwnerCharacter->GetController());
	if (!AIController) return;
	AIController->StopMovement();
	if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->Velocity = FVector::ZeroVector;
		MovementComponent->ClearAccumulatedForces();
	}
}

void UFSMStateComponent::FacePlayerInstant()
{
	if (!OwnerCharacter) return;

	APawn* PlayerPawn = GetTargetPlayer();
	if (!PlayerPawn) return;

	FVector Direction = PlayerPawn->GetActorLocation() - OwnerCharacter->GetActorLocation();
	Direction.Z = 0.f;

	if (Direction.IsNearlyZero()) return;

	const FRotator LookRotation = Direction.Rotation();
	OwnerCharacter->SetActorRotation(LookRotation);
}

void UFSMStateComponent::SetFocusToPlayer()
{
	APawn* PlayerPawn = GetTargetPlayer();
	if (!PlayerPawn) return;

	SetFocusTarget(PlayerPawn);
}

void UFSMStateComponent::SetFocusTarget(AActor* TargetActor)
{
	if (!OwnerCharacter || !TargetActor) return;

	AAIController* AIController = Cast<AAIController>(OwnerCharacter->GetController());
	if (!AIController) return;

	AIController->SetFocus(TargetActor, EAIFocusPriority::Gameplay);
}

void UFSMStateComponent::ClearFocusTarget()
{
	if (!OwnerCharacter) return;

	AAIController* AIController = Cast<AAIController>(OwnerCharacter->GetController());
	if (!AIController) return;

	AIController->ClearFocus(EAIFocusPriority::Gameplay);
}

void UFSMStateComponent::SetRootMotionFromMontage(bool bEnabled)
{
	if (!OwnerCharacter) return;
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	AnimInstance->SetRootMotionMode(bEnabled ? ERootMotionMode::RootMotionFromMontagesOnly : ERootMotionMode::IgnoreRootMotion);
	
}

AEnemyBase* UFSMStateComponent::GetOwnerEnemy() const
{
	return OwnerCharacter;
}

UEnemyFSMControllerComponent* UFSMStateComponent::GetFSMController() const
{
	return FSMController;
}



