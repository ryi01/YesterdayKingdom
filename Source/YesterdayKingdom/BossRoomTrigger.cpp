// Fill out your copyright notice in the Description page of Project Settings.


#include "BossRoomTrigger.h"

#include "BossRoomEntrance.h"
#include "PlayerCharacter.h"
#include "PlayerHUDWidget.h"
#include "Components/BoxComponent.h"

class UPlayerHUDWidget;
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
	Boss = InBoss;
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
	UE_LOG(LogTemp, Warning, TEXT("[BossRoomTrigger] Player Entered Boss Room"));
}


