// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillNodeWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void USkillNodeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_Skill)
	{
		BTN_Skill->OnClicked.RemoveDynamic(this, &USkillNodeWidget::OnSkillClicked);
		BTN_Skill->OnClicked.AddDynamic(this, &USkillNodeWidget::OnSkillClicked);
	}
}
void USkillNodeWidget::InitializeSkillNode(FName InSkillRowName, const FSkillDataRow& InSkillName)
{
	SkillRowName = InSkillRowName;

	if (TB_SkillName)
	{
		TB_SkillName->SetText(InSkillName.SkillName);
	}
	if (TB_Gold) TB_Gold->SetText(FText::AsNumber(InSkillName.GoldCost));
}

void USkillNodeWidget::RefreshSkillNode(bool bIsUnlocked, bool bInCanUnlock)
{
	bUnlocked = bIsUnlocked;
	bCanUnlock = bInCanUnlock;

	if (BTN_Skill)
	{
		BTN_Skill->SetIsEnabled(!bUnlocked && bCanUnlock);
	}
	if (bUnlocked)
	{
		// 해금 완료
		BTN_Skill->SetBackgroundColor(
			FLinearColor(0.2f, 0.8f, 0.3f, 1.f)
		);
	}
	else if (bCanUnlock)
	{
		// 현재 해금 가능
		BTN_Skill->SetBackgroundColor(
			FLinearColor(0.9f, 0.65f, 0.15f, 1.f)
		);
	}
	else
	{
		// 잠김
		BTN_Skill->SetBackgroundColor(
			FLinearColor(0.25f, 0.25f, 0.25f, 1.f)
		);
	}
}

void USkillNodeWidget::OnSkillClicked()
{
	if (bUnlocked) return;
	if (SkillRowName.IsNone()) return;

	OnSkillNodeClicked.Broadcast(SkillRowName);
}

FName USkillNodeWidget::GetSkillRowName() const
{
	return SkillRowName;
}
