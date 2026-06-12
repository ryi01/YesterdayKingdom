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
