// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHPWidget.h"

#include "Components/ProgressBar.h"

void UEnemyHPWidget::SetEnemyHP(float CurrentHP, float MaxHP)
{
	if (!EnemyHpBar) return;

	const float Percent = MaxHP > 0.f ? CurrentHP / MaxHP : 0.f;
	EnemyHpBar->SetPercent(Percent);
}
