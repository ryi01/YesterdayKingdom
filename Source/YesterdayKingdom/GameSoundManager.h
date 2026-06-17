// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameSoundManager.generated.h"

UCLASS()
class YESTERDAYKINGDOM_API AGameSoundManager : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, Category="Sound")
	TObjectPtr<class UAudioComponent> BGMComponent;

	UPROPERTY(EditAnywhere, Category="Sound|BGM")
	TObjectPtr<class USoundBase> DungeonBGM;

	UPROPERTY(EditAnywhere, Category="Sound|BGM")
	TObjectPtr<class USoundBase> BossBGM;
	UPROPERTY(EditAnywhere, Category="Sound|BGM")
	TObjectPtr<class USoundBase> Phase2BGM;

	UPROPERTY(EditAnywhere, Category="Sound|BGM")
	TObjectPtr<class USoundBase> ClearBrightBGM;

	UPROPERTY(EditAnywhere, Category="Sound|BGM")
	TObjectPtr<class USoundBase> ClearDarkBGM;

	UPROPERTY(EditAnywhere, Category="Sound|BGM")
	float BGMVolume = 0.7f;

	UPROPERTY(EditAnywhere, Category="Sound|BGM")
	float FadeInTime = 1.0f;

	UPROPERTY(EditAnywhere, Category="Sound|BGM")
	float FadeOutTime = 1.0f;
	
	UPROPERTY()
	TObjectPtr<class USoundBase> PendingBGM;

	FTimerHandle BGMTransitionTimerHandle;
	
public:	
	// Sets default values for this actor's properties
	AGameSoundManager();
private:
	void PlayBGM(USoundBase* NewBGM);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable)
	void PlayDungeonBGM();

	UFUNCTION(BlueprintCallable)
	void PlayBossBGM();

	UFUNCTION(BlueprintCallable)
	void PlayBossPhase2BGM();
	
	UFUNCTION(BlueprintCallable)
	void PlayClearBrightBGM();

	UFUNCTION(BlueprintCallable)
	void PlayClearDarkBGM();

	UFUNCTION(BlueprintCallable)
	void StopBGM();

	UFUNCTION(BlueprintCallable)
	void PlayUISound(USoundBase* Sound);

	UFUNCTION(BlueprintCallable)
	void PlaySFX2D(USoundBase* Sound);

	UFUNCTION(BlueprintCallable)
	void PlaySFXAtLocation(USoundBase* Sound, FVector Location);
	void StartPendingBGM();

};
