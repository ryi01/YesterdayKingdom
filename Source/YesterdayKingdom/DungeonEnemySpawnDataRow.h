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
	Boss		UMETA(DisplayName="Boss")
};
USTRUCT(BlueprintType)
struct FDungeonRoomInfo
{
	GENERATED_BODY()
	
	UPROPERTY()
	FVector2D Center = FVector2D::ZeroVector;
	UPROPERTY()
	EDungeonRoomType RoomType = EDungeonRoomType::Normal;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnChance = 0.5f;
};
USTRUCT(BlueprintType)
struct FDungeonDecorationDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EDungeonRoomType RoomType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDungeonDecorationEntry> Decorations;
};
#pragma endregion
