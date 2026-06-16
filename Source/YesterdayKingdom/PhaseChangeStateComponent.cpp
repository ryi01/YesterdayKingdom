// Fill out your copyright notice in the Description page of Project Settings.


#include "PhaseChangeStateComponent.h"

#include "BossEnemy.h"
#include "EnemyBase.h"
#include "GameSoundManager.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UPhaseChangeStateComponent::OnStateEnter()
{
	Super::OnStateEnter();
	if (!OwnerCharacter || !FSMController) return;
	StopMove();
	if (UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->Velocity = FVector::ZeroVector;
	}

	OwnerCharacter->ClearSelectedAttackRowName();
	
	StartPhaseChangeWaveFX();
	PlayPhaseChangeWaveSound();
	ChangePhaseBGM();
	
	GetWorld()->GetTimerManager().SetTimer(WavePushTimerHandle, this, &UPhaseChangeStateComponent::ApplyPhaseChangeWavePush,  WavePushDelay, false);
	
	UE_LOG(LogTemp, Warning,
	TEXT("[FSM][PhaseChange] Enter / Phase=%d / HP=%.2f"),
	OwnerCharacter->GetCurrentPhase(),
	OwnerCharacter->GetCurrentHP()
);

	GetWorld()->GetTimerManager().SetTimer(PhaseChangeTimerHandle, this, &UPhaseChangeStateComponent::FinishPhaseChange, PhaseChangeDuration, false);
}

void UPhaseChangeStateComponent::OnStateExit()
{
	Super::OnStateExit();
	
	GetWorld()->GetTimerManager().ClearTimer(PhaseChangeTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(WavePushTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(WaveFXScaleTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(WaveFXDestroyTimerHandle);
	StopPhaseChangeWaveFX();
}

void UPhaseChangeStateComponent::FinishPhaseChange()
{
	if (!OwnerCharacter) return;

	OwnerCharacter->FinishPhaseChange();
}

void UPhaseChangeStateComponent::ApplyPhaseChangeWavePush()
{
	if (!bUsePhaseChangeWavePush || !OwnerCharacter || !GetWorld()) return;
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!PlayerCharacter) return;
	const float Distance = FVector::Dist2D(OwnerCharacter->GetActorLocation(), PlayerCharacter->GetActorLocation());
	UE_LOG(LogTemp, Warning,
	TEXT("[PhaseWave] Player Distance=%.2f / Radius=%.2f"),
	Distance,
	WavePushRadius
);

	if (bDrawWaveDebug)
	{
		DrawDebugSphere(
			GetWorld(),
			OwnerCharacter->GetActorLocation(),
			WavePushRadius,
			32,
			FColor::Cyan,
			false,
			1.5f
		);
	}

	if (Distance > WavePushRadius)
	{
		return;
	}
	PlayerCharacter->ApplyWaveKnockbackFromLocation(OwnerCharacter->GetActorLocation());
	
}

void UPhaseChangeStateComponent::StartPhaseChangeWaveFX()
{
	if (!WaveFX || !OwnerCharacter || !GetWorld()) return;

	WaveFXElapsedTime = 0.f;
	ActiveWaveFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(OwnerCharacter, WaveFX, OwnerCharacter->GetActorLocation() + WaveFXOffset, FRotator::ZeroRotator, WaveStartScale, true, true);
	if (ActiveWaveFXComponent)
	{
		ActiveWaveFXComponent->SetWorldScale3D(WaveStartScale);
		ActiveWaveFXComponent->Activate(true);
	}

	GetWorld()->GetTimerManager().ClearTimer(WaveFXScaleTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(WaveFXScaleTimerHandle,this,&UPhaseChangeStateComponent::UpdatePhaseChangeWaveFXScale,0.02f,true);
}

void UPhaseChangeStateComponent::UpdatePhaseChangeWaveFXScale()
{
	if (!ActiveWaveFXComponent || !GetWorld())
	{
		return;
	}

	WaveFXElapsedTime += 0.02f;

	const float Alpha = FMath::Clamp(WaveFXElapsedTime / WaveScaleDuration, 0.f, 1.f);
	const float SmoothAlpha = FMath::SmoothStep(0.f, 1.f, Alpha);

	const FVector NewScale = FMath::Lerp(WaveStartScale, WaveEndScale, SmoothAlpha);
	ActiveWaveFXComponent->SetWorldScale3D(NewScale);

	if (Alpha >= 1.f)
	{
		DeactivateWaveFX();
	}
}

void UPhaseChangeStateComponent::StopPhaseChangeWaveFX()
{
	GetWorld()->GetTimerManager().ClearTimer(WaveFXScaleTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(WaveFXDestroyTimerHandle);
	if (ActiveWaveFXComponent)
	{
		ActiveWaveFXComponent->DeactivateImmediate();
		ActiveWaveFXComponent->DestroyComponent();
		ActiveWaveFXComponent = nullptr;
	}
}

void UPhaseChangeStateComponent::PlayPhaseChangeWaveSound()
{
	if (!WaveSound || !OwnerCharacter) return;
	UGameplayStatics::PlaySoundAtLocation(OwnerCharacter, WaveSound, OwnerCharacter->GetActorLocation(), WaveSoundVolume);
}

void UPhaseChangeStateComponent::ChangePhaseBGM()
{
	if (!bChangeBGMOnPhaseChange || !GetWorld()) return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AGameSoundManager::StaticClass(),FoundActors);

	if (FoundActors.Num() <= 0) return;

	AGameSoundManager* SoundManager = Cast<AGameSoundManager>(FoundActors[0]);
	if (!SoundManager) return;

	SoundManager->PlayBossPhase2BGM();
}

void UPhaseChangeStateComponent::DestroyWaveFXComponent()
{
	if (!ActiveWaveFXComponent) return;

	ActiveWaveFXComponent->DestroyComponent();
	ActiveWaveFXComponent = nullptr;
}

void UPhaseChangeStateComponent::DeactivateWaveFX()
{
	if (!GetWorld()) return;

	GetWorld()->GetTimerManager().ClearTimer(WaveFXScaleTimerHandle);

	if (!ActiveWaveFXComponent) return;

	ActiveWaveFXComponent->Deactivate();

	GetWorld()->GetTimerManager().ClearTimer(WaveFXDestroyTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(WaveFXDestroyTimerHandle, this, &UPhaseChangeStateComponent::DestroyWaveFXComponent, WaveFXDestroyDelay, false);
}
