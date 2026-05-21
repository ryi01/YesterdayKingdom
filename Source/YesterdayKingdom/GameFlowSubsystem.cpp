// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFlowSubsystem.h"

void UGameFlowSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CurrentState = EGameFlowState::Title;
}

void UGameFlowSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UGameFlowSubsystem::SetGameFlowState(EGameFlowState NewState)
{
	// 새로운 State가 현재 State와 같다면 반환
	if (CurrentState == NewState) return;
	// 아니라면 현재 State를 이전 State에 등록하고
	const EGameFlowState PrevState = CurrentState;
	// 현재 State를 새로운 State로 변환한다.
	CurrentState = NewState;
	// 이전 상태와 이후 상태를 뿌린다
	OnGameFlowStateChanged.Broadcast(PrevState, CurrentState);
}
