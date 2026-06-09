// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGeneratorComponent.h"

#include "BossRoomEntrance.h"
#include "BossRoomTrigger.h"
#include "EnemyBase.h"
#include "NavigationSystem.h"
#include "Containers/Queue.h"

// Sets default values for this component's properties
UDungeonGeneratorComponent::UDungeonGeneratorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}
// Called when the game starts
void UDungeonGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();
	GenerateDungeon();
}

void UDungeonGeneratorComponent::GenerateDungeon()
{
	ClearDungeon();
	
	InitMapData();
	
	GeneratePoint();
	Delaunay();
	DoMST();
	
	CalculateStartAndEnd();
	AssignRoomTypes();
	
	CreateFinalPath();
	
	CreateMap();
	CreateWalls();
	
	SetPlayerStartLocation();
	
	SpawnRoomActorsByRoomData();
	SpawnDecorationByRoomData();
	SpawnEnemiesByRoomData();
	ABossRoomEntrance* BossRoomEntranceActor = SpawnBossRoomEntrance();
	SpawnBossRoomTrigger(BossRoomEntranceActor);
	
	BuildNavMesh();
	
	bGenerationCompleted = true;
	
	OnDungeonGenerated.Broadcast();
}

#pragma region Delaunay
// map 데이터 갯수 및 타입 초기화
void UDungeonGeneratorComponent::InitMapData()
{
	MapData.SetNum(MapWidth * MapHeight);
	for (int32 i = 0; i < MapData.Num(); i++)
	{
		MapData[i] = EDungeonTileType::Empty;
	}
}
// 포인트 생성
void UDungeonGeneratorComponent::GeneratePoint()
{
	Points.Empty();
	TriangleList.Empty();
	
	int32 Count = 0;
	while (Points.Num() < PointCount && Count < 1000)
	{
		// 횟수 증가
		Count++;
		const float EdgePadding = 8.f;
		// 랜덤으로 X와 Y 값 구하기
		float RX = FMath::RandRange(EdgePadding, MapWidth - EdgePadding);
		float RY = FMath::RandRange(EdgePadding, MapHeight - EdgePadding);
		// 랜덤으로 구한 x, y를 기반으로 포인트 생성
		FVector2D NewPoint = FVector2D(FMath::RoundToInt(RX), FMath::RoundToInt(RY));
		// 가까운지 확인을 위한 bool값 선언
		bool bTooClose = false;
		// point리스트를 순회하며 거리가 가까운지 파악
		for (FVector2D Point : Points)
		{
			if (FVector2D::Distance(Point, NewPoint) < MinDistance)
			{
				bTooClose = true;
				break;
			}
		}
		if (!bTooClose) Points.Add(NewPoint);
	}
	
	for (const FVector2D& Point : Points)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Point X:%f Y:%f"),
			Point.X,
			Point.Y
		);
	}
}
// 델루네 삼각 기법
void UDungeonGeneratorComponent::Delaunay()
{
	SetupSuperTriangle();
	
	UE_LOG(LogTemp, Warning, TEXT("[Delaunay] After Setup TriangleList:%d"), TriangleList.Num());

	for (const FVector2D& Point : Points)
	{
		TArray<FDungeonTriangle> BadTriangles;
		for (const FDungeonTriangle& Triangle : TriangleList)
		{
			if (Triangle.IsPointInsideCircle(Point))
			{
				BadTriangles.Add(Triangle);
			}
		}
		UE_LOG(LogTemp, Warning,
					TEXT("[Delaunay] Point(%f,%f) BadTriangles:%d BeforeTriangleList:%d"),
					Point.X, Point.Y, BadTriangles.Num(), TriangleList.Num());
		TArray<FDungeonEdge> Polygon;
		for (const FDungeonTriangle& Triangle : BadTriangles)
		{
			TArray<FDungeonEdge> Edges;
			Edges.Add(FDungeonEdge(Triangle.A, Triangle.B));
			Edges.Add(FDungeonEdge(Triangle.B, Triangle.C));
			Edges.Add(FDungeonEdge(Triangle.C, Triangle.A));
			
			for (const FDungeonEdge& Edge : Edges)
			{
				bool bIsShared = false;
				for (const FDungeonTriangle& OtherTriangle : BadTriangles)
				{
					if (Triangle.A.Equals(OtherTriangle.A) && Triangle.B.Equals(OtherTriangle.B) && Triangle.C.Equals(OtherTriangle.C) ) continue;
					if (IsEdgeInTriangle(OtherTriangle, Edge))
					{
						bIsShared = true;
						break;
					}
				}
				if (!bIsShared) Polygon.Add(Edge);
			}
		}
		for (const FDungeonTriangle& BadTriangle : BadTriangles)
		{
			for (int32 i = TriangleList.Num() - 1; i >= 0; i--)
			{
				if (IsSameTriangle(TriangleList[i], BadTriangle))
				{
					TriangleList.RemoveAt(i);
				}
			}
		}
		for (const FDungeonEdge& Edge : Polygon)
		{
			TriangleList.Add(FDungeonTriangle(Edge.U, Edge.V, Point));
		}
		UE_LOG(LogTemp, Warning,
			TEXT("[Delaunay] After Add TriangleList:%d"),
			TriangleList.Num());
	}
	UE_LOG(LogTemp, Warning, TEXT("[Delaunay] Before RemoveSuper TriangleList:%d"), TriangleList.Num());


	for (int32 i = TriangleList.Num() - 1; i >= 0; i--)
	{
		if (IsSuperTrianglePoint(TriangleList[i].A) || IsSuperTrianglePoint(TriangleList[i].B) || IsSuperTrianglePoint(TriangleList[i].C))
		{
			TriangleList.RemoveAt(i);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("[Delaunay] After RemoveSuper TriangleList:%d"), TriangleList.Num());

}
// 슈퍼 삼각형 만들기
void UDungeonGeneratorComponent::SetupSuperTriangle()
{
	TriangleList.Empty();
	const float Size = FMath::Max(MapWidth, MapHeight);
	const float Margin = Size * 2.f;

	FVector2D V1 = FVector2D(MapWidth * 0.5f, MapHeight + Margin);
	FVector2D V2 = FVector2D(MapWidth + Margin, -Margin);
	FVector2D V3 = FVector2D(-Margin, -Margin);

	TriangleList.Add(FDungeonTriangle(V1, V2, V3));
}

void UDungeonGeneratorComponent::DoMST()
{
	MstEdges.Empty();
	if (Points.Num() == 0) return;
	
	TArray<FDungeonEdge> AllEdges = CollectUniqueEdges();
	UE_LOG(LogTemp, Warning,
	TEXT("[DoMST] Points:%d TriangleList:%d AllEdges:%d"),
	Points.Num(), TriangleList.Num(), AllEdges.Num());
	TArray<FVector2D> ReachedPoints;
	TArray<FVector2D> UnreachedPoints = Points;
	
	ReachedPoints.Add(UnreachedPoints[0]);
	UnreachedPoints.RemoveAt(0);

	while (UnreachedPoints.Num() > 0)
	{
		float MinDist = TNumericLimits<float>::Max();
		FDungeonEdge BestEdge;
		int32 BestPointIndex = INDEX_NONE;
		for (const FVector2D& Reached : ReachedPoints)
		{
			for (int32 i = 0; i < UnreachedPoints.Num(); i++)
			{
				const FVector2D& Unreached = UnreachedPoints[i];
				for (const FDungeonEdge& Edge : AllEdges)
				{
					const bool bConnected = (Edge.U.Equals(Reached) && Edge.V.Equals(Unreached) ||(Edge.U.Equals(Unreached) && Edge.V.Equals(Reached)));
					if (bConnected)
					{
						const float Dist = FVector2D::Distance(Reached, Unreached);
						if (Dist < MinDist)
						{
							MinDist = Dist;
							BestEdge = Edge;
							BestPointIndex = i;
						}
					}
				}
			}
		}
		
		if (BestPointIndex != INDEX_NONE)
		{
			MstEdges.Add(BestEdge);
			ReachedPoints.Add(UnreachedPoints[BestPointIndex]);
			UnreachedPoints.RemoveAt(BestPointIndex);
		}
		else
		{
			break;
		}
	}
}
void UDungeonGeneratorComponent::CreateFinalPath()
{
	FinalEdges.Empty();
	FinalEdges.Append(MstEdges);
	TArray<FDungeonEdge> AllDelaunayEdges = CollectUniqueEdges();
	for (const FDungeonEdge& Edge: AllDelaunayEdges)
	{
		bool bIsMST = false;
		for (const FDungeonEdge& MstEdge : MstEdges)
		{
			if (MstEdge.Equal(Edge))
			{
				bIsMST = true;
				break;
			}
		}
		if (bIsMST) continue;

		// 보스방으로 바로 이어지는 지름길 방지
		if (Edge.U.Equals(EndPoint) || Edge.V.Equals(EndPoint)) continue;
		// StoreRoom에 랜덤 지름길이 추가로 붙는 것 방지
		if (bHasSelectedStorePoint)
		{
			if (Edge.U.Equals(SelectedStorePoint) || Edge.V.Equals(SelectedStorePoint)) continue;
		}
		
		if (FMath::FRand() < 0.3f) FinalEdges.Add(Edge);
	}
}
#pragma endregion

void UDungeonGeneratorComponent::CalculateStartAndEnd()
{
	// 방이 2개 이상이고 MST 연결선이 존재하는가
	if (Points.Num() < 2 || MstEdges.Num() == 0) return;
	
	// 존재하는 경우에 그래프 구조로 변환
	TMap<FVector2D, TArray<FVector2D>> Graph;
	BuildMSTGraph(Graph);
	
	// 아무점을 하나 잡고 BFS를 확인함 => 가장 먼 지점 확인
	FVector2D FirstFar = FindFarthestPointByBFS(Points[0], Graph);
	// 확정된 가장 먼지점에서 반대로 BFS를 해서 가장 먼지점을 고름 => StartPoint
	FVector2D SecondFar = FindFarthestPointByBFS(FirstFar, Graph);

	StartPoint = FirstFar;
	EndPoint = SecondFar;
}

void UDungeonGeneratorComponent::AssignRoomTypes()
{
	Rooms.Empty();
	SelectedStorePoint = FVector2D::ZeroVector;
	bHasSelectedStorePoint = false;
	// 그래프 생성
	TMap<FVector2D, TArray<FVector2D>> Graph;
	BuildMSTGraph(Graph);
	
	// 연결된 경로를 따라 방을 array로 모음
	// S -- A -- B -- C -- Boss순으로 있을 때, [S,A,B,C,Boss]로 저장
	// Start -> Boss 메인 경로
	// Start -> Boss 메인 경로
	const TArray<FVector2D> MainPath = FindPathBFS(StartPoint, EndPoint, Graph);

	// ========================================================
	// Store는 MainPath 위가 아니라 MainPath 옆 사이드룸으로 선택
	// ========================================================
	for (const FVector2D& PathPoint : MainPath)
	{
		const TArray<FVector2D>* Neighbors = Graph.Find(PathPoint);
		if (!Neighbors) continue;

		for (const FVector2D& Neighbor : *Neighbors)
		{
			if (Neighbor.Equals(StartPoint)) continue;
			if (Neighbor.Equals(EndPoint)) continue;

			const bool bIsOnMainPath = MainPath.ContainsByPredicate([&Neighbor](const FVector2D& MainPathPoint)
			{
				return MainPathPoint.Equals(Neighbor);
			});

			if (bIsOnMainPath) continue;

			SelectedStorePoint = Neighbor;
			bHasSelectedStorePoint = true;
			break;
		}

		if (bHasSelectedStorePoint) break;
	}
	int32 GuaranteedElitePathIndex = INDEX_NONE;

	if (MainPath.Num() >= 3)
	{
		GuaranteedElitePathIndex = MainPath.Num() - 2;
	}
	
	// 모든 방을 순회하며
	for (const FVector2D& Point : Points)
	{
		FDungeonRoomInfo RoomInfo;
		RoomInfo.Center = Point;
		RoomInfo.RoomType = EDungeonRoomType::Normal;
		RoomInfo.DecorationTheme = EDungeonDecorationTheme::Basic;

		if (Point.Equals(StartPoint))
		{
			RoomInfo.RoomType = EDungeonRoomType::Start;
			RoomInfo.DecorationTheme = EDungeonDecorationTheme::StartRoom;
		}
		else if (Point.Equals(EndPoint))
		{
			RoomInfo.RoomType = EDungeonRoomType::Boss;
			RoomInfo.DecorationTheme = EDungeonDecorationTheme::BossEntrance;
		}
		else if (bHasSelectedStorePoint && Point.Equals(SelectedStorePoint))
		{
			RoomInfo.RoomType = EDungeonRoomType::Store;
			RoomInfo.DecorationTheme = EDungeonDecorationTheme::StoreRoom;
		}
		else
		{
			const int32 PathIndex = MainPath.IndexOfByPredicate([&Point](const FVector2D& PathPoint)
			{
				return PathPoint.Equals(Point);
			});

			if (PathIndex != INDEX_NONE && PathIndex == GuaranteedElitePathIndex)
			{
				RoomInfo.RoomType = EDungeonRoomType::Elite;
				RoomInfo.DecorationTheme = EDungeonDecorationTheme::WeaponRoom;
			}
			else
			{
				RoomInfo.RoomType = EDungeonRoomType::Normal;
				RoomInfo.DecorationTheme = FMath::RandBool()
					? EDungeonDecorationTheme::Basic
					: EDungeonDecorationTheme::Broken;
			}
		}

		Rooms.Add(RoomInfo);
	}
}

void UDungeonGeneratorComponent::CreateMap()
{
	if (MapData.Num() == 0) return;
	for (const FVector2D& Point : Points)
	{
		const FDungeonRoomInfo* RoomInfo = FindRoomInfoByCenter(Point);
		const EDungeonRoomType RoomType = RoomInfo ? RoomInfo->RoomType : EDungeonRoomType::Normal;
		
		// 방 크기를 랜덤으로 잡고
		const int32 CurrentRoomSize = GetRoomSizeByRoomType(RoomType);
		// 방의 중심좌표를 구함
		const int32 CX = FMath::RoundToInt(Point.X);
		const int32 CY = FMath::RoundToInt(Point.Y);
		
		// 보스 방인가를 확인하고 사이즈를 조절함 
		const bool bIsBossRoom = RoomType == EDungeonRoomType::Boss;
		
		for (int32 X = CX - CurrentRoomSize; X <= CX + CurrentRoomSize; X++ )
		{
			for (int32 Y = CY - CurrentRoomSize; Y <= CY + CurrentRoomSize; Y++)
			{
				if (!IsInMap(X, Y)) continue;
				if (GetTile(X, Y) == EDungeonTileType::Empty)
				{
					SetTile(X, Y, bIsBossRoom ? EDungeonTileType::BossRoom : EDungeonTileType::Room);
				}
			}
		}
	}
	// 지름길 추가
	for (const FDungeonEdge& Edge :FinalEdges)
	{
		MakeCorridorInData(Edge);
	}
	
	NormalizeBossRoomEntrance();

	SpawnFloorTiles();
}

void UDungeonGeneratorComponent::SpawnFloorTiles()
{
	for (int32 X = 0; X < MapWidth; X++)
	{
		for (int32 Y = 0; Y < MapHeight; Y++)
		{
			const EDungeonTileType Tile = GetTile(X, Y);

			EDungeonVisualTileType VisualType;
			FString Prefix;
			if (Tile == EDungeonTileType::Room)
			{
				VisualType = EDungeonVisualTileType::Room;
				Prefix = TEXT("Room");
			}
			else if (Tile == EDungeonTileType::BossRoom)
			{
				VisualType = EDungeonVisualTileType::BossRoom;
				Prefix = TEXT("BossRoom");
			}
			else if (Tile == EDungeonTileType::Corridor)
			{
				VisualType = EDungeonVisualTileType::Corridor;
				Prefix = TEXT("Corridor");
			}
			else
			{
				continue;
			}
			
			TSubclassOf<AActor> FloorClass = GetTileVisualClass(VisualType, EDungeonPieceShape::Straight);
			if (FloorClass) SpawnTileActor(FloorClass, X, Y, Prefix);
		}
	}
}



void UDungeonGeneratorComponent::CreateWalls()
{
	for (int32 X = 0; X < MapWidth; X++)
	{
		for (int32 Y = 0; Y < MapHeight; Y++)
		{
			if (!IsWalkableTile(X, Y)) continue;
			const bool bN = GetTile(X, Y + 1) == EDungeonTileType::Empty;
			const bool bS = GetTile(X, Y - 1) == EDungeonTileType::Empty;
			const bool bE = GetTile(X + 1, Y) == EDungeonTileType::Empty;
			const bool bW = GetTile(X - 1, Y) == EDungeonTileType::Empty;
			
			bool bUsedN = false;
			bool bUsedS = false;
			bool bUsedE = false;
			bool bUsedW = false;
			
			// 북동 코너
			if (bN && bE)
			{
				SpawnCornerWallOnTile(X, Y, FIntPoint(0, 1), FIntPoint(1, 0));
				bUsedN = true;
				bUsedE = true;
			}

			// 남동 코너
			if (bE && bS)
			{
				SpawnCornerWallOnTile(X, Y, FIntPoint(1, 0), FIntPoint(0, -1));
				bUsedE = true;
				bUsedS = true;
			}

			// 남서 코너
			if (bS && bW)
			{
				SpawnCornerWallOnTile(X, Y, FIntPoint(0, -1), FIntPoint(-1, 0));
				bUsedS = true;
				bUsedW = true;
			}

			// 북서 코너
			if (bW && bN)
			{
				SpawnCornerWallOnTile(X, Y, FIntPoint(-1, 0), FIntPoint(0, 1));
				bUsedW = true;
				bUsedN = true;
			}

			// 코너에 사용되지 않은 edge만 Straight 생성
			if (bN && !bUsedN)
			{
				const FIntPoint Dir(0, 1);
				SpawnWallPiece(X, Y, Dir, EDungeonPieceShape::Straight, DirectionToRotation(Dir));
			}

			if (bS && !bUsedS)
			{
				const FIntPoint Dir(0, -1);
				SpawnWallPiece(X, Y, Dir, EDungeonPieceShape::Straight, DirectionToRotation(Dir));
			}

			if (bE && !bUsedE)
			{
				const FIntPoint Dir(1, 0);
				SpawnWallPiece(X, Y, Dir, EDungeonPieceShape::Straight, DirectionToRotation(Dir));
			}

			if (bW && !bUsedW)
			{
				const FIntPoint Dir(-1, 0);
				SpawnWallPiece(X, Y, Dir, EDungeonPieceShape::Straight, DirectionToRotation(Dir));
			}
		}
	}
}

void UDungeonGeneratorComponent::MakeCorridorInData(const FDungeonEdge& Edge)
{
	// 가정 A(5, 5) B(10, 10)인 경우
	FIntPoint Start(FMath::RoundToInt(Edge.U.X), FMath::RoundToInt(Edge.U.Y));
	FIntPoint End(FMath::RoundToInt(Edge.V.X), FMath::RoundToInt(Edge.V.Y));
	
	// 가로가 먼저인지 세로가 먼저인지 랜덤으로 뽑음 
	// 가로 ㄴ 모양
	// 세로 역기억 모양
	const bool bHorizontalFirst = FMath::RandBool();
	
	FIntPoint Mid;
	
	// 꺽이는 부분 포인트 잡기 
	// (5,5) → Mid : (10,5) → (10,10)
	if (bHorizontalFirst)
	{
		Mid = FIntPoint(End.X, Start.Y);
	}
	else
	{
		// (5,5) → Mid : (5,10) → (10,10)
		Mid = FIntPoint(Start.X, End.Y);
	}
	
	// 꺽이는 복독가 만들어질 때까지 반복
	DigCorridorLine(Start, Mid);
	DigCorridorLine(Mid, End);
}

void UDungeonGeneratorComponent::DigCorridorLine(FIntPoint From, FIntPoint To)
{
	int32 X = From.X;
	int32 Y = From.Y;

	while (X != To.X)
	{
		PlaceCorridorTile(X, Y);
		X += X < To.X ? 1 : -1;
	}

	while (Y != To.Y)
	{
		PlaceCorridorTile(X, Y);
		Y += Y < To.Y ? 1 : -1;
	}

	PlaceCorridorTile(X, Y);
}

void UDungeonGeneratorComponent::ClearDungeon()
{
	for (AActor* Actor:SpawnedDungeonActors)
	{
		if (IsValid(Actor)) Actor->Destroy();
	}
	
	SpawnedDungeonActors.Empty();
	Points.Empty();
	Rooms.Empty();
	TriangleList.Empty();
	MstEdges.Empty();
	FinalEdges.Empty();
	MapData.Empty();
	
	BossRoomEntranceGridLocation = FVector2D::ZeroVector;
	BossRoomEntranceDirection = FIntPoint::ZeroValue;
	bHasBossRoomEntranceLocation = false;
	
	SelectedStorePoint = FVector2D::ZeroVector;
	bHasSelectedStorePoint = false;
	
	bGenerationCompleted = false;
}

bool UDungeonGeneratorComponent::IsInMap(int32 X, int32 Y) const
{
	return X >= 0 && X < MapWidth && Y >= 0 && Y < MapHeight;
}

int32 UDungeonGeneratorComponent::GetIndex(int32 X, int32 Y) const
{
	return Y * MapWidth + X;
}

EDungeonTileType UDungeonGeneratorComponent::GetTile(int32 X, int32 Y) const
{
	if (!IsInMap(X, Y)) return EDungeonTileType::Empty;

	return MapData[GetIndex(X, Y)];
}

void UDungeonGeneratorComponent::SetTile(int32 X, int32 Y, EDungeonTileType Type)
{
	if (!IsInMap(X, Y)) return;
	MapData[GetIndex(X, Y)] = Type;
}


bool UDungeonGeneratorComponent::IsNextToType(int32 X, int32 Y, EDungeonTileType Type) const
{
	return GetTile(X + 1, Y) == Type || GetTile(X - 1, Y) == Type || GetTile(X, Y + 1) == Type || GetTile(X, Y - 1) == Type;
}

int32 UDungeonGeneratorComponent::CountNeighborWalls(int32 X, int32 Y) const
{
	int32 Count = 0;
	
	for (int32 OffsetX = -1 ; OffsetX <= 1; OffsetX++)
	{
		for (int32 OffsetY = -1; OffsetY <= 1; OffsetY++)
		{
			if (OffsetX == 0 && OffsetY == 0) continue;
			const int32 NX = X + OffsetX;
			const int32 NY = Y + OffsetY;
			
			if (!IsInMap(NX, NY)) continue;
			if (GetTile(NX, NY) == EDungeonTileType::Wall || GetTile(NX, NY) == EDungeonTileType::Empty)
			{
				Count++;
			}
		}
	}
	return Count;
}

FIntPoint UDungeonGeneratorComponent::GetDirectionByType(int32 X, int32 Y, EDungeonTileType Type) const
{
	const TArray<FIntPoint> Dirs = {
		FIntPoint(0,1),
		FIntPoint(0,-1),
		FIntPoint(-1,0),
		FIntPoint(1,0)
	};
	
	
	for (const FIntPoint& Dir : Dirs)
	{
		const int32 NX = X + Dir.X;
		const int32 NY = Y + Dir.Y;
		
		if (!IsInMap(NX, NY)) continue;
		if (GetTile(NX, NY) == Type) return Dir;
	}
	return FIntPoint::ZeroValue;	
}

FRotator UDungeonGeneratorComponent::DirectionToRotation(const FIntPoint& Dir) const
{
	if (Dir == FIntPoint(-1, 0))
	{
		return FRotator(0.f, -90.f, 0.f);
	}

	if (Dir == FIntPoint(1, 0))
	{
		return FRotator(0.f, 90.f, 0.f);
	}

	if (Dir == FIntPoint(0, -1))
	{
		return FRotator::ZeroRotator;
	}

	if (Dir == FIntPoint(0, 1))
	{
		return FRotator(0.f, 180.f, 0.f);
	}

	return FRotator::ZeroRotator;
}

AActor* UDungeonGeneratorComponent::SpawnTileActor(TSubclassOf<AActor> ActorClass, int32 X, int32 Y,
	const FString& NamePrefix)
{
	if (!ActorClass || !GetWorld()) return nullptr;
	
	const FVector Location = GridToWorldLocation(FVector2D(X, Y), 0.f);
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ActorClass, Location, FRotator::ZeroRotator);
	
	if (Spawned)
	{
		SpawnedDungeonActors.Add(Spawned);
		Spawned->SetActorLabel(FString::Printf(TEXT("%s_%d_%d"), *NamePrefix, X, Y));
	}
	return Spawned;
}

TSubclassOf<AActor> UDungeonGeneratorComponent::GetTileVisualClass(EDungeonVisualTileType TileType,
	EDungeonPieceShape Shape) const
{
	if (!DungeonTileVisualDataTable) return nullptr;
	for (const FName& RowName : DungeonTileVisualDataTable->GetRowNames())
	{
		const FDungeonTileVisualDataRow* Row = DungeonTileVisualDataTable->FindRow<FDungeonTileVisualDataRow>(RowName, TEXT("TileVisual"));
		if (!Row) continue;
		if (Row->TileType == TileType && Row->Shape == Shape && Row->TileClasses.Num() > 0)
		{
			const int32 Index = FMath::RandRange(0, Row->TileClasses.Num() - 1);
			return Row->TileClasses[Index];
		}
	}
	
	return nullptr;
}

EDungeonPieceShape UDungeonGeneratorComponent::GetWallShape(int32 X, int32 Y) const
{
	const FDungeonNeighborInfo Info = GetWalkableNeighborInfo(X, Y);
	// 2방향 이상이면 코너나 straight로 결정
	if (Info.Count() == 2)
	{
		if ((Info.bN && Info.bE) || (Info.bE && Info.bS) || (Info.bS && Info.bW) || (Info.bW && Info.bN))
		{
			return EDungeonPieceShape::Corner;
		}
	}
	return EDungeonPieceShape::Straight;
}

FRotator UDungeonGeneratorComponent::GetWallRotation(int32 X, int32 Y) const
{
	const FDungeonNeighborInfo Info = GetWalkableNeighborInfo(X, Y);
	const EDungeonPieceShape Shape = GetWallShape(X, Y);

	return GetRotationByNeighborInfo(Info, Shape);
}

FName UDungeonGeneratorComponent::GetRoomTypeRowName(EDungeonRoomType RoomType) const
{
	const FString NameString = StaticEnum<EDungeonRoomType>()->GetNameStringByValue(static_cast<int64>(RoomType));
	return FName(*NameString);
}

FName UDungeonGeneratorComponent::GetDecorationThemeRowName(EDungeonDecorationTheme Theme) const
{
	switch (Theme)
	{
	case EDungeonDecorationTheme::Basic:
		return FName(TEXT("Basic"));
	case EDungeonDecorationTheme::Storage:
		return FName(TEXT("Storage"));
	case EDungeonDecorationTheme::Broken:
		return FName(TEXT("Broken"));
	case EDungeonDecorationTheme::WeaponRoom:
		return FName(TEXT("WeaponRoom"));
	case EDungeonDecorationTheme::BossEntrance:
		return FName(TEXT("BossEntrance"));
	case EDungeonDecorationTheme::StartRoom:
		return FName(TEXT("StartRoom"));
	case EDungeonDecorationTheme::StoreRoom:
		return FName(TEXT("StoreRoom"));
	default:
		return NAME_None;
	}
}

void UDungeonGeneratorComponent::SetPlayerStartLocation()
{
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!PlayerPawn) return;
	const FVector Location = GridToWorldLocation(StartPoint, 100.f);
	PlayerPawn->SetActorLocation(Location);
}

void UDungeonGeneratorComponent::SpawnBossRoomTrigger(ABossRoomEntrance* BossRoomEntranceActor)
{
	if (!BossRoomTriggerClass || !GetWorld()) return;

	bool bFoundBossTile = false;

	int32 MinX = TNumericLimits<int32>::Max();
	int32 MaxX = TNumericLimits<int32>::Lowest();
	int32 MinY = TNumericLimits<int32>::Max();
	int32 MaxY = TNumericLimits<int32>::Lowest();
	for (int32 X = 0; X < MapWidth; X++)
	{
		for (int32 Y = 0; Y < MapHeight; Y++)
		{
			if (GetTile(X, Y) != EDungeonTileType::BossRoom) continue;

			bFoundBossTile = true;

			MinX = FMath::Min(MinX, X);
			MaxX = FMath::Max(MaxX, X);
			MinY = FMath::Min(MinY, Y);
			MaxY = FMath::Max(MaxY, Y);
		}
	}
	
	if (!bFoundBossTile) return;

	const float CenterX = (MinX + MaxX) * 0.5f;
	const float CenterY = (MinY + MaxY) * 0.5f;

	const FVector Location = GridToWorldLocation(FVector2D(CenterX, CenterY), 100.f);

	const float Width = (MaxX - MinX + 1) * TileSize;
	const float Height = (MaxY - MinY + 1) * TileSize;
	
	const FVector TriggerExtent(Width * 0.5f, Height * 0.5f, 300.f);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABossRoomTrigger* Trigger = GetWorld()->SpawnActor<ABossRoomTrigger>(BossRoomTriggerClass, Location, FRotator::ZeroRotator, SpawnParams);

	if (!Trigger) return;

	Trigger->SetTriggerExtent(TriggerExtent);
	Trigger->SetBossRoomEntrance(BossRoomEntranceActor);

	SpawnedDungeonActors.Add(Trigger);
#if WITH_EDITOR
	Trigger->SetActorLabel(TEXT("BossRoomTrigger"));
#endif

	UE_LOG(LogTemp, Warning,
		TEXT("[Dungeon] BossRoomTrigger Spawned / Min=(%d,%d) Max=(%d,%d) Extent=%s Location=%s"),
		MinX,
		MinY,
		MaxX,
		MaxY,
		*TriggerExtent.ToString(),
		*Location.ToString()
	);
}

void UDungeonGeneratorComponent::SpawnEnemiesByRoomData()
{
	for (const FDungeonRoomInfo& Room : Rooms)
	{
		if (Room.RoomType == EDungeonRoomType::Start) continue;
		if (Room.RoomType == EDungeonRoomType::Store) continue;
		const FDungeonEnemySpawnDataRow* SpawnDataRow = GetEnemySpawnData(Room.RoomType);
		if (!SpawnDataRow)
		{
			UE_LOG(LogTemp, Error, TEXT("[Dungeon] SpawnDataRow NULL / RoomType=%s / RowName=%s"),
				*StaticEnum<EDungeonRoomType>()->GetNameStringByValue(static_cast<int64>(Room.RoomType)),
				*GetRoomTypeRowName(Room.RoomType).ToString()
			);
			continue;
		}
		for (const FDungeonEnemySpawnEntry& Entry : SpawnDataRow->EnemySpawnEntries)
		{
			if (!Entry.EnemyClass) continue;;
			const int32 Count = FMath::RandRange(Entry.MinCount, Entry.MaxCount);
			for (int32 i = 0; i < Count; i++)
			{
				SpawnEnemyAroundPoint(Entry.EnemyClass, Room.Center);
			}
		}
	}
}

void UDungeonGeneratorComponent::SpawnDecorationByRoomData()
{
	for (const FDungeonRoomInfo& Room : Rooms)
	{
		const FDungeonDecorationDataRow* DecorationDataRow = GetDecorationData(Room.DecorationTheme);
		if (!DecorationDataRow) continue;

		if (Room.DecorationTheme == EDungeonDecorationTheme::StoreRoom)
		{
			SpawnStoreRoomDecorations(Room, DecorationDataRow);
			continue;
		}
		if (DecorationDataRow->Decorations.Num() == 0) continue;
		
		const int32 TotalCount = FMath::RandRange(DecorationDataRow->MinTotalCount,DecorationDataRow->MaxTotalCount);

		for (int32 i = 0; i < TotalCount; i++)
		{
			const FDungeonDecorationEntry* PickedEntry = PickRandomDecorationEntry(DecorationDataRow->Decorations);
			if (!PickedEntry || !PickedEntry->DecorationClass) continue;

			SpawnDecorationAroundPoint(PickedEntry->DecorationClass, Room.Center);
		}
	}
}

void UDungeonGeneratorComponent::SpawnRoomActorsByRoomData()
{
	for (const FDungeonRoomInfo& Room : Rooms)
	{
		const FDungeonRoomActorSpawnDataRow* SpawnDataRow = GetRoomActorSpawnData(Room.RoomType);
		if (!SpawnDataRow) continue;

		for (const FDungeonRoomActorSpawnEntry& Entry : SpawnDataRow->ActorSpawnEntries)
		{
			if (!Entry.ActorClass) continue;

			const int32 Count = FMath::RandRange(Entry.MinCount, Entry.MaxCount);

			for (int32 i = 0; i < Count; i++)
			{
				FVector2D SpawnPoint = Room.Center;
				FRotator SpawnRotation = FRotator::ZeroRotator;

				if (Room.RoomType == EDungeonRoomType::Store)
				{
					const FIntPoint StoreForwardDir = FindStoreForwardDirection(Room);
					const FVector2D BackOffset = ConvertLocalStoreOffsetToWorldOffset(FVector2D(0.f, -2.f),StoreForwardDir);
					SpawnPoint = Room.Center + BackOffset;
					SpawnRotation = GridDirectionToActorRotation(StoreForwardDir);
				}

				AActor* Spawned = SpawnRoomActorAroundPoint(Entry.ActorClass, SpawnPoint, Entry.SpawnRadius, Entry.SpawnZ, SpawnRotation);
				if (Spawned)
				{
					UE_LOG(LogTemp, Warning,
						TEXT("[Dungeon] RoomActor Spawned / RoomType=%s / Actor=%s / Location=%s"),
						*GetRoomTypeRowName(Room.RoomType).ToString(),
						*GetNameSafe(Spawned),
						*Spawned->GetActorLocation().ToString()
					);
				}
			}
		}
	}
}

AActor* UDungeonGeneratorComponent::SpawnRoomActorAroundPoint(TSubclassOf<AActor> ActorClass, const FVector2D& Point,
	float SpawnRadius, float SpawnZ, const FRotator& SpawnRotation)
{
	if (!ActorClass || !GetWorld()) return nullptr;

	FVector Location = GridToWorldLocation(Point, SpawnZ);

	if (SpawnRadius > 0.f)
	{
		const FVector2D Offset = FMath::RandPointInCircle(SpawnRadius);
		Location.X += Offset.X;
		Location.Y += Offset.Y;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ActorClass, Location, SpawnRotation, SpawnParams);

	if (!Spawned) return nullptr;

	SpawnedDungeonActors.Add(Spawned);

#if WITH_EDITOR
	Spawned->SetActorLabel(TEXT("RoomActor"));
#endif

	return Spawned;
}

const FDungeonRoomActorSpawnDataRow* UDungeonGeneratorComponent::GetRoomActorSpawnData(EDungeonRoomType RoomType) const
{
	if (!DungeonRoomActorSpawnDataTable) return nullptr;

	return DungeonRoomActorSpawnDataTable->FindRow<FDungeonRoomActorSpawnDataRow>(GetRoomTypeRowName(RoomType),TEXT("DungeonRoomActorSpawnData"));
}

void UDungeonGeneratorComponent::SpawnCornerWallOnTile(int32 X, int32 Y, const FIntPoint& DirA, const FIntPoint& DirB)
{
	FRotator Rotation = FRotator::ZeroRotator;
	if ((DirA == FIntPoint(0, 1) && DirB == FIntPoint(1, 0)) || (DirA == FIntPoint(1, 0) && DirB == FIntPoint(0, 1)))
	{
		Rotation = FRotator(0.f, 180.f, 0.f);
	}
	else if ((DirA == FIntPoint(1, 0) && DirB == FIntPoint(0, -1)) || (DirA == FIntPoint(0, -1) && DirB == FIntPoint(1, 0)))
	{
		Rotation = FRotator(0.f, 90.f, 0.f);
	}
	// 남서 코너
	else if ((DirA == FIntPoint(0, -1) && DirB == FIntPoint(-1, 0)) || (DirA == FIntPoint(-1, 0) && DirB == FIntPoint(0, -1)))
	{
		Rotation = FRotator(0.f, 0.f, 0.f);
	}
	// 북서 코너
	else if ((DirA == FIntPoint(-1, 0) && DirB == FIntPoint(0, 1)) || (DirA == FIntPoint(0, 1) && DirB == FIntPoint(-1, 0)))
	{
		Rotation = FRotator(0.f, 270.f, 0.f);
	}
	SpawnWallPiece(X, Y, FIntPoint::ZeroValue, EDungeonPieceShape::Corner, Rotation);
}

void UDungeonGeneratorComponent::SpawnWallPiece(int32 X, int32 Y, const FIntPoint& Dir, EDungeonPieceShape Shape, const FRotator& Rotator)
{
	TSubclassOf<AActor> WallClass = GetTileVisualClass(EDungeonVisualTileType::Wall, Shape);
	if (!WallClass || !GetWorld()) return;
	
	FVector Location = GridToWorldLocation(FVector2D(X, Y), 0.f);
	const float Half = TileSize * 0.5f;
	
	if (Shape == EDungeonPieceShape::Straight)
	{
		Location.X += Dir.X * Half;
		Location.Y += Dir.Y * Half;
	}
	AActor* Wall = GetWorld()->SpawnActor<AActor>(WallClass, Location, Rotator);
	if (Wall)
	{
		SpawnedDungeonActors.Add(Wall);

#if WITH_EDITOR
		const FString ShapeName = Shape == EDungeonPieceShape::Corner
			? TEXT("Corner")
			: TEXT("Straight");

		Wall->SetActorLabel(
			FString::Printf(TEXT("Wall_%s_%d_%d"), *ShapeName, X, Y)
		);
#endif
	}
}

void UDungeonGeneratorComponent::SpawnEnemyAroundPoint(TSubclassOf<AActor> EnemyClass, const FVector2D& Point)
{
	if (!EnemyClass || !GetWorld()) return;
	const FVector2D Offset = FMath::RandPointInCircle(EnemySpawnRadius);
	FVector Location = GridToWorldLocation(Point, 100.f);
	Location.X += Offset.X;
	Location.Y += Offset.Y;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(EnemyClass, Location, FRotator::ZeroRotator, SpawnParams);
	if (!Spawned) return;
	SpawnedDungeonActors.Add(Spawned);
	if (AEnemyBase* EnemyBase = Cast<AEnemyBase>(Spawned))
	{
		const FVector RoomCenterLocation = GridToWorldLocation(Point, 100.f);
		EnemyBase->SetHomeLocation(RoomCenterLocation);
	}
}

void UDungeonGeneratorComponent::SpawnDecorationAroundPoint(TSubclassOf<AActor> DecorationClass, const FVector2D& Point)
{
	if (!DecorationClass || !GetWorld()) return;

	const int32 CX = FMath::RoundToInt(Point.X);
	const int32 CY = FMath::RoundToInt(Point.Y);

	const int32 SearchRadius = 5;

	TArray<FIntPoint> CandidateTiles;
	
	for (int32 X = CX - SearchRadius; X <= CX + SearchRadius; X++)
	{
		for (int32 Y = CY - SearchRadius; Y <= CY + SearchRadius; Y++)
		{
			if (!IsWalkableTile(X, Y)) continue;

			// 너무 벽에 붙는 걸 피하고 싶으면 가장자리 제외
			if (GetTile(X + 1, Y) == EDungeonTileType::Empty) continue;
			if (GetTile(X - 1, Y) == EDungeonTileType::Empty) continue;
			if (GetTile(X, Y + 1) == EDungeonTileType::Empty) continue;
			if (GetTile(X, Y - 1) == EDungeonTileType::Empty) continue;

			CandidateTiles.Add(FIntPoint(X, Y));
		}
	}

	if (CandidateTiles.Num() == 0)
	{
		return;
	}

	const int32 Index = FMath::RandRange(0, CandidateTiles.Num() - 1);
	const FIntPoint Tile = CandidateTiles[Index];

	FVector Location = GridToWorldLocation(FVector2D(Tile.X, Tile.Y), DecorationHeight);

	const float InnerRandom = TileSize * 0.35f;
	Location.X += FMath::RandRange(-InnerRandom, InnerRandom);
	Location.Y += FMath::RandRange(-InnerRandom, InnerRandom);

	const FRotator Rotation(0.f, FMath::RandRange(0.f, 360.f), 0.f);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Spawned = GetWorld()->SpawnActor<AActor>(DecorationClass, Location, Rotation, SpawnParams);
	if (Spawned)
	{
		SpawnedDungeonActors.Add(Spawned);
	}
}

void UDungeonGeneratorComponent::SpawnStoreRoomDecorations(const FDungeonRoomInfo& Room,
	const FDungeonDecorationDataRow* DecorationDataRow)
{
	if (!DecorationDataRow) return;

	const FIntPoint StoreForwardDir = FindStoreForwardDirection(Room);
	const FVector2D StoreBackOffset = ConvertLocalStoreOffsetToWorldOffset(FVector2D(0.f, -2.f),StoreForwardDir);
	const FRotator FurnitureRotationOffset(0.f, -90.f, 0.f);
	
	FDungeonRoomInfo StoreLayoutRoom = Room;
	StoreLayoutRoom.Center = Room.Center + StoreBackOffset;

#pragma region NPC Direction
	const FIntPoint BackDir(-StoreForwardDir.X, -StoreForwardDir.Y);
	const FIntPoint RightDir(StoreForwardDir.Y, -StoreForwardDir.X);
	const FIntPoint LeftDir(-RightDir.X, -RightDir.Y);

	TArray<FVector2D> BackOffsets;
	BackOffsets.Add(ConvertLocalStoreOffsetToWorldOffset(FVector2D(-2.f, -1.f), StoreForwardDir));
	BackOffsets.Add(ConvertLocalStoreOffsetToWorldOffset(FVector2D(0.f, -1.f), StoreForwardDir));
	BackOffsets.Add(ConvertLocalStoreOffsetToWorldOffset(FVector2D(2.f, -1.f), StoreForwardDir));

	TArray<FVector2D> FrontOffsets;
	FrontOffsets.Add(ConvertLocalStoreOffsetToWorldOffset(FVector2D(0.f, 0.75f), StoreForwardDir));
	FrontOffsets.Add(ConvertLocalStoreOffsetToWorldOffset(FVector2D(-1.f, 0.75f), StoreForwardDir));
	FrontOffsets.Add(ConvertLocalStoreOffsetToWorldOffset(FVector2D(1.f, 0.75f), StoreForwardDir));

	TArray<FVector2D> LeftOffsets;
	LeftOffsets.Add(ConvertLocalStoreOffsetToWorldOffset(FVector2D(-2.f, 0.f), StoreForwardDir));
	LeftOffsets.Add(ConvertLocalStoreOffsetToWorldOffset(FVector2D(-2.f, 1.f), StoreForwardDir));

	TArray<FVector2D> RightOffsets;
	RightOffsets.Add(ConvertLocalStoreOffsetToWorldOffset(FVector2D(2.f, 0.f), StoreForwardDir));
	RightOffsets.Add(ConvertLocalStoreOffsetToWorldOffset(FVector2D(2.f, 1.f), StoreForwardDir));

#pragma endregion
	int32 SpawnedCount = 0;
	SpawnedCount += SpawnStoreDecorationGroup(DecorationDataRow->StoreBackDecorations, BackOffsets, StoreLayoutRoom, GridDirectionToActorRotation(StoreForwardDir) + FurnitureRotationOffset);
	SpawnedCount += SpawnStoreDecorationGroup(DecorationDataRow->StoreFrontDecorations, FrontOffsets, StoreLayoutRoom, GridDirectionToActorRotation(BackDir) + FurnitureRotationOffset);
	SpawnedCount += SpawnStoreDecorationGroup(DecorationDataRow->StoreLeftDecorations, LeftOffsets, StoreLayoutRoom, GridDirectionToActorRotation(RightDir) + FurnitureRotationOffset);
	SpawnedCount += SpawnStoreDecorationGroup(DecorationDataRow->StoreRightDecorations, RightOffsets, StoreLayoutRoom, GridDirectionToActorRotation(LeftDir) + FurnitureRotationOffset);

}

AActor* UDungeonGeneratorComponent::SpawnDecorationAtGridOffset(TSubclassOf<AActor> DecorationClass,
	const FVector2D& RoomCenter, const FVector2D& GridOffset, float Z, const FRotator& Rotation)
{
	if (!DecorationClass) return nullptr;
	const FVector2D TargetGrid = RoomCenter + GridOffset;
	const int32 X = FMath::RoundToInt(TargetGrid.X);
	const int32 Y = FMath::RoundToInt(TargetGrid.Y);

	if (!IsWalkableTile(X, Y)) return nullptr;
	FVector Location = GridToWorldLocation(TargetGrid, Z);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Spawned = GetWorld()->SpawnActor<AActor>(DecorationClass, Location, Rotation, SpawnParams);

	if (!Spawned) return nullptr;

	SpawnedDungeonActors.Add(Spawned);

#if WITH_EDITOR
	Spawned->SetActorLabel(TEXT("StoreDecoration"));
#endif

	return Spawned;
	
}
void UDungeonGeneratorComponent::PlaceCorridorTile(int32 X, int32 Y)
{
	for (int32 OffsetX = -1; OffsetX <= 1; OffsetX++)
	{
		for (int32 OffsetY = -1; OffsetY <= 1; OffsetY++)
		{
			const int32 NX = X + OffsetX;
			const int32 NY = Y + OffsetY;
			const EDungeonTileType Current = GetTile(NX, NY);

			if (Current == EDungeonTileType::Wall) continue;
			if (Current != EDungeonTileType::Room && Current != EDungeonTileType::BossRoom)
			{
				SetTile(NX,NY,EDungeonTileType::Corridor);
			}
		}
	}
	const EDungeonTileType CurrentTile = GetTile(X, Y);

	// Wall이면 못감
	if (CurrentTile == EDungeonTileType::Wall)
	{
		return;
	}
	if (CurrentTile != EDungeonTileType::Room && CurrentTile != EDungeonTileType::BossRoom)
	{
		SetTile(X,Y,EDungeonTileType::Corridor);
	}
}

const FDungeonEnemySpawnDataRow* UDungeonGeneratorComponent::GetEnemySpawnData(EDungeonRoomType RoomType) const
{
	if (!DungeonEnemySpawnDataTable) return  nullptr;
	return DungeonEnemySpawnDataTable->FindRow<FDungeonEnemySpawnDataRow>(GetRoomTypeRowName(RoomType), TEXT("DungeonSpawnData"));
}

const FDungeonDecorationDataRow* UDungeonGeneratorComponent::GetDecorationData(EDungeonDecorationTheme  RoomType) const
{
	if (!DungeonDecorationDataTable) return  nullptr;
	return DungeonDecorationDataTable->FindRow<FDungeonDecorationDataRow>(GetDecorationThemeRowName(RoomType),TEXT("DungeonSpawnData"));
}

EDungeonPieceShape UDungeonGeneratorComponent::GetWallShapeForEdge(int32 X, int32 Y, const FIntPoint& Dir) const
{
	// 핵심:
	// 한 코너를 두 edge에서 동시에 Corner로 만들지 않기 위해
	// Corner가 되는 방향을 고정한다.
	//
	// 북동 코너: 북쪽 edge에서만 Corner
	// 남동 코너: 동쪽 edge에서만 Corner
	// 남서 코너: 남쪽 edge에서만 Corner
	// 북서 코너: 서쪽 edge에서만 Corner
	
	// 북/남 edge 벽이면 좌우가 비었는지 확인
	if (Dir == FIntPoint(0, 1))
	{
		const bool bWestEmpty = GetTile(X - 1, Y) == EDungeonTileType::Empty;
		const bool bEastEmpty = GetTile(X + 1, Y) == EDungeonTileType::Empty;

		if (bWestEmpty || bEastEmpty)
		{
			return EDungeonPieceShape::Corner;
		}
	}

	// 동/서 edge 벽이면 위아래가 비었는지 확인
	if (Dir == FIntPoint(1, 0))
	{
		const bool bNorthEmpty = GetTile(X, Y + 1) == EDungeonTileType::Empty;
		const bool bSouthEmpty = GetTile(X, Y - 1) == EDungeonTileType::Empty;

		if (bNorthEmpty || bSouthEmpty)
		{
			return EDungeonPieceShape::Corner;
		}
	}
	if (Dir == FIntPoint(0, -1))
	{
		const bool bForwardEmpty = GetTile(X, Y - 1) == EDungeonTileType::Empty;
		const bool bWestEmpty = GetTile(X - 1, Y) == EDungeonTileType::Empty;

		// 남서 코너만 여기서 처리
		if (bForwardEmpty && bWestEmpty)
		{
			return EDungeonPieceShape::Corner;
		}
	}
	// 서쪽 edge
	if (Dir == FIntPoint(-1, 0))
	{
		const bool bForwardEmpty = GetTile(X - 1, Y) == EDungeonTileType::Empty;
		const bool bNorthEmpty = GetTile(X, Y + 1) == EDungeonTileType::Empty;

		// 북서 코너만 여기서 처리
		if (bForwardEmpty && bNorthEmpty)
		{
			return EDungeonPieceShape::Corner;
		}
	}

	return EDungeonPieceShape::Straight;
}

FDungeonNeighborInfo UDungeonGeneratorComponent::GetWallNeighborInfoForEdge(int32 X, int32 Y,
	const FIntPoint& Dir) const
{
	FDungeonNeighborInfo Info;

	const EDungeonPieceShape Shape = GetWallShapeForEdge(X, Y, Dir);

	if (Shape == EDungeonPieceShape::Straight)
	{
		if (Dir == FIntPoint(0, 1) || Dir == FIntPoint(0, -1))
		{
			Info.bN = true;
		}
		else if (Dir == FIntPoint(1, 0) || Dir == FIntPoint(-1, 0))
		{
			Info.bE = true;
		}

		return Info;
	}

	// Corner는 위 GetWallShapeForEdge()의 방향 규칙과 동일하게 맞춤

	// 북동 코너
	if (Dir == FIntPoint(0, 1))
	{
		Info.bN = true;
		Info.bE = true;
		return Info;
	}

	// 남동 코너
	if (Dir == FIntPoint(1, 0))
	{
		Info.bE = true;
		Info.bS = true;
		return Info;
	}

	// 남서 코너
	if (Dir == FIntPoint(0, -1))
	{
		Info.bS = true;
		Info.bW = true;
		return Info;
	}

	// 북서 코너
	if (Dir == FIntPoint(-1, 0))
	{
		Info.bW = true;
		Info.bN = true;
		return Info;
	}

	return Info;
}

// 4방향을 보고 회전값 결정 
FRotator UDungeonGeneratorComponent::GetRotationByNeighborInfo(const FDungeonNeighborInfo& Info, EDungeonPieceShape Shape) const
{
	if (Shape == EDungeonPieceShape::Corner)
	{
		if (Info.bE && Info.bS) return FRotator(0.f, 90.f, 0.f);
		if (Info.bS && Info.bW) return FRotator(0.f, 0.f, 0.f);
		if (Info.bW && Info.bN) return FRotator(0.f, 270.f, 0.f);
		if (Info.bN && Info.bE) return FRotator(0.f, 180.f, 0.f);
	}

	if (Info.bN || Info.bS) return FRotator(0.f, 0.f, 0.f);
	if (Info.bE || Info.bW) return FRotator(0.f, 90.f, 0.f);

	return FRotator::ZeroRotator;
}

FRotator UDungeonGeneratorComponent::DirectionToInnerWallRotation(const FIntPoint& Dir) const
{
	const FIntPoint InnerDir(-Dir.X, -Dir.Y);
	return DirectionToRotation(InnerDir);
}

FDungeonNeighborInfo UDungeonGeneratorComponent::GetNeighborInfo(int32 X, int32 Y, TFunctionRef<bool(int32, int32)> Predicate) const
{
	// 4방향의 정보를 확인하고 반환 
	FDungeonNeighborInfo Info;

	Info.bN = Predicate(X, Y + 1);
	Info.bS = Predicate(X, Y - 1);
	Info.bE = Predicate(X + 1, Y);
	Info.bW = Predicate(X - 1, Y);

	return Info;
}

FVector UDungeonGeneratorComponent::GetWallOffset(int32 X, int32 Y) const
{
	const float Offset = TileSize * 0.5f;
	const FDungeonNeighborInfo Info = GetWalkableNeighborInfo(X, Y);
	const EDungeonPieceShape Shape = GetWallShape(X, Y);
	if(Shape==EDungeonPieceShape::Corner)
	{
		FVector Result = FVector::ZeroVector;

		if(Info.bN) Result.Y+=Offset;
		if(Info.bS) Result.Y-=Offset;
		if(Info.bE) Result.X+=Offset;
		if(Info.bW) Result.X-=Offset;

		return Result;
	}
	if(Info.bN) return FVector(0,Offset,0);
	if(Info.bS) return FVector(0,-Offset,0);
	if(Info.bE) return FVector(Offset,0,0);
	if(Info.bW) return FVector(-Offset,0,0);

	return FVector::ZeroVector;
}

// 현재 타일 주변에 걸을 수 있는 타일을 확인 
// 벽 배치 / 회전에 사용 
FDungeonNeighborInfo UDungeonGeneratorComponent::GetWalkableNeighborInfo(int32 X, int32 Y) const
{
	return GetNeighborInfo(X, Y, [this](int32 NX, int32 NY)
	{
		return IsWalkableTile(NX, NY);
	});
}
bool UDungeonGeneratorComponent::IsWalkableTile(int32 X, int32 Y) const
{
	const EDungeonTileType Tile = GetTile(X, Y);
	
	return Tile == EDungeonTileType::Room || Tile == EDungeonTileType::Corridor || Tile == EDungeonTileType::BossRoom;
}

TArray<FDungeonEdge> UDungeonGeneratorComponent::CollectUniqueEdges() const
{
	TArray<FDungeonEdge> Result;
	for (const FDungeonTriangle& Triangle : TriangleList)
	{
		TArray<FDungeonEdge> Edges;
		Edges.Add(FDungeonEdge(Triangle.A, Triangle.B));
		Edges.Add(FDungeonEdge(Triangle.B, Triangle.C));
		Edges.Add(FDungeonEdge(Triangle.C, Triangle.A));
		for (const FDungeonEdge& Edge : Edges)
		{
			bool bExits = false;
			for (const FDungeonEdge& AddedEdge : Result)
			{
				if (AddedEdge.Equal(Edge))
				{
					bExits = true;
					break;
				}
			}
			if (!bExits) Result.Add(Edge);
		}
	}
	
	return Result;
}
FIntPoint UDungeonGeneratorComponent::FindStoreForwardDirection(const FDungeonRoomInfo& Room) const
{
	const int32 RoomSize = GetRoomSizeByRoomType(Room.RoomType);
	const int32 CX = FMath::RoundToInt(Room.Center.X);
	const int32 CY = FMath::RoundToInt(Room.Center.Y);

	int32 NorthWallScore = 0;
	int32 SouthWallScore = 0;
	int32 EastWallScore = 0;
	int32 WestWallScore = 0;
	
	// 방 바로 바깥쪽이 Empty인 쪽을 "벽 방향"으로 판단
	for (int32 Offset = -RoomSize; Offset <= RoomSize; Offset++)
	{
		if (GetTile(CX + Offset, CY + RoomSize + 1) == EDungeonTileType::Empty)
		{
			NorthWallScore++;
		}

		if (GetTile(CX + Offset, CY - RoomSize - 1) == EDungeonTileType::Empty)
		{
			SouthWallScore++;
		}

		if (GetTile(CX + RoomSize + 1, CY + Offset) == EDungeonTileType::Empty)
		{
			EastWallScore++;
		}

		if (GetTile(CX - RoomSize - 1, CY + Offset) == EDungeonTileType::Empty)
		{
			WestWallScore++;
		}
	}
	
	// 가장 벽다운 방향 선택
	int32 BestScore = NorthWallScore;
	FIntPoint BackWallDir(0, 1);

	if (SouthWallScore > BestScore)
	{
		BestScore = SouthWallScore;
		BackWallDir = FIntPoint(0, -1);
	}

	if (EastWallScore > BestScore)
	{
		BestScore = EastWallScore;
		BackWallDir = FIntPoint(1, 0);
	}

	if (WestWallScore > BestScore)
	{
		BestScore = WestWallScore;
		BackWallDir = FIntPoint(-1, 0);
	}
	
	// NPC는 벽을 등지고 서야 하니까 벽 반대 방향을 바라봄
	const FIntPoint ForwardDir(-BackWallDir.X, -BackWallDir.Y);

	return ForwardDir;
}

FRotator UDungeonGeneratorComponent::GridDirectionToActorRotation(const FIntPoint& Dir) const
{
	const FVector Direction(static_cast<float>(Dir.X), static_cast<float>(Dir.Y), 0.f);

	if (Direction.IsNearlyZero())
	{
		return FRotator::ZeroRotator;
	}

	return Direction.Rotation();
}

FVector2D UDungeonGeneratorComponent::ConvertLocalStoreOffsetToWorldOffset(const FVector2D& LocalOffset,
	const FIntPoint& ForwardDir) const
{
	const FVector2D Forward(static_cast<float>(ForwardDir.X), static_cast<float>(ForwardDir.Y));

	// Forward 기준 오른쪽 방향
	const FVector2D Right(-Forward.Y, Forward.X);

	return Right * LocalOffset.X + Forward * LocalOffset.Y;
}

bool UDungeonGeneratorComponent::IsSameTriangle(const FDungeonTriangle& A, const FDungeonTriangle& B) const
{
	return A.A.Equals(B.A) && A.B.Equals(B.B) && A.C.Equals(B.C);
}

bool UDungeonGeneratorComponent::IsSuperTrianglePoint(const FVector2D& Point) const
{
	const float Margin = AreaSize * 0.5f;
	return Point.X < -Margin || Point.X > AreaSize + Margin || Point.Y < -Margin || Point.Y > AreaSize + Margin;
}

bool UDungeonGeneratorComponent::IsEdgeInTriangle(const FDungeonTriangle& Triangle, const FDungeonEdge Edge) const
{
	TArray<FDungeonEdge> Edges;
	Edges.Add(FDungeonEdge(Triangle.A, Triangle.B));
	Edges.Add(FDungeonEdge(Triangle.B, Triangle.C));
	Edges.Add(FDungeonEdge(Triangle.C, Triangle.A));
	for (const FDungeonEdge& CurrentEdge : Edges)
	{
		if (CurrentEdge.Equal(Edge)) return true;
	}
	return false;
}

// 목적 : 어느 방에서 가장 멀리 있는 방 탐색
FVector2D UDungeonGeneratorComponent::FindFarthestPointByBFS(const FVector2D& Start,
                                                             const TMap<FVector2D, TArray<FVector2D>>& Graph) const
{
	// 다음에 탐색할 방 목록
	TQueue<FVector2D> Queue;
	// 탐색했던 방 목록
	TSet<FVector2D> Visited;
	// 각 방까지의 거리 저장
	TMap<FVector2D, int32> Distance;
	
	// 가장 먼저 Start 포인트에서 탐색 시작
	Queue.Enqueue(Start);
	// 탐색한 방에 추가
	Visited.Add(Start);
	// Start니까 0 저장
	Distance.Add(Start, 0);
	
	// 자기 자신이 가장 먼방으로 저장
	FVector2D Farthest = Start;
	int32 MaxDistance = 0;
	// 큐가 빌때까지 반복
	while (!Queue.IsEmpty())
	{
		// 현재 위치
		FVector2D Current;
		// 큐에서 하나 꺼내서 현재 위치로 저장
		Queue.Dequeue(Current);
		
		// 현재 위치까지의 거리 확인
		const int32 CurrentDistance  = Distance[Current];
		// 만약 현재 위치보다 멀면 갱신
		if (CurrentDistance > MaxDistance)
		{
			MaxDistance = CurrentDistance;
			Farthest = Current;
		}
		
		// 이웃된 방 목록 찾기 
		const TArray<FVector2D>* Neighbors = Graph.Find(Current);
		if (!Neighbors) continue;
		// 이웃방 탐색
		for (const FVector2D& Next : *Neighbors)
		{
			// 방문한 위치면 스킵
			if (Visited.Contains(Next)) continue;
			// 새방이면 방문 처리, 거리 측정, 큐 추가
			Visited.Add(Next);
			Distance.Add(Next, CurrentDistance + 1);
			Queue.Enqueue(Next);
		}
	}
	return Farthest;
}

// 목적 : Start -> End까지 가는 방 경로 배열 만들기 위함
TArray<FVector2D> UDungeonGeneratorComponent::FindPathBFS(const FVector2D& Start, const FVector2D& End,
	const TMap<FVector2D, TArray<FVector2D>>& Graph) const
{
	// 최정 경로
	TArray<FVector2D> Path;
	
	TQueue<FVector2D> Queue;
	TSet<FVector2D> Visited;
	// 내 앞에 방이 누구인지 확인하기 위함 
	TMap<FVector2D, FVector2D> Parent;

	// 시작 방 세팅 
	Queue.Enqueue(Start);
	Visited.Add(Start);
	
	while (!Queue.IsEmpty())
	{
		FVector2D Current;
		// 큐에서 하나 꺼내 현재위치로 저장
		Queue.Dequeue(Current);
		// 마지막 방이면 탐색 중단
		if (Current.Equals(End)) break;
		// 그래프 상에서 내 이웃된 방 목록 확인
		const TArray<FVector2D>* Neighbors = Graph.Find(Current);
		if (!Neighbors) continue;
		
		for (const FVector2D& Next : *Neighbors)
		{
			// 방문했으면 패스 
			if (Visited.Contains(Next))
			{
				continue;
			}
			// 새방이면 추가하고, 다음 방보다 앞에 있던 방이 현재 위치임을 저장 
			Visited.Add(Next);
			Parent.Add(Next, Current);
			Queue.Enqueue(Next);
		}
	}
	// End에 도달 못하면 빈경로 반환 
	if (!Visited.Contains(End)) return Path;
		
	// End에서부터 경로 복원
	FVector2D Current = End;
	// 최종 경로에 끝방을 넣음 
	Path.Insert(Current, 0);
	while (!Current.Equals(Start))
	{
		// 현재 방의 부모를 찾아 앞으로 이동 
		const FVector2D* Prev = Parent.Find(Current);
		if (!Prev)
		{
			Path.Empty();
			return Path;
		}
		// 현재 위치를 부모로 변경하고
		Current = *Prev;
		// 최종 경로에 현재 위치 추가
		Path.Insert(Current, 0);
	}
	return Path;
}

void UDungeonGeneratorComponent::BuildMSTGraph(TMap<FVector2D, TArray<FVector2D>>& OutGraph) const
{
	OutGraph.Empty();
	for (const FDungeonEdge& Edge : MstEdges)
	{
		OutGraph.FindOrAdd(Edge.U).Add(Edge.V);
		OutGraph.FindOrAdd(Edge.V).Add(Edge.U);
	}
}

void UDungeonGeneratorComponent::BuildNavMesh()
{
	UWorld* World = GetWorld();
	if (!World)return;
	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
	{
		const FVector Center = FVector::ZeroVector;
		const FVector Extent(MapWidth  * TileSize * 0.5f, MapHeight * TileSize * 0.5f, 500.f);
		const FBox DirtyBox(Center - Extent, Center + Extent);
		
		NavSys->AddDirtyArea(DirtyBox, ENavigationDirtyFlag::All);
		NavSys->Build();
	}
}

ABossRoomEntrance* UDungeonGeneratorComponent::SpawnBossRoomEntrance()
{
	if (!BossRoomEntranceClass || !GetWorld()) return nullptr;
	if (!bHasBossRoomEntranceLocation) return nullptr;

	FVector Location = GridToWorldLocation(BossRoomEntranceGridLocation, 0.f);
	FRotator Rotation = DirectionToRotation(BossRoomEntranceDirection);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABossRoomEntrance* Entrance = GetWorld()->SpawnActor<ABossRoomEntrance>(
		BossRoomEntranceClass,
		Location,
		Rotation,
		SpawnParams
	);

	if (!Entrance) return nullptr;

	SpawnedDungeonActors.Add(Entrance);
#if WITH_EDITOR
				Entrance->SetActorLabel(TEXT("BossRoomEntrance"));
#endif

	return Entrance;
}

// 맵 데이터에서 입구를 한칸만 남기고 나머지는 막아줌
void UDungeonGeneratorComponent::NormalizeBossRoomEntrance()
{
	const FDungeonRoomInfo* BossRoom = Rooms.FindByPredicate([](const FDungeonRoomInfo& Room)
	{
		return Room.RoomType == EDungeonRoomType::Boss;
	});

	if (!BossRoom) return;

	NormalizeRoomEntrance(*BossRoom, EDungeonTileType::BossRoom, 3, true);
}

bool UDungeonGeneratorComponent::NormalizeRoomEntrance(const FDungeonRoomInfo& Room, EDungeonTileType RoomTileType,
	int32 DesiredKeepCount, bool bSaveAsBossEntrance)
{
	struct FRoomEntranceContact
	{
		int32 RoomX = 0;
		int32 RoomY = 0;

		int32 CorridorX = 0;
		int32 CorridorY = 0;

		FIntPoint Dir = FIntPoint::ZeroValue;
	};

	TArray<FRoomEntranceContact> Contacts;
	
	const TArray<FIntPoint> Directions = { FIntPoint(0, 1), FIntPoint(0, -1), FIntPoint(1, 0), FIntPoint(-1, 0)};

	const int32 RoomSize = GetRoomSizeByRoomType(Room.RoomType);
	const int32 CX = FMath::RoundToInt(Room.Center.X);
	const int32 CY = FMath::RoundToInt(Room.Center.Y);
	for (int32 X = CX - RoomSize; X <= CX + RoomSize; X++)
	{
		for (int32 Y = CY - RoomSize; Y <= CY + RoomSize; Y++)
		{
			if (!IsInMap(X, Y)) continue;
			if (GetTile(X, Y) != RoomTileType) continue;

			for (const FIntPoint& Dir : Directions)
			{
				const int32 NX = X + Dir.X;
				const int32 NY = Y + Dir.Y;

				if (GetTile(NX, NY) != EDungeonTileType::Corridor) continue;

				FRoomEntranceContact Contact;
				Contact.RoomX = X;
				Contact.RoomY = Y;
				Contact.CorridorX = NX;
				Contact.CorridorY = NY;
				Contact.Dir = Dir;

				Contacts.Add(Contact);
			}
		}
	}
	if (Contacts.Num() == 0) return false;

	TArray<FRoomEntranceContact> BestLine;
	
	for (const FIntPoint& Dir : Directions)
	{
		TArray<FRoomEntranceContact> SameDirContacts;
		for (const FRoomEntranceContact& Contact : Contacts)
		{
			if (Contact.Dir == Dir) SameDirContacts.Add(Contact);
		}
		if (SameDirContacts.Num() == 0) continue;
		SameDirContacts.Sort([Dir](const FRoomEntranceContact& A, const FRoomEntranceContact& B)
		{
			if (Dir.X != 0)
			{
				if (A.RoomX != B.RoomX) return A.RoomX < B.RoomX;
				return A.RoomY < B.RoomY;
			}

			if (A.RoomY != B.RoomY) return A.RoomY < B.RoomY;
			return A.RoomX < B.RoomX;
		});
		TArray<FRoomEntranceContact> CurrentLine;
		CurrentLine.Add(SameDirContacts[0]);
		for (int32 i = 1; i < SameDirContacts.Num() ; i++)
		{
			const FRoomEntranceContact& Prev = SameDirContacts[i - 1];
			const FRoomEntranceContact& Current = SameDirContacts[i];
			
			const bool bSameLine = Dir.X != 0 ? Prev.RoomX == Current.RoomX : Prev.RoomY == Current.RoomY;
			const bool bContinuous = Dir.X != 0 ? FMath::Abs(Prev.RoomY - Current.RoomY) == 1 : FMath::Abs(Prev.RoomX - Current.RoomX) == 1;
			if (bSameLine && bContinuous) CurrentLine.Add(Current);
			else
			{
				if (CurrentLine.Num() > BestLine.Num())
				{
					BestLine = CurrentLine;
				}

				CurrentLine.Empty();
				CurrentLine.Add(Current);
			}
		}
		if (CurrentLine.Num() > BestLine.Num())
		{
			BestLine = CurrentLine;
		}
	}	
	if (BestLine.Num() == 0) return false;
	
	const int32 ActualKeepCount = FMath::Min(DesiredKeepCount, BestLine.Num());
	const int32 StartKeepIndex = FMath::Max(0, (BestLine.Num() - ActualKeepCount) / 2);
	const int32 EndKeepIndex = StartKeepIndex + ActualKeepCount - 1;

	if (bSaveAsBossEntrance)
	{
		float SumRoomX = 0.f;
		float SumRoomY = 0.f;

		for (int32 i = StartKeepIndex; i <= EndKeepIndex; i++)
		{
			SumRoomX += BestLine[i].RoomX;
			SumRoomY += BestLine[i].RoomY;
		}

		const float CenterRoomX = SumRoomX / ActualKeepCount;
		const float CenterRoomY = SumRoomY / ActualKeepCount;

		const FIntPoint EntranceDir = BestLine[StartKeepIndex].Dir;

		BossRoomEntranceGridLocation = FVector2D(CenterRoomX, CenterRoomY);
		BossRoomEntranceGridLocation.X += EntranceDir.X * 0.5f;
		BossRoomEntranceGridLocation.Y += EntranceDir.Y * 0.5f;

		BossRoomEntranceDirection = EntranceDir;
		bHasBossRoomEntranceLocation = true;
	}
	for (const FRoomEntranceContact& Contact : Contacts)
	{
		bool bKeep = false;

		for (int32 i = StartKeepIndex; i <= EndKeepIndex; i++)
		{
			const FRoomEntranceContact& KeepContact = BestLine[i];

			const bool bSameContact =
				Contact.RoomX == KeepContact.RoomX &&
				Contact.RoomY == KeepContact.RoomY &&
				Contact.CorridorX == KeepContact.CorridorX &&
				Contact.CorridorY == KeepContact.CorridorY;

			if (bSameContact)
			{
				bKeep = true;
				break;
			}
		}

		if (bKeep) continue;

		if (Room.RoomType == EDungeonRoomType::Store)
		{
			TryCloseCorridorTileSafely(Contact.CorridorX, Contact.CorridorY);
		}
		else
		{
			SetTile(Contact.CorridorX, Contact.CorridorY, EDungeonTileType::Empty);
		}
	}
	return true;
}
bool UDungeonGeneratorComponent::TryCloseCorridorTileSafely(int32 X, int32 Y)
{
	if (GetTile(X, Y) != EDungeonTileType::Corridor) return false;

	SetTile(X, Y, EDungeonTileType::Empty);

	if (CanReachStartToEndByMapData())
	{
		return true;
	}

	SetTile(X, Y, EDungeonTileType::Corridor);
	return false;
}

bool UDungeonGeneratorComponent::CanReachStartToEndByMapData() const
{
	const FIntPoint Start(FMath::RoundToInt(StartPoint.X), FMath::RoundToInt(StartPoint.Y));
	const FIntPoint End(FMath::RoundToInt(EndPoint.X), FMath::RoundToInt(EndPoint.Y));
	
	if (!IsWalkableTile(Start.X, Start.Y)) return false;
	if (!IsWalkableTile(End.X, End.Y)) return false;

	TQueue<FIntPoint> Queue;
	TSet<FIntPoint> Visited;

	Queue.Enqueue(Start);
	Visited.Add(Start);
	
	const TArray<FIntPoint> Directions = { FIntPoint(0, 1), FIntPoint(0, -1), FIntPoint(1, 0), FIntPoint(-1, 0)};

	while (!Queue.IsEmpty())
	{
		FIntPoint Current;
		Queue.Dequeue(Current);
		
		for (const FIntPoint& Dir : Directions)
		{
			const FIntPoint Next(Current.X + Dir.X, Current.Y + Dir.Y);

			if (Visited.Contains(Next)) continue;
			if (!IsWalkableTile(Next.X, Next.Y)) continue;

			Visited.Add(Next);
			Queue.Enqueue(Next);
		}
	}
	return false;
}

FVector UDungeonGeneratorComponent::GridToWorldLocation(const FVector2D& Point, float Z) const
{
	const float CenterX = MapWidth * 0.5f;
	const float CenterY = MapHeight * 0.5f;
	return FVector((Point.X - CenterX) * TileSize, (Point.Y - CenterY) * TileSize, Z);
}

const FDungeonRoomInfo* UDungeonGeneratorComponent::FindRoomInfoByCenter(const FVector2D& Center) const
{
	for (const FDungeonRoomInfo& Room : Rooms)
	{
		if (Room.Center.Equals(Center)) return &Room;
	}

	return nullptr;
}

int32 UDungeonGeneratorComponent::GetRoomSizeByRoomType(EDungeonRoomType RoomType) const
{
	switch (RoomType)
	{
	case EDungeonRoomType::Boss:
		return 5;

	case EDungeonRoomType::Store:
		return 3;

	case EDungeonRoomType::Start:
		return 3;

	case EDungeonRoomType::Elite:
		return FMath::RandRange(3, 4);

	case EDungeonRoomType::Normal:
		return FMath::RandRange(3, 4);

	default:
		return FMath::RandRange(3, 4);
	}
}

int32 UDungeonGeneratorComponent::SpawnStoreDecorationGroup(const TArray<FDungeonDecorationEntry>& Entries,
	const TArray<FVector2D>& Offsets, const FDungeonRoomInfo& Room, const FRotator& Rotation)
{
	if (Entries.Num() == 0 || Offsets.Num() == 0) return 0;

	TArray<FVector2D> AvailableOffsets = Offsets;
	int32 SpawnedCount = 0;
	for (const FDungeonDecorationEntry& Entry : Entries)
	{
		if (!Entry.DecorationClass) continue;
		if (AvailableOffsets.Num() == 0) break;

		const int32 SafeMinCount = FMath::Max(0, Entry.MinCount);
		const int32 SafeMaxCount = FMath::Max(SafeMinCount, Entry.MaxCount);
		const int32 TargetCount = FMath::RandRange(SafeMinCount, SafeMaxCount);

		for (int32 i = 0; i < TargetCount; i++)
		{
			if (AvailableOffsets.Num() == 0) break;

			const int32 OffsetIndex = FMath::RandRange(0, AvailableOffsets.Num() - 1);
			const FVector2D Offset = AvailableOffsets[OffsetIndex];
			AvailableOffsets.RemoveAt(OffsetIndex);

			AActor* Spawned = SpawnDecorationAtGridOffset(Entry.DecorationClass, Room.Center, Offset, DecorationHeight, Rotation);

			if (Spawned)
			{
				SpawnedCount++;
			}
		}
	}

	return SpawnedCount;
}

const FDungeonDecorationEntry* UDungeonGeneratorComponent::PickRandomDecorationEntry(
	const TArray<FDungeonDecorationEntry>& Entries)
{
	float TotalWeight = 0.f;
	for (const FDungeonDecorationEntry& Entry : Entries)
	{
		if (!Entry.DecorationClass) continue;
		TotalWeight += FMath::Max(0.f, Entry.SpawnWeight);
	}
	
	if (TotalWeight <= 0.f) return nullptr;

	float RandomValue = FMath::FRandRange(0.f, TotalWeight);

	for (const FDungeonDecorationEntry& Entry : Entries)
	{
		if (!Entry.DecorationClass) continue;

		RandomValue -= FMath::Max(0.f, Entry.SpawnWeight);

		if (RandomValue <= 0.f)
		{
			return &Entry;
		}
	}

	return nullptr;
}
