// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API UEnemyHPWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UProgressBar> EnemyHpBar;
	
public:
	UFUNCTION()
	void SetEnemyHP(float CurrentHP, float MaxHP);
};
