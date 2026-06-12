// Fill out your copyright notice in the Description page of Project Settings.


#include "BossWidget.h"

#include "Components/ProgressBar.h"


void UBossWidget::SetBossHP(float CurrentHP, float MaxHP)
{
	if (!BossHPBar) return;

	const float Percent = MaxHP > 0.f ? CurrentHP / MaxHP : 0.f;
	BossHPBar->SetPercent(Percent);
}
