// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UInventoryWidget> WBP_Inventory;
	
	UPROPERTY()
	TObjectPtr<class APlayerCharacter> OwnerPlayer;

public:
	UFUNCTION(BlueprintCallable)
	void BindPlayer(class APlayerCharacter* InPlayer);

	UFUNCTION(BlueprintCallable)
	void SetInventoryVisible(bool bVisible);
};
