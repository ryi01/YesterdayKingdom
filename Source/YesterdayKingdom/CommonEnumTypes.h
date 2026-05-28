// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CommonEnumTypes.generated.h"

class UCameraShakeBase;
class UParticleSystem;
class USoundBase;
class UAnimMontage;

//========================================================================================================
// Hit 판정
//========================================================================================================
UENUM(BlueprintType)
enum class EHitReactionType : uint8
{
	None        UMETA(DisplayName = "None"),
	// 약경직
	Stagger		UMETA(DisplayName = "Stagger"),
	// 넉백
	Knockback		UMETA(DisplayName = "Knockback"),
	// 공중 띄기
	Launch		UMETA(DisplayName = "Launch"),
	// 바닥 강타
	Slam		UMETA(DisplayName = "Slam"),
	// 강경직
	Stun		UMETA(DisplayName = "Stun")
};
//========================================================================================================
// 게임 흐름
//========================================================================================================
UENUM(BlueprintType)
enum class EGameFlowState : uint8
{
	None        UMETA(DisplayName = "None"),

	Title       UMETA(DisplayName = "Title"),
	Town        UMETA(DisplayName = "Town"),
	Dungeon     UMETA(DisplayName = "Dungeon"),
	BossBattle  UMETA(DisplayName = "Boss Battle"),

	Loading     UMETA(DisplayName = "Loading"),
	Pause       UMETA(DisplayName = "Pause"),
	UI          UMETA(DisplayName = "UI"),
	GameOver    UMETA(DisplayName = "Game Over")
};

//========================================================================================================
// 카메라 효과 관련
//========================================================================================================
USTRUCT(BlueprintType)
struct FHitFeedbackData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hit Feedback|Camera")
	TSubclassOf<UCameraShakeBase> CameraShake;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hit Feedback|Camera")
	float ShakeScale = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hit Feedback|Hit Stop")
	bool bUseHitStop = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hit Feedback|Hit Stop", meta=(EditCondition="bUseHitStop"))
	float HitStopDuration = 0.04f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hit Feedback|Hit Stop", meta=(EditCondition="bUseHitStop", ClampMin="0.01", ClampMax="1.0"))
	float HitStopTimeScale = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hit Feedback|Effect")
	TObjectPtr<UParticleSystem> HitEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hit Feedback|Sound")
	TObjectPtr<USoundBase> HitSound;
};
//========================================================================================================
// 공격 관련
//========================================================================================================
UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None,
	Light,
	Heavy,
	Charge,
	Special,
	Dash,
	AOE
};

USTRUCT(BlueprintType)
struct FAttackNodeData
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack")
	int32 NextIndex= INDEX_NONE;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack")
	FName SectionName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack")
	float Damage = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack")
	FHitFeedbackData HitFeedback;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack")
	EHitReactionType HitReactionType = EHitReactionType::Stagger;
};
USTRUCT(BlueprintType)
struct FAttackDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack")
	EAttackType AttackType = EAttackType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack")
	TObjectPtr<UAnimMontage> Montage;
	
	UPROPERTY(EditAnywhere)
	TArray<FAttackNodeData> Nodes;
};