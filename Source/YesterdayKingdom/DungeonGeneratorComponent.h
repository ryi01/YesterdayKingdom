// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonEnemySpawnDataRow.h"
#include "Components/ActorComponent.h"
#include "DungeonGeneratorComponent.generated.h"

#pragma region Dungeon Variable
UENUM(BlueprintType)
enum class EDungeonTileType : uint8
{
	Empty,
	Room,
	Corridor,
	Wall,
	Prop,
	Trap,
	Door,
	BossRoom
};
USTRUCT(BlueprintType)
struct FDungeonEdge
{
	GENERATED_BODY()
	UPROPERTY()
	FVector2D U;
	UPROPERTY()
	FVector2D V;
	
	// 초기화 함수
	FDungeonEdge(){}
	FDungeonEdge(const FVector2D& InU, const FVector2D& InV) : U(InU), V(InV){}
	// 선분이 같은지 확인하는 함수
	bool Equal(const FDungeonEdge& Other) const
	{
		return ((U.Equals(Other.U) && V.Equals(Other.V)) || (U.Equals(Other.V) && V.Equals(Other.U)));
	}
};
USTRUCT(BlueprintType)
struct FDungeonTriangle
{
	GENERATED_BODY()
	
	// 삼각형 점 3개
	UPROPERTY()
	FVector2D A;
	UPROPERTY()
	FVector2D B;
	UPROPERTY()
	FVector2D C;
	// 삼각형 중심
	FVector2D CircleCenter;
	// 반지름 => 삼각형들 기반으로 외접원 생성
	UPROPERTY()
	float CircleRadius = 0.f;
	
	// 초기화 함수
	FDungeonTriangle() {}
	FDungeonTriangle(const FVector2D& InA, const FVector2D& InB, const FVector2D& InC) : A(InA), B(InB), C(InC)
	{
		CalculateCircle();
	}
	// 외접원 생성
	void CalculateCircle()
	{
		const float X1 = A.X;
		const float X2 = B.X;
		const float X3 = C.X;
		const float Y1 = A.Y;
		const float Y2 = B.Y;
		const float Y3 = C.Y;
		
		// 외접원 중심을 계산하기 위한 공식의 분모
		// D=2(x1(y2−y3)+x2(y3−y1)+x3(y1−y2))
		const float D = 2.f * (X1 * (Y2 - Y3) + X2 * (Y3 - Y1) + X3 * (Y1 - Y2));
		if (FMath::Abs(D) < 0.001f)
		{
			CircleRadius = 0.f;
			return;
		}
		// 외접원 중심 공식
		// 각 점의 벡터 길이의 제곱값 = x^2 + y^2
		const float ASq = A.SizeSquared();
		const float BSq = B.SizeSquared();
		const float CSq = C.SizeSquared();
		// 외접원의 X구하는 공식
		// Ux=(x1^2+y1^2)(y2−y3)+(x2^2+y2^2)(y3−y1)+(x3^2+y3^2)(y1−y2) / D
		const float CenterX = (ASq * (Y2 - Y3) + BSq * (Y3 - Y1) + CSq * (Y1 - Y2)) / D;
		// 외접원의 Y구하는 공식
		// Uy=(x1^2+y1^2)(x3−x2)+(x2^2+y2^2)(x1−x3)+(x3^2+y3^2)(x2−x1) / D
		const float CenterY = (ASq *(X3 - X2) + BSq * (X1 - X3) + CSq * (X2 - X1) ) / D;
		
		CircleCenter = FVector2D(CenterX, CenterY);
		CircleRadius = FVector2D::Distance(A, CircleCenter);
	}
	// 점이 외접원 내에 있는지 확인
	bool IsPointInsideCircle(const FVector2D& Point) const
	{
		return FVector2D::Distance(Point, CircleCenter) < CircleRadius;
	}
};

// N/S/E/W 체크
struct FDungeonNeighborInfo
{
	// 현재 타일 주변 4방향 정보를 담음
	bool bN = false;
	bool bS = false;
	bool bE = false;
	bool bW = false;
	
	// 4방향중 true값이 몇개인지
	int32 Count() const
	{
		return (bN ? 1 : 0) + (bS ? 1 : 0) + (bE ? 1 : 0) + (bW ? 1 : 0);
	}
};
#pragma endregion

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDungeonGenerated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API UDungeonGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	// ==============================
	// Dungeon Settings
	// ==============================

	UPROPERTY(EditAnywhere, Category="Dungeon|Settings")
	int32 PointCount = 10;

	UPROPERTY(EditAnywhere, Category="Dungeon|Settings")
	float AreaSize = 50.f;

	UPROPERTY(EditAnywhere, Category="Dungeon|Settings")
	float MinDistance = 5.f;

	UPROPERTY(EditAnywhere, Category="Dungeon|Settings")
	int32 MapWidth = 60;

	UPROPERTY(EditAnywhere, Category="Dungeon|Settings")
	int32 MapHeight = 60;

	UPROPERTY(EditAnywhere, Category="Dungeon|Settings")
	float TileSize = 200.f;
	
	// ====================================================
	// Dungeon DataTable
	// ====================================================
	UPROPERTY(EditAnywhere, Category="Dungeon|Data")
	TObjectPtr<UDataTable> DungeonTileVisualDataTable;
	
	UPROPERTY(EditAnywhere, Category="Dungeon|Data")
	TObjectPtr<UDataTable> DungeonDecorationDataTable;
	
	UPROPERTY(EditAnywhere, Category="Dungeon|Data")
	TObjectPtr<UDataTable> DungeonEnemySpawnDataTable;
	
	UPROPERTY(EditAnywhere, Category="Dungeon|Data")
	TObjectPtr<UDataTable> DungeonRoomActorSpawnDataTable;
	
	// ==============================
	// Spawn Options
	// ==============================
	UPROPERTY(EditAnywhere, Category="Dungeon|Boss")
	TSubclassOf<class ABossRoomTrigger> BossRoomTriggerClass;
	
	UPROPERTY(EditAnywhere, Category="Dungeon|Spawn")
	float EnemySpawnRadius = 250.f;

	UPROPERTY(EditAnywhere, Category="Dungeon|Decoration")
	float DecorationSpawnRadius = 350.f;

	UPROPERTY(EditAnywhere, Category="Dungeon|Decoration")
	float DecorationHeight = 5.f;
	
	// ==============================
	// Runtime Data
	// ==============================
	UPROPERTY()
	TArray<FVector2D> Points;

	UPROPERTY()
	TArray<FDungeonRoomInfo> Rooms;

	UPROPERTY()
	TArray<FDungeonTriangle> TriangleList;

	UPROPERTY()
	TArray<FDungeonEdge> MstEdges;

	UPROPERTY()
	TArray<FDungeonEdge> FinalEdges;

	UPROPERTY()
	TArray<EDungeonTileType> MapData;

	UPROPERTY()
	TArray<AActor*> SpawnedDungeonActors;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	bool bGenerationCompleted = false;

	UPROPERTY()
	FVector2D StartPoint;

	UPROPERTY()
	FVector2D EndPoint;
		
	// ==============================
	// 보스 방
	// ==============================
	UPROPERTY(EditAnywhere, Category="Dungeon|Boss")
	TSubclassOf<class ABossRoomEntrance> BossRoomEntranceClass;
	UPROPERTY()
	FVector2D BossRoomEntranceGridLocation = FVector2D::ZeroVector;

	UPROPERTY()
	FIntPoint BossRoomEntranceDirection = FIntPoint::ZeroValue;

	UPROPERTY()
	bool bHasBossRoomEntranceLocation = false;
	
public:	
	// Sets default values for this component's properties
	UDungeonGeneratorComponent();
	UPROPERTY(BlueprintAssignable)
	FOnDungeonGenerated OnDungeonGenerated;

private:
	// ==============================
	// Generation
	// ==============================
	void InitMapData();

	void GeneratePoint();
	void SetupSuperTriangle();
	void Delaunay();
	void DoMST();
	void CreateFinalPath();

	void CalculateStartAndEnd();
	void AssignRoomTypes();

	void CreateMap();
	void SpawnFloorTiles();
	void CreateWalls();

	void MakeCorridorInData(const FDungeonEdge& Edge);
	void DigCorridorLine(FIntPoint From, FIntPoint To);
	
	void ClearDungeon();
	
	// ==============================
	// Map Utils
	// ==============================
	bool IsInMap(int32 X, int32 Y) const;
	int32 GetIndex(int32 X, int32 Y) const;
	
	EDungeonTileType GetTile(int32 X, int32 Y) const;
	void SetTile(int32 X, int32 Y, EDungeonTileType Type);
	
	bool IsWalkableTile(int32 X, int32 Y) const;
	bool IsNextToType(int32 X, int32 Y, EDungeonTileType Type) const;

	int32 CountNeighborWalls(int32 X, int32 Y) const;

	FIntPoint GetDirectionByType(int32 X, int32 Y, EDungeonTileType Type) const;
	FRotator DirectionToRotation(const FIntPoint& Dir) const;
	FVector GridToWorldLocation(const FVector2D& Point, float Z) const;
	
	const FDungeonRoomInfo* FindRoomInfoByCenter(const FVector2D& Center) const;
	int32 GetRoomSizeByRoomType(EDungeonRoomType RoomType) const;
	int32 SpawnStoreDecorationGroup(const TArray<FDungeonDecorationEntry>& Entries, const TArray<FVector2D>& Offsets, const FDungeonRoomInfo& Room, const FRotator& Rotation);
	// ==============================
	// Tile Visual
	// ==============================
	AActor* SpawnTileActor(TSubclassOf<AActor> ActorClass, int32 X, int32 Y, const FString& NamePrefix);
	TSubclassOf<AActor> GetTileVisualClass(EDungeonVisualTileType TileType, EDungeonPieceShape Shape) const;
	
	EDungeonPieceShape GetWallShape(int32 X, int32 Y) const;
	
	FRotator GetWallRotation(int32 X, int32 Y) const;
	
	FName GetRoomTypeRowName(EDungeonRoomType RoomType) const;
	
	FName GetDecorationThemeRowName(EDungeonDecorationTheme Theme) const;
	
	FIntPoint FindStoreForwardDirection(const FDungeonRoomInfo& Room) const;
	FRotator GridDirectionToActorRotation(const FIntPoint& Dir) const;
	FVector2D ConvertLocalStoreOffsetToWorldOffset(const FVector2D& LocalOffset, const FIntPoint& ForwardDir) const;
	// ==============================
	// Spawn
	// ==============================
	void SetPlayerStartLocation();
	void SpawnBossRoomTrigger(class ABossRoomEntrance* BossRoomEntranceActor);
	void SpawnEnemiesByRoomData();
	void SpawnDecorationByRoomData();
	void SpawnRoomActorsByRoomData();
	void SpawnCornerWallOnTile(int32 X, int32 Y, const FIntPoint& DirA, const FIntPoint& DirB);
	void SpawnWallPiece(int32 X, int32 Y, const FIntPoint& Dir, EDungeonPieceShape Shape, const FRotator& Rotator);
	
	void SpawnEnemyAroundPoint(TSubclassOf<AActor> EnemyClass, const FVector2D& Point);
	void SpawnDecorationAroundPoint(TSubclassOf<AActor> DecorationClass, const FVector2D& Point);
	void SpawnStoreRoomDecorations(const FDungeonRoomInfo& Room, const FDungeonDecorationDataRow* DecorationDataRow);
	
	void PlaceCorridorTile(int32 X, int32 Y);

	const FDungeonEnemySpawnDataRow* GetEnemySpawnData(EDungeonRoomType RoomType) const;
	const FDungeonDecorationDataRow* GetDecorationData(EDungeonDecorationTheme  RoomType) const;
	const FDungeonDecorationEntry* PickRandomDecorationEntry(const TArray<FDungeonDecorationEntry>& Entries);
	const FDungeonRoomActorSpawnDataRow* GetRoomActorSpawnData(EDungeonRoomType RoomType) const;

	EDungeonPieceShape GetWallShapeForEdge(int32 X, int32 Y, const FIntPoint& Dir) const;
	FDungeonNeighborInfo GetWallNeighborInfoForEdge(int32 X, int32 Y, const FIntPoint& Dir) const;
	FRotator GetRotationByNeighborInfo(const FDungeonNeighborInfo& Info, EDungeonPieceShape Shape) const;
	FRotator DirectionToInnerWallRotation(const FIntPoint& Dir) const;
	
	FDungeonNeighborInfo GetWalkableNeighborInfo(int32 X, int32 Y) const;
	FDungeonNeighborInfo GetNeighborInfo(int32 X, int32 Y, TFunctionRef<bool(int32, int32)> Predicate) const;
	FVector GetWallOffset(int32 X, int32 Y) const;
	
	AActor* SpawnRoomActorAroundPoint(TSubclassOf<AActor> ActorClass, const FVector2D& Point, float SpawnRadius, float SpawnZ, const FRotator& SpawnRotation);
	AActor* SpawnDecorationAtGridOffset(TSubclassOf<AActor> DecorationClass, const FVector2D& RoomCenter, const FVector2D& GridOffset, float Z, const FRotator& Rotation);
	
	// ==============================
	// Delaunay / MST Utils
	// ==============================
	TArray<FDungeonEdge> CollectUniqueEdges() const;
	
	bool IsSameTriangle(const FDungeonTriangle& A, const FDungeonTriangle& B) const;
	
	bool IsSuperTrianglePoint(const FVector2D& Point) const;
	bool IsEdgeInTriangle(const FDungeonTriangle& Triangle, const FDungeonEdge Edge) const;
	
	void BuildMSTGraph(TMap<FVector2D, TArray<FVector2D>>& OutGraph) const;
	TArray<FVector2D> FindPathBFS(const FVector2D& Start, const FVector2D& End, const TMap<FVector2D, TArray<FVector2D>>& Graph) const;
	FVector2D FindFarthestPointByBFS(const FVector2D& Start, const TMap<FVector2D, TArray<FVector2D>>& Graph) const;
	
	// ==============================
	// NavMesh
	// ==============================
	void BuildNavMesh();
	
	// ==============================
	// 보스 방
	// ==============================
	class ABossRoomEntrance* SpawnBossRoomEntrance();
	void NormalizeBossRoomEntrance();
public:	
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void GenerateDungeon();
};
