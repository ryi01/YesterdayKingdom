// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionObject.h"
#include "GameFramework/Character.h"
#include "NPCBaseActor.generated.h"

UCLASS()
class YESTERDAYKINGDOM_API ANPCBaseActor : public ACharacter, public IInteractionObject
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPCBaseActor();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC|Component")
	TObjectPtr<UCapsuleComponent> InteractionCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	FText NPCName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Interaction")
	FText InteractionText = FText::FromString(TEXT("상호작용"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Interaction")
	bool bCanInteract = true;


public:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
//===============================================================================================
// 인터렉션 인터페이스
//===============================================================================================

	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual bool CanInteract_Implementation(AActor* Interactor) const override;

	virtual FText GetInteractionText_Implementation() const override;
//===============================================================================================
// 공격 관련 데이터
//===============================================================================================
	UFUNCTION(BlueprintCallable, Category = "NPC")
	FText GetNPCName() const;
};
