// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillTreeWidget.generated.h"

class UGoldComponent;
/**
 * 
 */
UCLASS()
class YESTERDAYKINGDOM_API USkillTreeWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TObjectPtr<class UPlayerSkillComponent> SkillComponent;

	UPROPERTY()
	TObjectPtr<UGoldComponent> GoldComponent;
protected:
	// ========================================================
	// 공격 계열 - 위쪽 2개
	// ========================================================
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_Attack_01;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_Attack_02;

	// ========================================================
	// 차지 계열 - 아래쪽 3개
	// ========================================================
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_Charge_01;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_Charge_02;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_Charge_03;

	// ========================================================
	// 방어 계열 - 위쪽 2개
	// ========================================================
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_Defense_01;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_Defense_02;

	// ========================================================
	// 체력 계열 - 아래쪽 3개
	// ========================================================
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_HP_01;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_HP_02;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_HP_03;

	// ========================================================
	// 기동 계열
	// ========================================================
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_ST_01;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_Move_01;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_Move_02;

	// ========================================================
	// 가드 계열
	// ========================================================
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_Guard_01;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_Guard_02;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_Guard_03;

	// ========================================================
	// 버프 계열
	// ========================================================
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_MP_01;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_Buff_01;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USkillNodeWidget> NODE_Buff_02;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TB_Gold;
private:
	void InitializeSkillTree();
	void InitializeSkillNode(USkillNodeWidget* SkillNodeWidget, FName SkillRowName);
	void RefreshSkillTree();
	void RefreshGold(); 

	void RefreshSkillNode(USkillNodeWidget* SkillNode);

	UFUNCTION()
	void OnSkillNodeClicked(FName SkillRowName);

	UFUNCTION()
	void OnSkillTreeChanged();
	
	UFUNCTION()
	void OnGoldChanged(int32 CurrentGold);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
