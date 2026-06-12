// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterStatusDataRow.generated.h"
USTRUCT(BlueprintType)
struct FCharacterStatRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	// 최대 HP 
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHP = 100.f;

	// 최대 스테미너
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxST = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxMP = 100.f;
	
	// 공격력
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Attack = 10.f;

	// 방어력
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Defense = 5.f;

	// 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MoveSpeed = 400.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GuardSpeed = 350.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RunSpeed = 600.f;
	
	// 경직 
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxStun = 50.f;
};