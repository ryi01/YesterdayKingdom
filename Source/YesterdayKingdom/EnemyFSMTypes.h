// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyFSMTypes.generated.h"
// ========================================================
// 필요한 state만 골라 사용 
// ========================================================
UENUM(BlueprintType)
enum class EEnemyFSMStateType : uint8
{
	None            UMETA(DisplayName = "None"),

	Idle            UMETA(DisplayName = "Idle"),
	Patrol          UMETA(DisplayName = "Patrol"),
	Chase           UMETA(DisplayName = "Chase"),
	Return			UMETA(DisplayName = "Return"),
	
	Attack          UMETA(DisplayName = "Attack"),
	JumpAttack      UMETA(DisplayName = "JumpAttack"),
	RotationAttack  UMETA(DisplayName = "Rotation Attack"),
	
	Cooldown        UMETA(DisplayName = "Cooldown"),
	BackStep        UMETA(DisplayName = "BackStep"),

	Flanking        UMETA(DisplayName = "Flanking"),

	
	// 인형 State -----------------------------------
	Down			UMETA(DisplayName = "Down"),
	Revive			UMETA(DisplayName = "Revive"),
	// ---------------------------------------------

	PatternSelect   UMETA(DisplayName = "Pattern Select"),
	PhaseChange     UMETA(DisplayName = "Phase Change"),
	Groggy          UMETA(DisplayName = "Groggy"),

	Hit             UMETA(DisplayName = "Hit"),
	Dead            UMETA(DisplayName = "Dead")
};