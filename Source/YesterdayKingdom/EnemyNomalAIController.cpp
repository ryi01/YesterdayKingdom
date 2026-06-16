// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyNomalAIController.h"

#include "EnemyNomal.h"
#include "EnemyDefinition.h"
#include "Components/StateTreeAIComponent.h"

AEnemyNomalAIController::AEnemyNomalAIController()
{
	StateTreeAI = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAI"));
	check(StateTreeAI); // 컴포넌트 추가가 안되어있을 경우 크러쉬 발생

	// 컨트롤러에 캐릭터 빙의 시 자동으로 StateTree 자동 시작
	bStartAILogicOnPossess = false;

	// AI컨트롤러와 AI 캐릭터간의 자동 빙의 수행
	bAttachToPawn = true; 
}

void AEnemyNomalAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	UE_LOG(LogTemp, Warning, TEXT("AI Possessed: %s"), *GetNameSafe(InPawn));
	
	AEnemyNomal* EnemyNomal = Cast<AEnemyNomal>(InPawn);
	if (!EnemyNomal) return;
	
	UEnemyDefinition* EnemyDefinition = EnemyNomal->GetEnemyDefinition();
	if (!EnemyDefinition || !EnemyDefinition->StateTree) return;
	
	if (!EnemyDefinition || !EnemyDefinition->StateTree)
	{
		UE_LOG(LogTemp, Error, TEXT("StateTree Missing"));
		return;
	}
	StateTreeAI->StopLogic(TEXT("Change StateTree"));
	StateTreeAI->SetStateTree(EnemyDefinition->StateTree);
	StateTreeAI->StartLogic();
	
	UE_LOG(LogTemp, Warning, TEXT("StateTree Started"));
}

void AEnemyNomalAIController::OnUnPossess()
{
	if (StateTreeAI)
	{
		StateTreeAI->StopLogic(TEXT("UnPossess"));
	}
	
	Super::OnUnPossess();
}

