// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillTypes.h"
#include "Blueprint/UserWidget.h"
#include "SkillNodeWidget.generated.h"
class UImage;
class UTexture2D;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillNodeClicked, FName, SkillRowName);
UCLASS()
class YESTERDAYKINGDOM_API USkillNodeWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY()
	FName SkillRowName = NAME_None;
	
	UPROPERTY()
	TObjectPtr<UTexture2D> CachedInactiveSkillIcon;
	UPROPERTY()
	TObjectPtr<UTexture2D> CachedActiveSkillIcon;
	
	bool bUnlocked = false;
	bool bCanUnlock = false;
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> BTN_Skill;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TB_SkillName;	
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TB_Gold;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> I_StateImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> I_SkillIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Style")
	TObjectPtr<UTexture2D> CanUnlockFrameTexture;
	
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
