// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"

#include "BaseStatComponent.h"
#include "PlayerCharacter.h"
#include "InventoryWidget.h"
#include "Components/ProgressBar.h"

void UPlayerHUDWidget::BindPlayer(class APlayerCharacter* InPlayer)
{
	OwnerPlayer = InPlayer;
	if (!OwnerPlayer) return;
	if (UBaseStatComponent* StatComponent = OwnerPlayer->GetStatComponent())
	{
		StatComponent->OnHPChanged.AddDynamic(this, &UPlayerHUDWidget::UpdateHP);
		StatComponent->OnSTChanged.AddDynamic(this, &UPlayerHUDWidget::UpdateST);
		StatComponent->OnMPChanged.AddDynamic(this, &UPlayerHUDWidget::UpdateMP);

		UpdateHP(StatComponent->GetCurrentHP(), StatComponent->GetMaxHP());
		UpdateST(StatComponent->GetCurrentST(), StatComponent->GetMaxST());
		UpdateMP(StatComponent->GetCurrentMP(), StatComponent->GetMaxMP());
	}
	if (WBP_Inventory)
	{
		WBP_Inventory->BindInventory(OwnerPlayer->GetInventoryComponent());
		WBP_Inventory->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UPlayerHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (HP)
	{
		const float CurrentPercent = HP->GetPercent();
		const float NewPercent = FMath::FInterpTo(CurrentPercent, TargetHPPercent, InDeltaTime, BarInterpSpeed);
		HP->SetPercent(NewPercent);
	}

	if (ST)
	{
		const float CurrentPercent = ST->GetPercent();
		const float NewPercent = FMath::FInterpTo(CurrentPercent, TargetSTPercent, InDeltaTime, BarInterpSpeed);
		ST->SetPercent(NewPercent);
	}

	if (MP)
	{
		const float CurrentPercent = MP->GetPercent();
		const float NewPercent = FMath::FInterpTo(CurrentPercent, TargetMPPercent, InDeltaTime, BarInterpSpeed);
		MP->SetPercent(NewPercent);
	}
}


void UPlayerHUDWidget::SetInventoryVisible(bool bVisible)
{
	const ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	FString InventoryOpen = bVisible ? TEXT("OPEN INVENTORY") :  TEXT("CLOSE INVENTORY") ;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *InventoryOpen);
	if (WBP_Inventory) WBP_Inventory->SetVisibility(NewVisibility);
}


void UPlayerHUDWidget::UpdateHP(float CurrentHP, float MaxHP)
{
	TargetHPPercent = MaxHP > 0.f ? CurrentHP / MaxHP : 0.f;
}

void UPlayerHUDWidget::UpdateST(float CurrentST, float MaxST)
{
	TargetSTPercent = MaxST > 0.f ? CurrentST / MaxST : 0.f;
}

void UPlayerHUDWidget::UpdateMP(float CurrentMP, float MaxMP)
{
	TargetMPPercent = MaxMP > 0.f ? CurrentMP / MaxMP : 0.f;
}