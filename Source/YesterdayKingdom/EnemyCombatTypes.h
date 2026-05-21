// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCombatTypes.generated.h"

// Enemy의 직업 => 근거리, 원거리, 마법형
UENUM(BlueprintType)
enum class EEnemyAttackType : uint8
{
	Melee		UMETA(DisplayName = "Melee"),
	Ranged		UMETA(DisplayName = "Ranged"),
	Magic		UMETA(DisplayName = "Magic")
};
// Enemy의 종류 => 기본, 엘리트, 보스
UENUM(BlueprintType)
enum class EEnemyRole : uint8
{
	Normal		UMETA(DisplayName = "Normal"),
	Elite		UMETA(DisplayName = "Elite"),
	Boss		UMETA(DisplayName = "Boss")
};
// AI State : 적이 이동하는 상태 
UENUM(BlueprintType)
enum class EEnemyBehaviorState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Patrol		UMETA(DisplayName = "Patrol"),
	Chase		UMETA(DisplayName = "Chase"),
	Attack		UMETA(DisplayName = "Attack"),
	Hit			UMETA(DisplayName = "Hit"),
	Stunned		UMETA(DisplayName = "Stunned"),
	Dead		UMETA(DisplayName = "Dead"),
};