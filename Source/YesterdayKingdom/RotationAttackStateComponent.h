// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackActionStateComponent.h"
#include "RotationAttackStateComponent.generated.h"

UCLASS()
class YESTERDAYKINGDOM_API URotationAttackStateComponent : public UAttackActionStateComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|RotationAttack")
	bool bFacePlayerOnEnter = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|RotationAttack")
	bool bStopMovementOnEnter = true;

	// Start Notify 이후 이동하는 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|RotationAttack")
	float RotationMoveDuration = 0.6f;

	// MoveTo 도착 판정 반경
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|RotationAttack")
	float RotationMoveAcceptanceRadius = 80.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|RotationAttack")
	bool bTrackPlayerDuringRotation = false;

	float RotationMoveElapsedTime = 0.f;
	float CachedMaxWalkSpeed = 0.f;

	bool bStartedRotationMove = false;
	bool bStartedRotationAttack = false;

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate(float DeltaTime) override;
	virtual void OnStateExit() override;
	
	virtual void NotifyAttackActionStart() override;
};
