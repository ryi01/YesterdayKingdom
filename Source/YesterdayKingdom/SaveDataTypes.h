// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "ItemData.h"
#include "SaveDataTypes.generated.h"
//===============================================================================================
// 플레이어 저장 데이터
// 기본 MaxHP, Attack 같은 값은 DataTable과 스킬 효과로 다시 계산
// DB에는 실제 플레이 중 변하는 현재값만 저장
//===============================================================================================
USTRUCT(BlueprintType)
struct FPlayerSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerId = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Nickname;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHP = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentST = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentMP = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Gold = 0;
	
	bool IsValid() const
	{
		return PlayerId > 0;
	}

	bool IsNewPlayer() const
	{
		return PlayerId <= 0;
	}
	
};
//===============================================================================================
// 스킬 저장 데이터
// 해금된 스킬의 DataTable RowName만 저장
// 로드 시 RowName으로 스킬 데이터를 찾아 효과를 다시 적용
//===============================================================================================
USTRUCT(BlueprintType)
struct FSkillSaveData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SkillRowName = NAME_None;

	bool IsValid() const
	{
		return !SkillRowName.IsNone();
	}
};

//===============================================================================================
// 인벤토리 저장 데이터
// 아이템 전체 데이터를 저장하지 않고 슬롯 번호, RowName, 개수만 저장
//===============================================================================================
USTRUCT(BlueprintType)
struct FInventorySaveData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SlotIndex = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemRowName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;

	bool IsEmpty() const
	{
		return SlotIndex < 0 || ItemRowName.IsNone() || Count <= 0;
	}

	void Clear()
	{
		SlotIndex = -1;
		ItemRowName = NAME_None;
		Count = 0;
	}
};
//===============================================================================================
// 장비창 저장 데이터 
//===============================================================================================
USTRUCT(BlueprintType)
struct FEquipmentSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipmentSlotType EquipmentSlot = EEquipmentSlotType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemRowName = NAME_None;
	
	bool IsValid() const
	{
		return EquipmentSlot != EEquipmentSlotType::None && !ItemRowName.IsNone();
	}
};