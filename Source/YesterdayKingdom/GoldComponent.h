// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoldComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, int32, NewGold);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API UGoldComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gold")
	int32 StartGold = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gold")
	int32 CurrentGold = 0.f;
public:	
	// Sets default values for this component's properties
	UGoldComponent();

private:
	void SetGold(int32 NewGold);

public:	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintAssignable)
	FOnGoldChanged OnGoldChanged;

	UFUNCTION(BlueprintCallable)
	void AddGold(int32 Amount);

	UFUNCTION(BlueprintCallable)
	bool SpendGold(int32 Amount);

	UFUNCTION(BlueprintCallable)
	bool CanAfford(int32 Amount) const;

	UFUNCTION(BlueprintCallable)
	int32 GetGold() const;
};
