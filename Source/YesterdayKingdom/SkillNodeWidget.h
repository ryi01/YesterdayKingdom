// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillTypes.h"
#include "Blueprint/UserWidget.h"
#include "SkillNodeWidget.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillNodeClicked, FName, SkillRowName);
UCLASS()
class YESTERDAYKINGDOM_API USkillNodeWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY()
	FName SkillRowName = NAME_None;

	bool bUnlocked = false;
	bool bCanUnlock = false;
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> BTN_Skill;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TB_SkillName;	
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TB_Gold;
public:
	UPROPERTY(BlueprintAssignable, Category = "Skill")
	FOnSkillNodeClicked OnSkillNodeClicked;
	
private:
	UFUNCTION()
	void OnSkillClicked();

protected:
	virtual void NativeConstruct() override;

public:
	void InitializeSkillNode(FName InSkillRowName, const FSkillDataRow& InSkillName);
	void RefreshSkillNode(bool bIsUnlocked, bool bInCanUnlock);
	FName GetSkillRowName() const;
	
	
};
