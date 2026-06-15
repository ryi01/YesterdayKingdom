// Fill out your copyright notice in the Description page of Project Settings.

#include "YesterdayKingdomGameInstance.h"
#include "Misc/Paths.h"

sqlite3* UYesterdayKingdomGameInstance::Database = nullptr;

void UYesterdayKingdomGameInstance::Init()
{
	Super::Init();
	if (OpenDatabase())
	{
		CreateTablesIfNeeded();
	}
}

void UYesterdayKingdomGameInstance::Shutdown()
{
	if (Database)
	{
		sqlite3_close(Database);
		Database = nullptr;
	}
	Super::Shutdown();
}

bool  UYesterdayKingdomGameInstance::OpenDatabase()
{
	FString DBPath = GetDatabasePath();
	// 없다면 생성 
	int Result = sqlite3_open(TCHAR_TO_UTF8(*DBPath), &Database);
	if (Result != SQLITE_OK)
	{
		const FString ErrorMessage = Database ?  UTF8_TO_TCHAR(sqlite3_errmsg(Database)) : TEXT("Database pointer is null");
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] DB 열기 실패: %s / Error: %s"),
			*DBPath,
			*ErrorMessage
		);
		if (Database)
		{
			sqlite3_close(Database);
			Database = nullptr;
		}
		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("[SQLite] DB 열기 성공: %s"), *DBPath);
	const int32 ForeignKeyResult = sqlite3_exec(Database, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
	if (ForeignKeyResult != SQLITE_OK)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[SQLite] 외래키 활성화 실패: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);
	}
	return true;
}

void UYesterdayKingdomGameInstance::CreateTablesIfNeeded()
{
	if (!Database) return;
	
	const char* CreatePlayerTableSQL  = 
		"CREATE TABLE IF NOT EXISTS PlayerData ("
		"PlayerId INTEGER PRIMARY KEY AUTOINCREMENT,"
		"Nickname TEXT NOT NULL UNIQUE,"
		"CurrentHP REAL NOT NULL,"
		"CurrentST REAL NOT NULL,"
		"CurrentMP REAL NOT NULL,"
		"Gold INTEGER NOT NULL DEFAULT 0"
		");";
	
	const char* CreateInventoryTableSQL =
		"CREATE TABLE IF NOT EXISTS Inventory ("
		"PlayerId INTEGER NOT NULL,"
		"SlotIndex INTEGER NOT NULL,"
		"ItemRowName TEXT NOT NULL,"
		"Count INTEGER NOT NULL DEFAULT 0,"
		"PRIMARY KEY (PlayerId, SlotIndex),"
		"FOREIGN KEY (PlayerId) "
		"REFERENCES PlayerData(PlayerId) "
		"ON DELETE CASCADE"
		");";
	char* ErrorMessage = nullptr;
	
	int32 Result = sqlite3_exec(Database, CreatePlayerTableSQL, nullptr, nullptr, &ErrorMessage);
	if (Result != SQLITE_OK)
	{
		UE_LOG(LogTemp, Error, TEXT("[SQLite] PlayerData 생성 실패: %s"),
			ErrorMessage ? UTF8_TO_TCHAR(ErrorMessage) : TEXT("Unknown"));
		if (ErrorMessage)
		{
			sqlite3_free(ErrorMessage);
			ErrorMessage = nullptr;
		}
	}
	Result = sqlite3_exec(Database, CreateInventoryTableSQL, nullptr, nullptr, &ErrorMessage);
	if (Result != SQLITE_OK)
	{
		UE_LOG(LogTemp, Error, TEXT("[SQLite] Inventory 생성 실패: %s"),
			ErrorMessage ? UTF8_TO_TCHAR(ErrorMessage) : TEXT("Unknown"));
		if (ErrorMessage)
		{
			sqlite3_free(ErrorMessage);
			ErrorMessage = nullptr;
		}
	}
}

FString UYesterdayKingdomGameInstance::GetDatabasePath() const
{
	return  FPaths::Combine(FPaths::ProjectSavedDir(), 	TEXT("YesterdayKingdomSave.db"));
}
//===============================================================================================
// 플레이어 데이터 저장 로드
//===============================================================================================
int32 UYesterdayKingdomGameInstance::CreatePlayerData(const FPlayerSaveData& SaveData)
{
	if (!Database) return 0;
	
	const char* InsertSQL = 
		"INSERT INTO PlayerData "
		"(Nickname, CurrentHP, CurrentST, CurrentMP, Gold) "
		"VALUES (?, ?, ?, ?, ?)";
	sqlite3_stmt* Statement = nullptr;
	const int32 PrepareResult = sqlite3_prepare_v2(Database, InsertSQL, -1, &Statement, nullptr);
	if (PrepareResult != SQLITE_OK)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 플레이어 생성 쿼리 준비 실패: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);

		return 0;
	}
	const FString TrimmedNickname = SaveData.Nickname.TrimStartAndEnd();
	if (TrimmedNickname.IsEmpty())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[SQLite] 플레이어 생성 실패: 닉네임이 비어 있음")
		);

		return 0;
	}
	const FTCHARToUTF8 NicknameUTF8(*TrimmedNickname);
	sqlite3_bind_text(Statement, 1, NicknameUTF8.Get(), NicknameUTF8.Length(), SQLITE_TRANSIENT);
	sqlite3_bind_double(Statement, 2, SaveData.CurrentHP);
	sqlite3_bind_double(Statement, 3, SaveData.CurrentST);
	sqlite3_bind_double(Statement, 4, SaveData.CurrentMP);
	sqlite3_bind_int(Statement, 5, SaveData.Gold);

	const int32 StepResult = sqlite3_step(Statement);
	if (StepResult != SQLITE_DONE)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 플레이어 생성 실패: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);

		sqlite3_finalize(Statement);
		return 0;
	}
	
	int32 NewPlayerId = static_cast<int32>(sqlite3_last_insert_rowid(Database));
	sqlite3_finalize(Statement);
	
	UE_LOG(LogTemp,Log,TEXT("[SQLite] 플레이어 생성 완료 / PlayerId=%d"),NewPlayerId);

	return NewPlayerId;
}
bool UYesterdayKingdomGameInstance::SavePlayerData(const FPlayerSaveData& SaveData)
{
	if (!Database || SaveData.PlayerId <= 0) return false;
	const char* UpdateSQL =	
		"UPDATE PlayerData SET "
		"CurrentHP = ?, "
		"CurrentST = ?, "
		"CurrentMP = ?, "
		"Gold = ? "
		"WHERE PlayerId = ?;";
	
	sqlite3_stmt* Statement = nullptr; 
												
	const int32 PrepareResult = 
		sqlite3_prepare_v2
		(
			Database,   // 열린 DB 연결
			UpdateSQL,    // 실행할 SQL 문자열
			-1,         // 문자열 끝까지 읽기
			&Statement, // 만들어진 준비 문장을 받을 변수
			nullptr     // SQL 뒤에 남는 문자열, 보통 사용 안 함
		);
	if (PrepareResult != SQLITE_OK)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 플레이어 저장 쿼리 준비 실패: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);
		return false;
	}
	sqlite3_bind_double(Statement, 1, SaveData.CurrentHP);
	sqlite3_bind_double(Statement, 2, SaveData.CurrentST);
	sqlite3_bind_double(Statement, 3, SaveData.CurrentMP);
	sqlite3_bind_int(Statement, 4, SaveData.Gold);
	sqlite3_bind_int(Statement, 5, SaveData.PlayerId);
	
	const int32 StepResult = sqlite3_step(Statement);
	if (StepResult != SQLITE_DONE)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 플레이어 저장 실패: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);

		sqlite3_finalize(Statement);
		return false;
	}
	const int32 ChangedRows = sqlite3_changes(Database);

	sqlite3_finalize(Statement);
	if (ChangedRows <= 0)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[SQLite] 저장할 플레이어를 찾지 못함 / PlayerId=%d"),
			SaveData.PlayerId
		);

		return false;
	}

	UE_LOG(
	LogTemp,
	Log,
	TEXT("[SQLite] 플레이어 저장 완료 / PlayerId=%d HP=%.1f ST=%.1f MP=%.1f Gold=%d"),
	SaveData.PlayerId,
	SaveData.CurrentHP,
	SaveData.CurrentST,
	SaveData.CurrentMP,
	SaveData.Gold
);

	return true;
}

bool UYesterdayKingdomGameInstance::LoadPlayerData(int32 PlayerId,FPlayerSaveData& OutSaveData)
{
	if (!Database || PlayerId <= 0) return false;
	const char* LoadSQL = 
		"SELECT PlayerId, Nickname, CurrentHP, CurrentST, CurrentMP, Gold "
		"FROM PlayerData "
		"WHERE PlayerId = ?;";
	sqlite3_stmt* Statement = nullptr;
	const int32 PrepareResult = sqlite3_prepare_v2(Database, LoadSQL, -1, &Statement, nullptr);
	if (PrepareResult != SQLITE_OK)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 플레이어 로드 쿼리 준비 실패: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);

		return false;
	}
	
	sqlite3_bind_int(Statement, 1, PlayerId);
	
	const int32 StepResult = sqlite3_step(Statement);
	if (StepResult == SQLITE_ROW)
	{
		OutSaveData.PlayerId = sqlite3_column_int(Statement, 0);
		const unsigned char* NicknameText = sqlite3_column_text(Statement, 1);
		OutSaveData.Nickname = NicknameText ? UTF8_TO_TCHAR(reinterpret_cast<const char*>(NicknameText)) : TEXT("");
		OutSaveData.CurrentHP = static_cast<float>(sqlite3_column_double(Statement, 2));
		OutSaveData.CurrentST = static_cast<float>(sqlite3_column_double(Statement, 3));
		OutSaveData.CurrentMP = static_cast<float>(sqlite3_column_double(Statement, 4));
		OutSaveData.Gold = sqlite3_column_int(Statement, 5);
		sqlite3_finalize(Statement);

		UE_LOG(
			LogTemp,
			Log,
			TEXT("[SQLite] 플레이어 로드 완료 / HP=%.1f ST=%.1f MP=%.1f Gold=%d"),
			OutSaveData.CurrentHP,
			OutSaveData.CurrentST,
			OutSaveData.CurrentMP,
			OutSaveData.Gold
		);

		return true;
	}
	sqlite3_finalize(Statement);
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[SQLite] 저장된 플레이어 데이터 없음")
	);

	return false;
}

bool UYesterdayKingdomGameInstance::GetLatestPlayerId(int32& OutPlayerId)
{
	OutPlayerId = 0;
	if (!Database) return false;
	const char* SelectSQL = 
		"SELECT PlayerId "
		"FROM PlayerData " 
		"ORDER BY PlayerId DESC "
		"LIMIT 1;";
	sqlite3_stmt* Statement = nullptr;
	const int32 PrepareResult = sqlite3_prepare_v2(Database, SelectSQL, -1, &Statement, nullptr);
	if (PrepareResult != SQLITE_OK)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 최근 플레이어 조회 쿼리 준비 실패: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);

		return false;
	}
	const int32 StepResult = sqlite3_step(Statement);
	if (StepResult == SQLITE_ROW)
	{
		OutPlayerId = sqlite3_column_int(Statement, 0);

		sqlite3_finalize(Statement);

		UE_LOG(
			LogTemp,
			Log,
			TEXT("[SQLite] 최근 플레이어 조회 완료 / PlayerId=%d"),
			OutPlayerId
		);

		return OutPlayerId > 0;
	}
	if (StepResult != SQLITE_DONE)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 최근 플레이어 조회 실패: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);
	}

	sqlite3_finalize(Statement);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[SQLite] 이어서 할 플레이어 데이터가 없음")
	);

	return false;
}

bool UYesterdayKingdomGameInstance::FindPlayerIdByNickname(const FString& Nickname, int32& OutPlayerId)
{
	OutPlayerId = 0;

	if (!Database) return false;
	
	const FString TrimmedNickname = Nickname.TrimStartAndEnd();
	if (TrimmedNickname.IsEmpty()) return false;
	const char* SelectSQL = 
		"SELECT PlayerId "
		"FROM PlayerData "
		"WHERE Nickname = ? "
		"LIMIT 1;";
	sqlite3_stmt* Statement = nullptr;
	const int32 PrepareResult = sqlite3_prepare_v2(Database, SelectSQL, -1, &Statement, nullptr);
	if (PrepareResult != SQLITE_OK)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 닉네임 조회 쿼리 준비 실패: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);

		return false;
	}
	const FTCHARToUTF8 NicknameUTF8(*TrimmedNickname);
	sqlite3_bind_text(Statement, 1, NicknameUTF8.Get(), NicknameUTF8.Length(), SQLITE_TRANSIENT);
	const int32 StepResult = sqlite3_step(Statement);
	if (StepResult == SQLITE_ROW)
	{
		OutPlayerId = sqlite3_column_int(Statement, 0);
		
		sqlite3_finalize(Statement);

		UE_LOG(
			LogTemp,
			Log,
			TEXT("[SQLite] 닉네임 조회 성공 / Nickname=%s PlayerId=%d"),
			*TrimmedNickname,
			OutPlayerId
		);

		return OutPlayerId > 0;
	}
	if (StepResult != SQLITE_DONE)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 닉네임 조회 실패: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);
	}

	sqlite3_finalize(Statement);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[SQLite] 해당 닉네임의 플레이어 없음 / Nickname=%s"),
		*TrimmedNickname
	);

	return false;
}

void UYesterdayKingdomGameInstance::SetCurrentPlayerId(int32 InPlayerId)
{
	CurrentPlayerId = InPlayerId;
}

int32 UYesterdayKingdomGameInstance::GetCurrentPlayerId() const
{
	return CurrentPlayerId;
}
//===============================================================================================
// 인벤토리 데이터 저장 로드
//===============================================================================================
bool UYesterdayKingdomGameInstance::SaveInventoryData(int32 PlayerId,const TArray<FInventorySaveData>& InventoryDataList)
{
	if (!Database || PlayerId <= 0) return false;
	
	char* ErrorMessage = nullptr;
	int32 Result  = sqlite3_exec(Database, "BEGIN TRANSACTION;", nullptr, nullptr, &ErrorMessage);
	if (Result != SQLITE_OK)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 인벤토리 트랜잭션 시작 실패: %s"),
			ErrorMessage
				? UTF8_TO_TCHAR(ErrorMessage)
				: TEXT("Unknown")
		);

		if (ErrorMessage)
		{
			sqlite3_free(ErrorMessage);
		}

		return false;
	}
	const char* DeleteSQL =
		"DELETE FROM Inventory "
		"WHERE PlayerId = ?;";
	sqlite3_stmt* DeleteStatement = nullptr;
	Result = sqlite3_prepare_v2(Database, DeleteSQL, -1, &DeleteStatement, nullptr);
	if (Result != SQLITE_OK)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 기존 인벤토리 삭제 쿼리 준비 실패: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);

		sqlite3_exec(Database, "ROLLBACK;", nullptr, nullptr, nullptr);
		return false;
	}
	
	sqlite3_bind_int(DeleteStatement, 1, PlayerId);
	Result = sqlite3_step(DeleteStatement);
	sqlite3_finalize(DeleteStatement);
	if (Result != SQLITE_DONE)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 기존 인벤토리 삭제 실패: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);

		sqlite3_exec(Database, "ROLLBACK;", nullptr, nullptr, nullptr);
		return false;
	}
	
	const char* InsertSQL = 
		"INSERT INTO Inventory "
		"(PlayerId, SlotIndex, ItemRowName, Count) "
		"VALUES (?, ?, ?, ?);";
	
	sqlite3_stmt* InsertStatement = nullptr;
	Result = sqlite3_prepare_v2(Database, InsertSQL, -1, &InsertStatement, nullptr);
	if (Result != SQLITE_OK)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 인벤토리 저장 쿼리 준비 실패: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);

		sqlite3_exec(Database, "ROLLBACK;", nullptr, nullptr, nullptr);
		return false;
	}

	int32 SavedSlotCount = 0;
	for (const FInventorySaveData& InventorySaveData : InventoryDataList)
	{
		if (InventorySaveData.IsEmpty()) continue;
		const FString ItemRowNameString = InventorySaveData.ItemRowName.ToString();
		const FTCHARToUTF8 ItemRowNameUTF8(*ItemRowNameString);
		sqlite3_bind_int(InsertStatement, 1, PlayerId);
		sqlite3_bind_int(InsertStatement, 2, InventorySaveData.SlotIndex);
		sqlite3_bind_text(InsertStatement, 3, ItemRowNameUTF8.Get(), ItemRowNameUTF8.Length(), SQLITE_TRANSIENT);
		sqlite3_bind_int(InsertStatement, 4, InventorySaveData.Count);
		
		Result = sqlite3_step(InsertStatement);
		
		if (Result != SQLITE_DONE)
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("[SQLite] 인벤토리 슬롯 저장 실패 / Slot=%d / Error=%s"),
				InventorySaveData.SlotIndex,
				UTF8_TO_TCHAR(sqlite3_errmsg(Database))
			);

			sqlite3_finalize(InsertStatement);
			sqlite3_exec(
				Database,
				"ROLLBACK;",
				nullptr,
				nullptr,
				nullptr
			);

			return false;
		}
		++SavedSlotCount;
		sqlite3_reset(InsertStatement);
		sqlite3_clear_bindings(InsertStatement);
	}
	
	sqlite3_finalize(InsertStatement);
	Result = sqlite3_exec(Database, "COMMIT;", nullptr, nullptr, &ErrorMessage);
	if (Result != SQLITE_OK)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 인벤토리 트랜잭션 커밋 실패: %s"),
			ErrorMessage
				? UTF8_TO_TCHAR(ErrorMessage)
				: TEXT("Unknown")
		);

		if (ErrorMessage)
		{
			sqlite3_free(ErrorMessage);
			ErrorMessage = nullptr;
		}

		sqlite3_exec(Database,"ROLLBACK;",nullptr,nullptr,nullptr);

		return false;
	}
	
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[SQLite] 인벤토리 저장 완료 / PlayerId=%d / SlotCount=%d"),
		PlayerId,
		SavedSlotCount
	);

	return true;
}

bool UYesterdayKingdomGameInstance::LoadInventoryData(int32 PlayerId, TArray<FInventorySaveData>& OutInventoryDataList)
{
	if (!Database || PlayerId <= 0) return false;

	OutInventoryDataList.Empty();

	const char* LoadSQL = 
		"SELECT SlotIndex, ItemRowName, Count "
		"FROM Inventory "
		"WHERE PlayerId = ? "
		"ORDER BY SlotIndex ASC;";
	
	sqlite3_stmt* Statement = nullptr;
	const int32 PrepareResult = sqlite3_prepare_v2(Database, LoadSQL, -1, &Statement, nullptr);
	if (PrepareResult != SQLITE_OK)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 인벤토리 로드 쿼리 준비 실패: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);

		return false;
	}

	sqlite3_bind_int(Statement, 1, PlayerId);
	
	int32 StepResult = SQLITE_OK;
	
	while ((StepResult = sqlite3_step(Statement)) == SQLITE_ROW)
	{
		FInventorySaveData InventorySaveData;
		InventorySaveData.SlotIndex = sqlite3_column_int(Statement, 0);
		const unsigned char* ItemRowNameText = sqlite3_column_text(Statement, 1);
		if (ItemRowNameText)
		{
			const FString ItemRowNameString = UTF8_TO_TCHAR(reinterpret_cast<const char*>(ItemRowNameText));
			InventorySaveData.ItemRowName = FName(*ItemRowNameString);
		}
		InventorySaveData.Count = sqlite3_column_int(Statement, 2);
		if (!InventorySaveData.IsEmpty())
		{
			OutInventoryDataList.Add(InventorySaveData);
		}
	}
	if (StepResult != SQLITE_DONE)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[SQLite] 인벤토리 조회 중 오류 발생: %s"),
			UTF8_TO_TCHAR(sqlite3_errmsg(Database))
		);

		sqlite3_finalize(Statement);
		OutInventoryDataList.Empty();
		return false;
	}

	sqlite3_finalize(Statement);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[SQLite] 인벤토리 로드 완료 / PlayerId=%d / SlotCount=%d"),
		PlayerId,
		OutInventoryDataList.Num()
	);

	return true;
}