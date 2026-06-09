// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSMStateComponent.h"
#include "DeadStateComponent.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UDeadStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dead")
	float DestroyDelay = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dead")
	float FreezeBeforeDeathMontageEndTime = 0.05f;

	FTimerHandle DestroyTimerHandle;
	FTimerHandle FreezeDeathPoseTimerHandle;

	FOnMontageEnded DeathMontageEndedDelegate;
	
protected:
	void HandleDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void FreezeDeathPose();
	void DestroyOwner();
	
public:
	virtual void OnStateEnter() override;
	virtual void OnStateExit() override;


};
