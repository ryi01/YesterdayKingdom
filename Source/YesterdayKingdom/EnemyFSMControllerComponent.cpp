// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSMControllerComponent.h"

#include "FSMStateComponent.h"

// Sets default values for this component's properties
UEnemyFSMControllerComponent::UEnemyFSMControllerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UEnemyFSMControllerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UEnemyFSMControllerComponent::InitializeFSM(AEnemyBase* InOwnerEnemy)
{
	OwnerCharacter = InOwnerEnemy;
	if (!OwnerCharacter) UE_LOG(LogTemp, Warning, TEXT("[EnemyFSMController] OwnerEnemy is null."));
}

void UEnemyFSMControllerComponent::RegisterState(EEnemyFSMStateType StateType, UFSMStateComponent* StateComponent)
{
	if (StateType == EEnemyFSMStateType::None || !OwnerCharacter ||!StateComponent) return;
	StateComponent->InitializeState(OwnerCharacter, this);
	StateMap.Add(StateType, StateComponent);
}

void UEnemyFSMControllerComponent::StartFSM(EEnemyFSMStateType StartStateType)
{
	if (!OwnerCharacter || !FindState(StartStateType)) return;
	bIsRunning = true;
	ChangeState(StartStateType);
}

void UEnemyFSMControllerComponent::TickFSM(float DeltaTime)
{
	if (!bIsRunning || !CurrentStateComponent) return;
	StateElapsedTime += DeltaTime;
	CurrentStateComponent->OnStateUpdate(DeltaTime);
}

void UEnemyFSMControllerComponent::StopFSM()
{
	if (CurrentStateComponent) CurrentStateComponent->OnStateExit();
	bIsRunning = false;
	PreviousStateType = CurrentStateType;
	CurrentStateType = EEnemyFSMStateType::None;
	CurrentStateComponent = nullptr;
}

void UEnemyFSMControllerComponent::ChangeState(EEnemyFSMStateType NewStateType)
{
	if (!bIsRunning || CurrentStateType == NewStateType) return;
	UFSMStateComponent* NewState = FindState(NewStateType);
	if (!NewState) return;
	
	if (CurrentStateComponent) CurrentStateComponent->OnStateExit();
	
	PreviousStateType = CurrentStateType;
	CurrentStateType = NewStateType;
	CurrentStateComponent = NewState;
	StateElapsedTime = 0.f;

	CurrentStateComponent->OnStateEnter();
}

// ========================================================
// 맵핑된 state 찾기
// ========================================================
UFSMStateComponent* UEnemyFSMControllerComponent::FindState(EEnemyFSMStateType StateType) const
{
	const TObjectPtr<UFSMStateComponent>* FoundState = StateMap.Find(StateType);
	if (!FoundState) return nullptr;
	return FoundState->Get();
}

// ========================================================
// Getter
// ========================================================
EEnemyFSMStateType UEnemyFSMControllerComponent::GetCurrentStateType() const
{
	return CurrentStateType;
}

EEnemyFSMStateType UEnemyFSMControllerComponent::GetPreviousStateType() const
{
	return PreviousStateType;
}

bool UEnemyFSMControllerComponent::IsRunning() const
{
	return bIsRunning;
}

AEnemyBase* UEnemyFSMControllerComponent::GetOwnerEnemy() const
{
	return OwnerCharacter;
}

float UEnemyFSMControllerComponent::GetStateElapsedTime() const
{
	return StateElapsedTime;
}

