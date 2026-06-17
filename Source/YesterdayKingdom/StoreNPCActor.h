// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPCBaseActor.h"
#include "StoreNPCActor.generated.h"

class UStoreComponent;

UCLASS()
class YESTERDAYKINGDOM_API AStoreNPCActor : public ANPCBaseActor
{
	GENERATED_BODY()
public:
	AStoreNPCActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Store")
	TObjectPtr<UStoreComponent> StoreComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<class USoundBase> StoreOpenSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	float StoreOpenSoundVolume = 1.2f;
public:
	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual FText GetInteractionText_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "Store")
	UStoreComponent* GetStoreComponent() const;
};
