// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TObjectPtr<class UInventoryComponent> InventoryComponent;
	
protected:
	UFUNCTION()
	void RefreshInventory();
public:
	UFUNCTION(BlueprintCallable)
	void BindInventory(class UInventoryComponent* InInventory);
};
