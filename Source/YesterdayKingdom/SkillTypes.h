// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SkillTypes.generated.h"

UENUM(BlueprintType)
enum class ESkillCategory : uint8
{
	Attack      UMETA(DisplayName = "Attack"),
	Defense     UMETA(DisplayName = "Defense"),
	Utility     UMETA(DisplayName = "Utility"),
	Buff        UMETA(DisplayName = "Buff")
};

UENUM(BlueprintType)
enum class ESkillEffectType : uint8
{
	None                UMETA(DisplayName = "None"),

	AddAttack           UMETA(DisplayName = "Add Attack"),
	AddDefense          UMETA(DisplayName = "Add Defense"),
	AddMaxHP            UMETA(DisplayName = "Add Max HP"),
	AddMaxMP            UMETA(DisplayName = "Add Max MP"),
	AddMaxST            UMETA(DisplayName = "Add Max ST"),

	AddMoveSpeed        UMETA(DisplayName = "Add Move Speed"),
	
	UnlockChargeAttack  UMETA(DisplayName = "Unlock Charge Attack"),
	UnlockParry         UMETA(DisplayName = "Unlock Parry"),
	UnlockBattleBuff	UMETA(DisplayName = "Unlock Battle Buff")
};
USTRUCT(BlueprintType)
struct FSkillDataRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FText SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	ESkillCategory Category = ESkillCategory::Attack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	ESkillEffectType EffectType = ESkillEffectType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	float EffectValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	int32 GoldCost = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FName RequiredSkillRowName = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<class UTexture2D> InactiveSkillIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<class UTexture2D> ActiveSkillIcon;
	
};