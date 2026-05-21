// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonEnumTypes.generated.h"

// Hit 판정 : 피격 반응 타입 
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
// 전반적인 게임 state
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