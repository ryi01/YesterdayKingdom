// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillNodeWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USkillNodeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_Skill)
	{
		BTN_Skill->OnClicked.RemoveDynamic(this, &USkillNodeWidget::OnSkillClicked);
		BTN_Skill->OnClicked.AddDynamic(this, &USkillNodeWidget::OnSkillClicked);
		BTN_Skill->SetBackgroundColor(FLinearColor::Transparent);
	}
	if (I_StateImage)
	{
		I_StateImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if (I_SkillIcon)
	{
		I_SkillIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}
void USkillNodeWidget::InitializeSkillNode(FName InSkillRowName, const FSkillDataRow& InSkillName)
{
	SkillRowName = InSkillRowName;
	
	CachedInactiveSkillIcon = InSkillName.InactiveSkillIcon;
	CachedActiveSkillIcon = InSkillName.ActiveSkillIcon;
	
	if (TB_SkillName)
	{
		TB_SkillName->SetText(InSkillName.SkillName);
	}
	if (TB_Gold) TB_Gold->SetText(FText::AsNumber(InSkillName.GoldCost));
	if (I_StateImage)
	{
		if (CanUnlockFrameTexture)
		{
			I_StateImage->SetBrushFromTexture(CanUnlockFrameTexture);
		}

		I_StateImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if (I_SkillIcon)
	{
		if (CachedInactiveSkillIcon)
		{
			I_SkillIcon->SetBrushFromTexture(CachedInactiveSkillIcon);
		}

		I_SkillIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void USkillNodeWidget::RefreshSkillNode(bool bIsUnlocked, bool bInCanUnlock)
{
	bUnlocked = bIsUnlocked;
	bCanUnlock = bInCanUnlock;
	
	const bool bCanClick = !bUnlocked && bCanUnlock;
	const bool bUseActiveIcon = bUnlocked || bCanUnlock;
	
	if (BTN_Skill)
	{
		BTN_Skill->SetIsEnabled(!bUnlocked && bCanUnlock);
	}
	if (I_SkillIcon)
	{
		if (bUseActiveIcon)
		{
			if (CachedActiveSkillIcon)
			{
				I_SkillIcon->SetBrushFromTexture(CachedActiveSkillIcon);
			}
		}
		else
		{
			if (CachedInactiveSkillIcon)
			{
				I_SkillIcon->SetBrushFromTexture(CachedInactiveSkillIcon);
			}
		}

		I_SkillIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		I_SkillIcon->SetColorAndOpacity(FLinearColor::White);
	}
	if (I_StateImage)
	{
		if (bCanClick)
		{
			if (CanUnlockFrameTexture)
			{
				I_StateImage->SetBrushFromTexture(CanUnlockFrameTexture);
			}

			I_StateImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			I_StateImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void USkillNodeWidget::OnSkillClicked()
{
	if (bUnlocked) return;
	if (!bCanUnlock) return;
	if (SkillRowName.IsNone()) return;

	OnSkillNodeClicked.Broadcast(SkillRowName);
}

FName USkillNodeWidget::GetSkillRowName() const
{
	return SkillRowName;
}
