// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractTargetChanged, AActor*, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionTextChanged, const FText&, InteractionText);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API UPlayerInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerInteractionComponent();

protected:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<AActor> CurrentInteractTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionRadius = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionDotThreshold = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> InteractionTraceChannel = ECC_WorldDynamic;
public:
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractTargetChanged OnInteractTargetChanged;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionTextChanged OnInteractionTextChanged;
protected:
	AActor* FindBestInteractTarget() const;

	void SetCurrentInteractTarget(AActor* NewTarget);
public:	
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void UpdateInteractTarget();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetCurrentInteractTarget() const;
};
