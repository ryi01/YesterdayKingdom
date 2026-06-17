// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SQLiteDatabase.h"
#include "SaveDataTypes.h"
#include <sqlite/sqlite3.h>
#include "YesterdayKingdomGameInstance.generated.h"

UCLASS()
class YESTERDAYKINGDOM_API UYesterdayKingdomGameInstance : public UGameInstance
{
	GENERATED_BODY()
private:
	int32 CurrentPlayerId = 0;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SQLite|Item")
	TObjectPtr<UDataTable> ItemDataTable;
public:
	static sqlite3* Database;
private:
	bool OpenDatabase();
	void CreateTablesIfNeeded();
	UFUNCTION(BlueprintCallable, Category="SQLite|Item")
	bool SeedItemMasterDataFromTable();
	FString GetDatabasePath() const;
public:
	virtual void Init() override;
	virtual void Shutdown() override;
	//===============================================================================================
	// 플레이어 데이터 저장 로드
	//===============================================================================================
	int32 CreatePlayerData(const FPlayerSaveData& SaveData);
	bool SavePlayerData(const FPlayerSaveData& SaveData);
	bool LoadPlayerData(int32 PlayerId, FPlayerSaveData& OutSaveData);
	bool GetLatestPlayerId(int32& OutPlayerId);
	bool FindPlayerIdByNickname(const FString& Nickname, int32& OutPlayerId);
	void SetCurrentPlayerId(int32 InPlayerId);
	int32 GetCurrentPlayerId() const;
	//===============================================================================================
	// 인벤토리 데이터 저장 로드
	//===============================================================================================
	bool SaveInventoryData(int32 PlayerId, const TArray<FInventorySaveData>& InventoryDataList);
	bool LoadInventoryData(int32 PlayerId, TArray<FInventorySaveData>& OutInventoryDataList);
	//===============================================================================================
	// 스킬트리 데이터 저장 로드
	//===============================================================================================
	bool SaveSkillTreeData(int32 PlayerId, const TArray<FSkillSaveData>& SkillSaveDatas);
	bool LoadSkillTreeData(int32 PlayerId, TArray<FSkillSaveData>& SkillSaveDatas);
	//===============================================================================================
	// 장비 데이터 저장 로드
	//===============================================================================================
	bool SaveEquipmentData(int32 PlayerId, const TArray<FEquipmentSaveData>& EquipmentSaveDatas);
	bool LoadEquipmentData(int32 PlayerId, TArray<FEquipmentSaveData>& EquipmentSaveDatas);
	//===============================================================================================
	// 아이템 데이터
	//===============================================================================================
	UFUNCTION(BlueprintCallable, Category="SQLite|Inventory")
	bool LoadInventoryViewData(int32 PlayerId, TArray<FInventoryViewData>& OutInventoryViewDataList);
};
