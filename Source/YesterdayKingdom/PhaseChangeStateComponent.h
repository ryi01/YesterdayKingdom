// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSMStateComponent.h"
#include "PhaseChangeStateComponent.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UPhaseChangeStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()
protected:
	// ========================================================
	// Phase Change Wave
	// ========================================================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|PhaseChange")
	float PhaseChangeDuration = 1.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|Wave")
	bool bUsePhaseChangeWavePush = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|Wave")
	float WavePushRadius = 650.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|Wave")
	float WavePushPower = 900.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|Wave")
	float WavePushUpPower = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|Wave")
	bool bDrawWaveDebug = false;
	
	// ========================================================
	// Phase Change Wave FX
	// ========================================================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|Wave FX")
	TObjectPtr<class UNiagaraSystem> WaveFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|Wave FX")
	FVector WaveFXOffset = FVector::ZeroVector;

	// ========================================================
	// Phase Change Wave FX Scale Animation
	// ========================================================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|Wave FX")
	FVector WaveStartScale = FVector(0.2f, 0.2f, 0.2f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|Wave FX")
	FVector WaveEndScale = FVector(3.0f, 3.0f, 3.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|Wave FX")
	float WaveScaleDuration = 0.5f;

	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> ActiveWaveFXComponent;

	FTimerHandle WaveFXScaleTimerHandle;
	FTimerHandle WaveFXDestroyTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|Wave FX")
	float WaveFXDestroyDelay = 0.4f;
	
	float WaveFXElapsedTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|Wave")
	float WavePushDelay = 0.25f;
	// ========================================================
	// Phase Change Wave Sound
	// ========================================================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|Wave Sound")
	TObjectPtr<class USoundBase> WaveSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|Wave Sound")
	float WaveSoundVolume = 1.f;
	// ========================================================
	// Phase Change BGM
	// ========================================================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FSM|PhaseChange|BGM")
	bool bChangeBGMOnPhaseChange = true;


	FTimerHandle WavePushTimerHandle;
	FTimerHandle PhaseChangeTimerHandle;
	
protected:
	void FinishPhaseChange();
	void ApplyPhaseChangeWavePush();
	void StartPhaseChangeWaveFX();
	void UpdatePhaseChangeWaveFXScale();
	void StopPhaseChangeWaveFX();
	void PlayPhaseChangeWaveSound();
	void ChangePhaseBGM();
	void DestroyWaveFXComponent();
	void DeactivateWaveFX();
public:
	virtual void OnStateEnter() override;
	virtual void OnStateExit() override;

};
