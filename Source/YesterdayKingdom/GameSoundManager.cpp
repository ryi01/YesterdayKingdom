// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSoundManager.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGameSoundManager::AGameSoundManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BGMComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMComponent"));
	SetRootComponent(BGMComponent);

	BGMComponent->bAutoActivate = false;
	BGMComponent->bIsUISound = true;
}

// Called when the game starts or when spawned
void AGameSoundManager::BeginPlay()
{
	Super::BeginPlay();
	PlayDungeonBGM();
}

void AGameSoundManager::PlayDungeonBGM()
{
	PlayBGM(DungeonBGM);
}

void AGameSoundManager::PlayBossBGM()
{
	PlayBGM(BossBGM);
}

void AGameSoundManager::PlayBossPhase2BGM()
{
	PlayBGM(Phase2BGM);
}

void AGameSoundManager::PlayClearBrightBGM()
{
	PlayBGM(ClearBrightBGM);
}

void AGameSoundManager::PlayClearDarkBGM()
{
	PlayBGM(ClearDarkBGM);
}

void AGameSoundManager::StopBGM()
{
	if (!BGMComponent) return;
	GetWorld()->GetTimerManager().ClearTimer(BGMTransitionTimerHandle);
	BGMComponent->FadeOut(FadeOutTime, 0.f);
}
void AGameSoundManager::PlayBGM(USoundBase* NewBGM)
{
	if (!BGMComponent || !NewBGM) return;

	if (BGMComponent->Sound == NewBGM && BGMComponent->IsPlaying())
	{
		return;
	}

	PendingBGM = NewBGM;
	if (BGMComponent->IsPlaying())
	{
		BGMComponent->FadeOut(FadeOutTime, 0.f);
		GetWorld()->GetTimerManager().ClearTimer(BGMTransitionTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(BGMTransitionTimerHandle, this, &AGameSoundManager::StartPendingBGM, FadeOutTime, false);
	}
	else
	{
		StartPendingBGM();
	}
}

void AGameSoundManager::PlayUISound(USoundBase* Sound)
{
	PlaySFX2D(Sound);
}

void AGameSoundManager::PlaySFX2D(USoundBase* Sound)
{
	if (!Sound) return;

	UGameplayStatics::PlaySound2D(this, Sound,1.f, 1.f, 0.f, nullptr, nullptr, true);
}

void AGameSoundManager::PlaySFXAtLocation(USoundBase* Sound, FVector Location)
{
	if (!Sound) return;

	UGameplayStatics::PlaySoundAtLocation(this, Sound, Location);
}

void AGameSoundManager::StartPendingBGM()
{
	if (!BGMComponent || !PendingBGM) return;

	BGMComponent->SetSound(PendingBGM);
	BGMComponent->FadeIn(FadeInTime, BGMVolume);

	PendingBGM = nullptr;
}


