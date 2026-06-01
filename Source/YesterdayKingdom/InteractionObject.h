// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionObject.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionObject : public UInterface
{
	GENERATED_BODY()
};

class YESTERDAYKINGDOM_API IInteractionObject
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 실제 상호작용 실행
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);

	// 현재 상호작용 가능한지 확인
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(AActor* Interactor) const;

	// UI에 표시할 문구
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionText() const;
};
