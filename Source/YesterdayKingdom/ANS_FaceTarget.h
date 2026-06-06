// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_FaceTarget.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UANS_FaceTarget : public UAnimNotifyState
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FaceTarget")
	bool bUseAttackDataOption = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FaceTarget", meta=(ClampMin="0.0"))
	float DefaultRotationSpeed = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FaceTarget")
	bool bDebugLog = false;
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
