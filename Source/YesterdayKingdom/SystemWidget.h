// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SystemWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSystemReturnRequested);
UCLASS()
class YESTERDAYKINGDOM_API USystemWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> BTN_Return;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> BTN_Quit;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> BTN_Main;
public:
	UPROPERTY(BlueprintAssignable)
	FOnSystemReturnRequested OnSystemReturnRequested;
protected:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnClickReturn();
	UFUNCTION()
	void OnClickQuit();
	UFUNCTION()
	void OnClickMain();


};
