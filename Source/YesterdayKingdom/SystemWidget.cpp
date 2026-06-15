// Fill out your copyright notice in the Description page of Project Settings.


#include "SystemWidget.h"

#include "PlayerCharacter.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void USystemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_Main)
	{
		BTN_Main->OnClicked.RemoveDynamic(this, &USystemWidget::OnClickMain);
		BTN_Main->OnClicked.AddDynamic(this, &USystemWidget::OnClickMain);
	}
	if (BTN_Quit)
	{
		BTN_Quit->OnClicked.RemoveDynamic(this, &USystemWidget::OnClickQuit);
		BTN_Quit->OnClicked.AddDynamic(this, &USystemWidget::OnClickQuit);
	}
	if (BTN_Return)
	{
		BTN_Return->OnClicked.RemoveDynamic(this, &USystemWidget::OnClickReturn);
		BTN_Return->OnClicked.AddDynamic(this, &USystemWidget::OnClickReturn);
	}
}

void USystemWidget::OnClickReturn()
{
	OnSystemReturnRequested.Broadcast();
}

void USystemWidget::OnClickQuit()
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		Player->SaveGamePlay();
	}
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);
	}
}

void USystemWidget::OnClickMain()
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		Player->SaveGamePlay();
	}

	UGameplayStatics::OpenLevel(this, FName(TEXT("MainTitleLevel")));
}
