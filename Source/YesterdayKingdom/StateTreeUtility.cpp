// Fill out your copyright notice in the Description page of Project Settings.

#include "StateTreeUtility.h"

#include "StateTreeExecutionContext.h" // Task/Condition 로직 수행을 위한 컨텍스트 객체 모듈 포함
#include "StateTreeExecutionTypes.h" // StateTree 실행에 필요한 각종 열거형, 구조체, 상태값 모듈 포함
#include "StateTreeAsyncExecutionContext.h" // 비동기 작업 중 데이터 유효성을 안전하게 검사하는 모듈 포함
#include "NavigationSystem.h"

#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BaseStatComponent.h"
#include "EnemyDefinition.h"
#include "EnemyNomal.h"
#include "Kismet/GameplayStatics.h"

// Condition(조건)의 실제 판정 처리 로직 해당 메소드의 결과값을 통해
// 상태 전환/진입의 허용 여부를 결정함
bool FStateTreeCharacterGroundedCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	// 현재 인스턴스에 매핑된 InstanceData를 참조함
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// 현재 컨텍스트에 연결된 캐릭터가 바닥에 착지 상태인지를 구함
	bool bCondition = InstanceData.Character->GetMovementComponent()->IsMovingOnGround();

	// 공중에 떠있는지를 체크하는 상태 옵션 켜져 있다면 바닥 착지 여부의 반대값을 반환함
	return InstanceData.bMustBeAir ? !bCondition : bCondition;
}

#if WITH_EDITOR
// StateTree 에디터에 표시될 설명 문자열 반환 (태그 형식 지원)
FText FStateTreeCharacterGroundedCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Is Character Grounded</b>");
}
#endif

// 최근에 플레이어가 공격을 했고 지정된 시야 범위 안쪽에 있는지를 판단
bool FStateTreeIsInDangerCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	// 스테이트 트리 컨텍스트 및 컨디션 구조체 접근용 컨텍스트를 통해 인스턴스데이터 참조
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// 컨텍스트를 통해 참조된 캐릭터가 존재한다면
	if (InstanceData.Character)
	{
		// 마직막 위험 감지 경과 시간
		const float ReactionDelta = InstanceData.Character->GetWorld()->GetTimeSeconds() -
									InstanceData.Character->GetLastDangerTime();

		// 위험 감지 시간 범위안에 들어갔다면 회피를 발동을 위한 시야 범위 판단을 수행함
		if (ReactionDelta < InstanceData.MaxReactionTime &&
			ReactionDelta > InstanceData.MinReactionTime)
		{
			// 위험 발생 위치를 향한 단위 벡터 (수평 성분만) 구함
			const FVector DangerDir = (InstanceData.Character->GetLastDangerLocation()
				- InstanceData.Character->GetActorLocation()).GetSafeNormal2D();

			// 이전의 판단된 위험 시선 방향과 현재 캐릭터의 시선 방향과의
			// 시야각을 내적을 이용해서 구함 (내적 -> 코사인값 반환)
			const float DangerDot = FVector::DotProduct(DangerDir, InstanceData.Character->GetActorForwardVector());

			// * 회피 기준 시야각 -> 라이안 -> 코싸인 ====> 내적각도(코싸인) 과 비교 가능
			// ConAngles : 시야각의 Cos값 (목적 : 내적결과값과 비교를 위해)
			const float ConAngles = FMath::Cos(FMath::DegreesToRadians(InstanceData.DangerSightConAngle));

			// 회피 기준 시야를 벗어났는지 판단
			// * DangerDot이 더 크면 시야안에 들어온 것으로 판단
			return DangerDot > ConAngles; 
		}
	}

	return false;
}

FText FStateTreeIsInDangerCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Is Character In Danger</b>");
}

// 현재 공격 테스크 시작 이벤트 (다른 상태로 전환되어 종료됨)
// - 콤보 공격 시작 + 종료 콜백 처리
EStateTreeRunStatus FStateTreeComboAttackTask::EnterState(FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& Transition) const
{
	// 다른 상태에서 전환된게 맞다면
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		// 컨텍스트 인스턴스 데이터를 참조함
		FInstanceDataType& InstanceData = context.GetInstanceData(*this);

		// AI 캐릭터의 공격 완료 람다 이벤트 메소드 정의
		InstanceData.Character->OnAttackCompleted.BindLambda(
			[WeakContext = context.MakeWeakExecutionContext()](){
				// 현재 상태를 성공으로 완료함 (상태 전환 진행)
				WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
			}
		);

		// AI 캐릭터의 콤보 공격 수행
		InstanceData.Character->DoAIComboAttack();
	}

	return EStateTreeRunStatus::Running;
}

// 현재 공격 테스크 종교 이벤트 (다른 상태로 전환되어 종료됨)
void FStateTreeComboAttackTask::ExitState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& Transition) const
{
	// 노드가 정상적으로 전환되어서 완료되었다면
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		FInstanceDataType& InstanceData = context.GetInstanceData(*this);

		// 람다 이벤트 연결(바인드)를 해제함
		InstanceData.Character->OnAttackCompleted.Unbind();
	}
}

FText FStateTreeComboAttackTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Do Combo Attack</b>");
}

// 현재 차지 공격 테스크 시작 이벤트 (다른 상태로 전환되어 종료됨)
// - 차지 공격 시작 + 종료 콜백 처리
EStateTreeRunStatus FStateTreeChargedAttackTask::EnterState(FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& Transition) const
{
	// 다른 상태에서 전환된게 맞다면
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		// 컨텍스트 인스턴스 데이터를 참조함
		FInstanceDataType& InstanceData = context.GetInstanceData(*this);

		// AI 캐릭터의 공격 완료 람다 이벤트 메소드 정의
		InstanceData.Character->OnAttackCompleted.BindLambda(
			[WeakContext = context.MakeWeakExecutionContext()]() {
				// 현재 상태를 성공으로 완료함 (상태 전환 진행)
				WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
			}
		);

		// AI 캐릭터의 차지 공격 수행
		InstanceData.Character->DoAIChargedAttack();
	}

	return EStateTreeRunStatus::Running;
}

// 현재 차지 공격 테스크 종료 이벤트 (다른 상태로 전환되어 종료됨)
void FStateTreeChargedAttackTask::ExitState(FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& Transition) const
{
	// 노드가 정상적으로 전환되어서 완료되었다면
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		FInstanceDataType& InstanceData = context.GetInstanceData(*this);

		// 람다 이벤트 연결(바인드)를 해제함
		InstanceData.Character->OnAttackCompleted.Unbind();
	}
}

FText FStateTreeChargedAttackTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Do Charged Attack</b>");
}

// 피격에 의한 넉백 테스크 시작 이벤트
EStateTreeRunStatus FStateTreeWaitForLandingTask::EnterState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& Transition) const
{
	// 이전 상태에서 정상적으로 전환되었다면
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		FInstanceDataType& InstanceData = context.GetInstanceData(*this);

		// 캐릭터의 바닥 착지 이벤트 델리이트의 람다 메소드를 연결함
		InstanceData.Character->OnEnemyLanded.BindLambda(
			[WeakContext = context.MakeWeakExecutionContext()]() {
				// 현재 상태를 성공으로 완료함 (상태 전환 진행)
				WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
			}
		);
	}

	return EStateTreeRunStatus::Running; // 현재 상태를 유지함
}

// 피격에 의한 넉백 테스크 종료 이벤트
void FStateTreeWaitForLandingTask::ExitState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& Transition) const
{
	// 노드가 정상적으로 전환되어서 완료되었다면
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		// 람다 이벤트 연결(바인드)를 해제함
		FInstanceDataType& InstanceData = context.GetInstanceData(*this);
		InstanceData.Character->OnEnemyLanded.Unbind();
	}
}

FText FStateTreeWaitForLandingTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Wait for Landing</b>");
}

// 플레이어 시선(감시) 처리 테스크 종료 이벤트
EStateTreeRunStatus FStateTreeFaceActorTask::EnterState(FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& Transition) const
{
	// 이전 상태에서 정상적으로 전환되었다면
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		FInstanceDataType& InstanceData = context.GetInstanceData(*this);

		// 지정한 액터 참조를 기준으로 시선(회전)을 수행함
		InstanceData.Controller->SetFocus(InstanceData.ActorToFaceTowards);
	}

	return EStateTreeRunStatus::Running;
}

// 플레이어 시선(감시) 처리 테스크 종료 이벤트
void FStateTreeFaceActorTask::ExitState(FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& Transition) const
{
	// 노드가 정상적으로 전환되어서 완료되었다면
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		FInstanceDataType& InstanceData = context.GetInstanceData(*this);

		// 포커스(시선주시) 해제
		InstanceData.Controller->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

FText FStateTreeFaceActorTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Face Towards Actor</b>");
}

// 위치를 향한 시선(감시) 처리 테스크 시작 이벤트
EStateTreeRunStatus FStateTreeFaceLocationTask::EnterState(FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& Transition) const
{
	// 이전 상태에서 정상적으로 전환되었다면
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		FInstanceDataType& InstanceData = context.GetInstanceData(*this);

		// 지정한 위치를 향한 시선(회전)을 수행함
		// * 이동이 없을 경우 내비게이션을 이용한 회전보다 더 가벼움
		InstanceData.Controller->SetFocalPoint(InstanceData.FaceLocation);
	}

	return EStateTreeRunStatus::Running;
}

// 위치를 향한 시선(감시) 처리 테스크 종료 이벤트
void FStateTreeFaceLocationTask::ExitState(FStateTreeExecutionContext& context,
	const FStateTreeTransitionResult& Transition) const
{
	// 노드가 정상적으로 전환되어서 완료되었다면
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		FInstanceDataType& InstanceData = context.GetInstanceData(*this);

		// 위치주시(시선주시) 해제
		InstanceData.Controller->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

FText FStateTreeFaceLocationTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Face Towards Location</b>");
}

// AI 캐릭터 속도 설정 테스크 시작 이벤트
EStateTreeRunStatus FStateTreeSetCharacterSpeedTask::EnterState(FStateTreeExecutionContext& context, const FStateTreeTransitionResult& Transition) const
{
	// 이전 상태에서 정상적으로 전환되었다면
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		FInstanceDataType& InstanceData = context.GetInstanceData(*this);

		// 캐릭터의 최대 이동 속도를 설정(변경)함
		InstanceData.Character->GetCharacterMovement()->MaxWalkSpeed = InstanceData.Speed;
	}

	return EStateTreeRunStatus::Running;
}

FText FStateTreeSetCharacterSpeedTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Set Character Speed</b>");
}

EStateTreeRunStatus FStateTreeGetPlayerInfoTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
		if (AEnemyNomal* EnemyNormal = Cast<AEnemyNomal>(InstanceData.Character))
		{
			InstanceData.EnemyDefinition = EnemyNormal->GetEnemyDefinition();
		}
	}
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FStateTreeGetPlayerInfoTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// 플레이어 참조
	InstanceData.TargetPlayerCharacter =
		Cast<ABaseCharacter>(UGameplayStatics::GetPlayerPawn(InstanceData.Character, 0));

	// 타겟 플레이어가 존재한다면
	if (InstanceData.TargetPlayerCharacter)
	{
		// 플레이어의 위치를 타겟팅할 마지막 위치로 설정
		InstanceData.TargetPlayerLocation = InstanceData.TargetPlayerCharacter->GetActorLocation();
	}
	// homelocation과의 거리 측정
	if (AEnemyBase* EnemyBase = Cast<AEnemyBase>(InstanceData.Character))
	{
		InstanceData.DistanceToHome = FVector::Distance(EnemyBase->GetHomeLocation(), InstanceData.Character->GetActorLocation());
		InstanceData.HomeLocation = EnemyBase->GetHomeLocation();
		// AI 캐릭터 죽었는지 확인
		InstanceData.IsDead = EnemyBase->GetStatComponent()->IsDead();
		InstanceData.IsHit = EnemyBase->GetIsHit();
		// AI 캐릭터가 스턴에 걸렸는지 확인
		InstanceData.IsStunned = EnemyBase->GetStatComponent()->IsStunned();
	}

	// AI 캐릭터와 플레이어 캐릭터의 거리를 구함
	InstanceData.DistanceToTarget = FVector::Distance(
		InstanceData.TargetPlayerLocation,
		InstanceData.Character->GetActorLocation()
	);

	
	if (AEnemyNomal* EnemyNomal = Cast<AEnemyNomal>(InstanceData.Character))
	{
		if (UEnemyDefinition* Definition = EnemyNomal->GetEnemyDefinition())
		{
			InstanceData.DetectRange = Definition->DetectRange;
			InstanceData.AttackRange = Definition->AttackRange;
			InstanceData.ReturnRange = Definition->ReturnRadius;
		}
	}

	return EStateTreeRunStatus::Running;
}

FText FStateTreeGetPlayerInfoTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Get Player Info</b>");
}

EStateTreeRunStatus FStateTreeWidgetTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Enemy)
	{
		return EStateTreeRunStatus::Failed;
	}
	InstanceData.Enemy->SetEnemyHPWidgetVisible(InstanceData.bVisible);
	return EStateTreeRunStatus::Succeeded;
}


FText FStateTreeWidgetTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString("<b>Enemy HP Widget Visible</b>");
}
