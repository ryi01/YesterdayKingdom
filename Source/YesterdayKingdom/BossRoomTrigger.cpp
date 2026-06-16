// Fill out your copyright notice in the Description page of Project Settings.


#include "BossRoomTrigger.h"

#include "BossRoomEntrance.h"
#include "EnemyBase.h"
#include "GameSoundManager.h"
#include "PlayerCharacter.h"
#include "PlayerHUDWidget.h"
#include "Components/BoxComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Kismet/GameplayStatics.h"
#include "Components/LightComponent.h"

// Sets default values
ABossRoomTrigger::ABossRoomTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Called when the game starts or when spawned
void ABossRoomTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABossRoomTrigger::OnTriggerBeginOverlap);
	}
}
void ABossRoomTrigger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Boss)
	{
		Boss->OnEnemyDied.RemoveDynamic(this, &ABossRoomTrigger::OnBossDied);
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ClearUITimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}
void ABossRoomTrigger::SetTriggerExtent(const FVector& NewExtent)
{
	if (!TriggerBox) return;

	TriggerBox->SetBoxExtent(NewExtent);
}

void ABossRoomTrigger::SetBossRoomEntrance(ABossRoomEntrance* EntranceActor)
{
	BossRoomEntrance = EntranceActor;
}

void ABossRoomTrigger::SetBoss(AEnemyBase* InBoss)
{
	if (Boss)
	{
		Boss->OnEnemyDied.RemoveDynamic(this, &ABossRoomTrigger::OnBossDied);
	}

	Boss = InBoss;

	if (Boss)
	{
		Boss->OnEnemyDied.AddDynamic(this, &ABossRoomTrigger::OnBossDied);

		UE_LOG(LogTemp, Warning, TEXT("[BossRoomTrigger] Boss Registered : %s"), *Boss->GetName());
	}
}

void ABossRoomTrigger::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bAlreadyTriggered) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player) return;

	bAlreadyTriggered = true;

	Player->ShowBossHP(Boss);
	
	if (BossRoomEntrance)
	{
		BossRoomEntrance->RequestCloseEntrance();
	}
	FindSoundManager();

	if (SoundManager)
	{
		SoundManager->PlayBossBGM();
	}
	UE_LOG(LogTemp, Warning, TEXT("[BossRoomTrigger] Player Entered Boss Room"));
}

void ABossRoomTrigger::OnBossDied()
{
	FindSoundManager();

	if (SoundManager)
	{
		SoundManager->PlayClearBrightBGM();
	}

	GetWorld()->GetTimerManager().ClearTimer(ClearUITimerHandle);
	GetWorld()->GetTimerManager().SetTimer(ClearUITimerHandle,this,&ABossRoomTrigger::ShowClearUI, ClearUIDelay,false);
}

void ABossRoomTrigger::ShowClearUI()
{
	if (!ClearWidgetClass) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	UUserWidget* ClearWidget = CreateWidget<UUserWidget>(PC, ClearWidgetClass);
	if (ClearWidget)
	{
		ClearWidget->AddToViewport(100);
	}

	PC->bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(InputMode);

}

void ABossRoomTrigger::FindSoundManager()
{
	if (SoundManager) return;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, AGameSoundManager::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		SoundManager = Cast<AGameSoundManager>(FoundActors[0]);
	}
}

