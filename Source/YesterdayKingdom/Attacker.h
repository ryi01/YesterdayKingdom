// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Attacker.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, NotBlueprintable)
class UAttacker : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class YESTERDAYKINGDOM_API IAttacker
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION( BlueprintCallable, Category = "Attacker" )
	virtual void BeginAttackTrace() = 0;
	UFUNCTION( BlueprintCallable, Category = "Attacker" )
	virtual void DoAttackTrace() = 0;
	UFUNCTION( BlueprintCallable, Category = "Attacker" )
	virtual void EndAttackTrace() = 0;
	UFUNCTION( BlueprintCallable, Category = "Attacker" )
	virtual void CheckCombo() = 0;
};
