// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeWidget.h"

#include "GoldComponent.h"
#include "PlayerSkillComponent.h"
#include "PlayerCharacter.h"
#include "SkillNodeWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void USkillTreeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (!PlayerCharacter) return;

	SkillComponent = PlayerCharacter->GetSkillComponent();
	if (!SkillComponent) return;
	
	SkillComponent->OnSkillTreeChanged.RemoveDynamic(this, &USkillTreeWidget::OnSkillTreeChanged);
	SkillComponent->OnSkillTreeChanged.AddDynamic(this,&USkillTreeWidget::OnSkillTreeChanged);
	
	GoldComponent = PlayerCharacter->GetGoldComponent();
	if (!GoldComponent) return;
	GoldComponent->OnGoldChanged.RemoveDynamic(this, &USkillTreeWidget::OnGoldChanged);
	GoldComponent->OnGoldChanged.AddDynamic(this, &USkillTreeWidget::OnGoldChanged);

	InitializeSkillTree();
	RefreshSkillTree();
	RefreshGold();
}

void USkillTreeWidget::NativeDestruct()
{

	if (SkillComponent)
	{
		SkillComponent->OnSkillTreeChanged.RemoveDynamic(this, &USkillTreeWidget::OnSkillTreeChanged);
	}
	if (GoldComponent)
	{
		GoldComponent->OnGoldChanged.RemoveDynamic(this, &USkillTreeWidget::OnGoldChanged);
	}

	Super::NativeDestruct();
}


void USkillTreeWidget::InitializeSkillTree()
{
	// ========================================================
	// 공격 계열
	// ========================================================
	InitializeSkillNode(NODE_Attack_01,TEXT("SK_Attack_01"));
	InitializeSkillNode(NODE_Attack_02,TEXT("SK_Attack_02"));
	
	// ========================================================
	// 차지 계열
	// ========================================================
	InitializeSkillNode(NODE_Charge_01,TEXT("SK_Charge_01"));
	InitializeSkillNode(NODE_Charge_02,TEXT("SK_Charge_02"));
	InitializeSkillNode(NODE_Charge_03,TEXT("SK_Charge_03"));
	
	// ========================================================
	// 방어 계열
	// ========================================================
	InitializeSkillNode(NODE_Defense_01,TEXT("SK_Defense_01"));
	InitializeSkillNode(NODE_Defense_02,TEXT("SK_Defense_02"));
	
	// ========================================================
	// 체력 계열
	// ========================================================
	InitializeSkillNode(NODE_HP_01,TEXT("SK_HP_01"));
	InitializeSkillNode(NODE_HP_02,TEXT("SK_HP_02"));
	InitializeSkillNode(NODE_HP_03,TEXT("SK_HP_03"));
	
	// ========================================================
	// 기동 계열
	// ========================================================
	InitializeSkillNode(NODE_ST_01,TEXT("SK_ST_01"));
	InitializeSkillNode(NODE_Move_01,TEXT("SK_Move_01"));
	InitializeSkillNode(NODE_Move_02,TEXT("SK_Move_02"));
	
	// ========================================================
	// 가드 계열
	// ========================================================
	InitializeSkillNode(NODE_Guard_01,TEXT("SK_Guard_01"));
	InitializeSkillNode(NODE_Guard_02,TEXT("SK_Guard_02"));
	InitializeSkillNode(NODE_Guard_03,TEXT("SK_Guard_03"));
	
	// ========================================================
	// 버프 계열
	// ========================================================
	InitializeSkillNode(NODE_MP_01,TEXT("SK_MP_01"));
	InitializeSkillNode(NODE_Buff_01,TEXT("SK_Buff_01"));
	InitializeSkillNode(NODE_Buff_02,TEXT("SK_Buff_02"));
}

void USkillTreeWidget::InitializeSkillNode(USkillNodeWidget* SkillNodeWidget, FName SkillRowName)
{
	if (!SkillComponent || !SkillNodeWidget) return;
	FSkillDataRow SkillDataRow;
	if (!SkillComponent->GetSkillData(SkillRowName, SkillDataRow))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[SkillTreeWidget] Skill data not found: %s"),
			*SkillRowName.ToString()
		);

		return;
	}
	
	SkillNodeWidget->InitializeSkillNode(SkillRowName, SkillDataRow);
	SkillNodeWidget->OnSkillNodeClicked.RemoveDynamic(this, &USkillTreeWidget::OnSkillNodeClicked);
	SkillNodeWidget->OnSkillNodeClicked.AddDynamic(this, &USkillTreeWidget::OnSkillNodeClicked);
}

void USkillTreeWidget::RefreshSkillTree()
{
	RefreshSkillNode(NODE_Attack_01);
	RefreshSkillNode(NODE_Attack_02);

	RefreshSkillNode(NODE_Charge_01);
	RefreshSkillNode(NODE_Charge_02);
	RefreshSkillNode(NODE_Charge_03);

	RefreshSkillNode(NODE_Defense_01);
	RefreshSkillNode(NODE_Defense_02);

	RefreshSkillNode(NODE_HP_01);
	RefreshSkillNode(NODE_HP_02);
	RefreshSkillNode(NODE_HP_03);

	RefreshSkillNode(NODE_ST_01);
	RefreshSkillNode(NODE_Move_01);
	RefreshSkillNode(NODE_Move_02);

	RefreshSkillNode(NODE_Guard_01);
	RefreshSkillNode(NODE_Guard_02);
	RefreshSkillNode(NODE_Guard_03);

	RefreshSkillNode(NODE_MP_01);
	RefreshSkillNode(NODE_Buff_01);
	RefreshSkillNode(NODE_Buff_02);
}

void USkillTreeWidget::RefreshSkillNode(USkillNodeWidget* SkillNode)
{
	if (!SkillComponent || !SkillNode) return;

	const FName SkillRowName = SkillNode->GetSkillRowName();
	if (SkillRowName.IsNone()) return;
	const bool bIsUnlocked = SkillComponent->IsSkillUnlocked(SkillRowName);
	const bool bCanUnlock  = SkillComponent->CanUnlockSkill(SkillRowName);
	
	SkillNode->RefreshSkillNode(bIsUnlocked, bCanUnlock);
	
}

void USkillTreeWidget::OnSkillNodeClicked(FName SkillRowName)
{
	if (!SkillComponent) return;
	const bool bUnlocked = SkillComponent->TryUnlockSkill(SkillRowName);
	if (bUnlocked)
	{
		if (SkillUnlockSound) UGameplayStatics::PlaySound2D(this, SkillUnlockSound, SkillUnlockSoundVolume, 1.f, 0.f, nullptr, nullptr, true);
	}
	else
	{
		RefreshSkillTree();
	}
}

void USkillTreeWidget::OnSkillTreeChanged()
{
	RefreshSkillTree();
}

void USkillTreeWidget::OnGoldChanged(int32 CurrentGold)
{
	if (TB_Gold)
	{
		TB_Gold->SetText(FText::AsNumber(CurrentGold));
	}

	RefreshSkillTree();
}
void USkillTreeWidget::RefreshGold()
{
	if (!GoldComponent || !TB_Gold) return;

	TB_Gold->SetText(FText::AsNumber(GoldComponent->GetGold()));
}
