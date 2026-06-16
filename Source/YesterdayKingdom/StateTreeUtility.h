// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "StateTreeTaskBase.h" // FStateTreeTaskCommonBase 구조체 모듈 포함 (커스텀 테스크 구현)
#include "StateTreeConditionBase.h" // FStateTreeConditionCommonBase 구조체 모듈 포함 (커스텀 조건 구현)

#include "StateTreeUtility.generated.h"

class ABaseCharacter;
class ACharacter;
class AAIController;
class AEnemyNomal;

// Condition 관련 Condition [데이터] 구조체 선언
USTRUCT()
struct FStateTreeCharacterGroundedConditionInstanceData
{
	GENERATED_BODY()

	// 관련 캐릭터 참조
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<ABaseCharacter> Character;

	// 공중에 떠있는지를 체크하는 상태 옵션
	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bMustBeAir = false;
};

// FStateTreeCharacterGroundedConditionInstanceData를 (POD로 등록)
// POD : 기능없이 데이터만 가진 순수 메모리복사식 최적화 데이터 형식
// - 원시 포인트 사용시 POD 적용 필요 -> 예) ACharacter* Character;
// - 언리얼5 부터 적용된 스마트포인터 타입의 경우 POD 매크로를 사용하면 안됨!!
// - 포인터타입이 없는 순수 데이터기반 구조체의 경우에는 적용해 줄것
// 
//STATETREE_POD_INSTANCEDATA(FStateTreeCharacterGroundedConditionInstanceData);

// Condition 관련 Condition [구현] 구조체 선언
USTRUCT(DisplayName = "Character is Grounded")
struct FStateTreeCharacterGroundedCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	// 인스턴스 데이터 타입 using
	// * 이 패턴은 모든 Task 및 Condition에서 공통을 작성됨
	using FInstanceDataType = FStateTreeCharacterGroundedConditionInstanceData;
	virtual const UStruct* GetInstanceDataType() const override {
		return FInstanceDataType::StaticStruct();
	}

	// Condition(조건)의 실제 판정 처리 로직 해당 메소드의 결과값을 통해
	// 상태 전환/진입의 허용 여부를 결정함
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR // 현재 코드는 전처리 코드로 에디터에서만 동작함
	// StateTree 에디터에 표시될 설명 문자열 반환
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

//----------------------------------------------------------
// 최근에 플레이어가 공격을 했고 지정된 시야 범위 안쪽에 있는지를 판단
//----------------------------------------------------------
USTRUCT()
struct FStateTreeIsInDangerConditionInstanceData
{
	GENERATED_BODY()

	// 관련 캐릭터 참조
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AEnemyNomal> Character;

	// 위험 감지 후 회피 반응 시간 (최소, 최대)

	UPROPERTY(EditAnywhere, Category = "Parameters", meta = (Units = "s"))
	float MinReactionTime = 0.35f;

	UPROPERTY(EditAnywhere, Category = "Parameters", meta = (Units = "s"))
	float MaxReactionTime = 0.75f;

	UPROPERTY(EditAnywhere, Category = "Parameters", meta = (Units = "degrees"))
	float DangerSightConAngle = 120.0f; // 180기준 120도 (시선벡터 기준 좌우합 240도)
};

//STATETREE_POD_INSTANCEDATA(FStateTreeIsInDangerConditionInstanceData);

// 캐릭터가 회피 모션을 수행할지를 판단하는 조건(Conditions) 구조체
USTRUCT(DisplayName = "Character is in Danger")
struct FStateTreeIsInDangerCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeIsInDangerConditionInstanceData;
	virtual const UStruct* GetInstanceDataType() const override {
		return FInstanceDataType::StaticStruct();
	}

	// 회피 모션 재생 여부를 판단하고 반환활 TestCondition 메소드
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

// 공격 관련 인스턴스 데이터 구조체 선언
USTRUCT()
struct FStateTreeAttackInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AEnemyNomal> Character; // ACombatEnemy* Character;
};

// 콤보 공격 처리 테스크 구조체 선언
USTRUCT(meta = (DisplayName = "Combo Attack", Category = "Combat"))
struct FStateTreeComboAttackTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeAttackInstanceData;
	virtual const UStruct* GetInstanceDataType() const override {
		return FInstanceDataType::StaticStruct();
	}

	// 현재 테스크 시작 이벤트 (다른 상태에서 전환되어 시작됨)
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& context, // 현재 테스크 매칭된 컨텍스트 접근 참조
		const FStateTreeTransitionResult& Transition // 이전 상태(스테이트) 및 전환 정보
	) const override;

	// 현재 테스크 종료 이벤트 (다른 상태로 전환되어 종료됨)
	virtual void ExitState(
		FStateTreeExecutionContext& context, // 현재 테스크 매칭된 컨텍스트 접근 참조
		const FStateTreeTransitionResult& Transition // 이전 상태(스테이트) 및 전환 정보
	) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

// 차지 공격 처리 테스크 구조체 선언
USTRUCT(meta = (DisplayName = "Charged Attack", Category = "Combat"))
struct FStateTreeChargedAttackTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeAttackInstanceData;
	virtual const UStruct* GetInstanceDataType() const override {
		return FInstanceDataType::StaticStruct();
	}

	// 현재 테스크 시작 이벤트 (다른 상태에서 전환되어 시작됨)
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& context, // 현재 테스크 매칭된 컨텍스트 접근 참조
		const FStateTreeTransitionResult& Transition // 이전 상태(스테이트) 및 전환 정보
	) const override;

	// 현재 테스크 종료 이벤트 (다른 상태로 전환되어 종료됨)
	virtual void ExitState(
		FStateTreeExecutionContext& context, // 현재 테스크 매칭된 컨텍스트 접근 참조
		const FStateTreeTransitionResult& Transition // 이전 상태(스테이트) 및 전환 정보
	) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

// 데미지를 입고 공중에 떠올랐다가 착지 할 때까지의 상태 처리(테스크)
USTRUCT(meta = (DisplayName = "Wait for Landing", Category = "Combat"))
struct FStateTreeWaitForLandingTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeAttackInstanceData;
	virtual const UStruct* GetInstanceDataType() const override {
		return FInstanceDataType::StaticStruct();
	}

	// 현재 테스크 시작 이벤트 (다른 상태에서 전환되어 시작됨)
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& context, // 현재 테스크 매칭된 컨텍스트 접근 참조
		const FStateTreeTransitionResult& Transition // 이전 상태(스테이트) 및 전환 정보
	) const override;

	// 현재 테스크 종료 이벤트 (다른 상태로 전환되어 종료됨)
	virtual void ExitState(
		FStateTreeExecutionContext& context, // 현재 테스크 매칭된 컨텍스트 접근 참조
		const FStateTreeTransitionResult& Transition // 이전 상태(스테이트) 및 전환 정보
	) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

// 캐릭터 시선 처리 관련 컨텍스트 데이터 (액터 타겟)
USTRUCT()
struct FStateTreeFaceActorInstanceData
{
	GENERATED_BODY()

	// AI 컨트롤러 참조
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> Controller;

	// 시선을 맞출 대상(플레이어) 엑터
	UPROPERTY(EditAnywhere, Category = "Actor")
	TObjectPtr<AActor> ActorToFaceTowards;
};

// 특정 액터를 바라보는 시선처리 테스크 구조체
USTRUCT(meta = (DisplayName = "Face Towards Actor", Category = "Combat"))
struct FStateTreeFaceActorTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeFaceActorInstanceData;
	virtual const UStruct* GetInstanceDataType() const override {
		return FInstanceDataType::StaticStruct();
	}

	// 현재 테스크 시작 이벤트 (다른 상태에서 전환되어 시작됨)
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& context, // 현재 테스크 매칭된 컨텍스트 접근 참조
		const FStateTreeTransitionResult& Transition // 이전 상태(스테이트) 및 전환 정보
	) const override;

	// 현재 테스크 종료 이벤트 (다른 상태로 전환되어 종료됨)
	virtual void ExitState(
		FStateTreeExecutionContext& context, // 현재 테스크 매칭된 컨텍스트 접근 참조
		const FStateTreeTransitionResult& Transition // 이전 상태(스테이트) 및 전환 정보
	) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

// 캐릭터 시선 처리 관련 컨텍스트 데이터 (위치 타겟)
USTRUCT()
struct FStateTreeFaceLocationInstanceData
{
	GENERATED_BODY()

	// AI 컨트롤러 참조
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> Controller;

	// 시선을 맞출 대상(플레이어) 엑터
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FVector FaceLocation = FVector::ZeroVector;
};

// 특정 위치를 바라보는 시선처리 테스크 구조체
USTRUCT(meta = (DisplayName = "Face Towards Location", Category = "Combat"))
struct FStateTreeFaceLocationTask: public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeFaceLocationInstanceData;
	virtual const UStruct* GetInstanceDataType() const override {
		return FInstanceDataType::StaticStruct();
	}

	// 현재 테스크 시작 이벤트 (다른 상태에서 전환되어 시작됨)
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& context, // 현재 테스크 매칭된 컨텍스트 접근 참조
		const FStateTreeTransitionResult& Transition // 이전 상태(스테이트) 및 전환 정보
	) const override;

	// 현재 테스크 종료 이벤트 (다른 상태로 전환되어 종료됨)
	virtual void ExitState(
		FStateTreeExecutionContext& context, // 현재 테스크 매칭된 컨텍스트 접근 참조
		const FStateTreeTransitionResult& Transition // 이전 상태(스테이트) 및 전환 정보
	) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

// AI 캐릭터 이동 속도 설정 데이터 구조체
USTRUCT()
struct FStateTreeSetCharacterSpeedInstanceData
{
	GENERATED_BODY()

	// AI 캐릭터 참조
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<ACharacter> Character;

	// 캐릭터 이동 속도
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float Speed = 600.0f;
};

// AI 캐릭터 속도 설정 테스크 구조체
USTRUCT(meta = (DisplayName = "Set Character Speed", Category = "Combat"))
struct FStateTreeSetCharacterSpeedTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeSetCharacterSpeedInstanceData;
	virtual const UStruct* GetInstanceDataType() const override {
		return FInstanceDataType::StaticStruct();
	}

	// 현재 테스크 시작 이벤트 (다른 상태에서 전환되어 시작됨)
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& context, // 현재 테스크 매칭된 컨텍스트 접근 참조
		const FStateTreeTransitionResult& Transition // 이전 상태(스테이트) 및 전환 정보
	) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

USTRUCT()
struct FStateTreeGetPlayerInfoInstanceData
{
	GENERATED_BODY()

	// AI 캐릭터 참조
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AEnemyNomal> Character;

	// 플레이어 캐릭터 참조
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ABaseCharacter> TargetPlayerCharacter;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UEnemyDefinition> EnemyDefinition;
	
	// 플레이어 위치
	UPROPERTY(VisibleAnywhere)
	FVector TargetPlayerLocation = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere)
	FVector HomeLocation = FVector::ZeroVector;
	
	// 플레이어와의 거리
	UPROPERTY(VisibleAnywhere)
	float DistanceToTarget = 0.0f;
	
	UPROPERTY(VisibleAnywhere)
	float DistanceToHome = 0.f;

	UPROPERTY(VisibleAnywhere)
	bool IsHit = false;
	
	UPROPERTY(VisibleAnywhere)
	bool IsDead = false;
	
	UPROPERTY(VisibleAnywhere)
	bool IsStunned = false;
	
	UPROPERTY(VisibleAnywhere)
	float DetectRange = 0.f;

	UPROPERTY(VisibleAnywhere)
	float AttackRange = 0.f;
	
	UPROPERTY(VisibleAnywhere)
	float ReturnRange = 0.f;
	
};

// 플레이어 정보 구하기 (글로벌 테스크)
USTRUCT(meta = (DisplayName = "Get Player Info", Category = "Combat"))
struct FStateTreeGetPlayerInfoTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeGetPlayerInfoInstanceData;
	virtual const UStruct* GetInstanceDataType() const override {
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	// 현재 테스크 진행 중 이벤트
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

USTRUCT()
struct FSetEnemyHPWidgetVisibleTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AEnemyBase> Enemy;

	UPROPERTY(EditAnywhere, Category = "UI")
	bool bVisible = true;
};
USTRUCT(meta = (DisplayName = "Set Enemy HP Widget Visible", Category = "Combat"))
struct FStateTreeWidgetTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSetEnemyHPWidgetVisibleTaskInstanceData;
	virtual const UStruct* GetInstanceDataType() const override {
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

