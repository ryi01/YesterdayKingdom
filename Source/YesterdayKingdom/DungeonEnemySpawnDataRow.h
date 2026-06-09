// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DungeonEnemySpawnDataRow.generated.h"


#pragma region Dungeon RoomInfo
// 던전에 생성되는 방 정보
UENUM(BlueprintType)
enum class EDungeonRoomType : uint8
{
	None		UMETA(DisplayName="None"),
	Start		UMETA(DisplayName="Start"),
	Normal		UMETA(DisplayName="Normal"),
	Elite		UMETA(DisplayName="Elite"),
	Store		UMETA(DisplayName="Store"),
	Boss		UMETA(DisplayName="Boss")
};
UENUM(BlueprintType)
enum class EDungeonDecorationTheme : uint8
{
	None,
	Basic,
	Storage,
	Broken,
	WeaponRoom,
	StoreRoom,
	BossEntrance,
	StartRoom
};
USTRUCT(BlueprintType)
struct FDungeonRoomInfo
{
	GENERATED_BODY()
	
	UPROPERTY()
	FVector2D Center = FVector2D::ZeroVector;
	UPROPERTY()
	EDungeonRoomType RoomType = EDungeonRoomType::Normal;
	UPROPERTY()
	EDungeonDecorationTheme DecorationTheme = EDungeonDecorationTheme::Basic;
};

#pragma endregion
#pragma region Dungeon RoomActor
// 던전에 배치되는 벽, 바닥 등
UENUM(BlueprintType)
enum class EDungeonVisualTileType : uint8
{
	Room		UMETA(DisplayName="Room"),
	BossRoom	UMETA(DisplayName="BossRoom"),
	Corridor	UMETA(DisplayName="Corridor"),
	Wall		UMETA(DisplayName="Wall")
};

UENUM(BlueprintType)
enum class EDungeonPieceShape : uint8
{
	Default		UMETA(DisplayName="Default"),
	Straight	UMETA(DisplayName="Straight"),
	Corner		UMETA(DisplayName="Corner"),
	End			UMETA(DisplayName="End"),
	TJunction	UMETA(DisplayName="TJunction"),
	Cross		UMETA(DisplayName="Cross")
};
USTRUCT(BlueprintType)
struct FDungeonTileVisualDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EDungeonVisualTileType TileType = EDungeonVisualTileType::Room;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EDungeonPieceShape Shape = EDungeonPieceShape::Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<AActor>> TileClasses;
};
#pragma endregion
#pragma region Dungeon Enemy
USTRUCT(BlueprintType)
struct FDungeonEnemySpawnEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxCount = 0;
};
// 던전에 스폰되는 적 갯수
USTRUCT(BlueprintType)
struct FDungeonEnemySpawnDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EDungeonRoomType RoomType = EDungeonRoomType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDungeonEnemySpawnEntry> EnemySpawnEntries;
};
#pragma endregion
#pragma region Dungeon Decoration
// 던전에 꾸며지는 오브젝트
USTRUCT(BlueprintType)
struct FDungeonDecorationEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> DecorationClass;
	
	// 일반 방 랜덤 선택용
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnWeight = 1.f;

	// StoreRoom 전용 개수 지정용
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxCount = 1;
};
USTRUCT(BlueprintType)
struct FDungeonDecorationDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EDungeonDecorationTheme DecorationTheme = EDungeonDecorationTheme::Basic;

	// 일반 방용 총 장식 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinTotalCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxTotalCount = 3;

	// 일반 방용 랜덤 장식 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDungeonDecorationEntry> Decorations;

	// StoreRoom 전용: NPC 기준 뒤쪽
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="StoreRoom")
	TArray<FDungeonDecorationEntry> StoreBackDecorations;

	// StoreRoom 전용: NPC 기준 앞쪽
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="StoreRoom")
	TArray<FDungeonDecorationEntry> StoreFrontDecorations;

	// StoreRoom 전용: NPC 기준 왼쪽
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="StoreRoom")
	TArray<FDungeonDecorationEntry> StoreLeftDecorations;

	// StoreRoom 전용: NPC 기준 오른쪽
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="StoreRoom")
	TArray<FDungeonDecorationEntry> StoreRightDecorations;
};
#pragma endregion
#pragma region Dungeon Room Actor
USTRUCT(BlueprintType)
struct FDungeonRoomActorSpawnEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnWeight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnRadius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnZ = 100.f;
};

USTRUCT(BlueprintType)
struct FDungeonRoomActorSpawnDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EDungeonRoomType RoomType = EDungeonRoomType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDungeonRoomActorSpawnEntry> ActorSpawnEntries;
};
#pragma endregion