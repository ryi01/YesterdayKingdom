// Fill out your copyright notice in the Description page of Project Settings.


#include "StartWidget.h"

#include "YesterdayKingdomGameInstance.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UStartWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_OpenMainLevel)
	{
		BTN_OpenMainLevel->OnClicked.RemoveDynamic(this, &UStartWidget::OnCreateClicked);
		BTN_OpenMainLevel->OnClicked.AddDynamic(this, &UStartWidget::OnCreateClicked);
	}
}

void UStartWidget::OnCreateClicked()
{
	UYesterdayKingdomGameInstance* GI = GetGameInstance<UYesterdayKingdomGameInstance>();
	if (!GI || !ETB_Nickname) return;
	const FString Nickname = ETB_Nickname->GetText().ToString().TrimStartAndEnd();
	if (Nickname.IsEmpty())
	{
		if (TB_Popup) TB_Popup->SetText(FText::FromString(TEXT("닉네임을 입력하세요.")));
		return;
	}
	int32 PlayerId = 0;
	if (GI->FindPlayerIdByNickname(Nickname, PlayerId))
	{
		if (TB_Popup)
		{
			TB_Popup->SetText(
				FText::FromString(TEXT("기존 데이터를 불러왔습니다."))
			);
		}
		UE_LOG(
			LogTemp,
			Log,
			TEXT("[StartWidget] 기존 플레이어 로드 / Nickname=%s PlayerId=%d"),
			*Nickname,
			PlayerId
		);
	}	
	else
	{
		// 없으면 신규 생성
		FPlayerSaveData NewPlayerData;
		NewPlayerData.Nickname = Nickname;
		NewPlayerData.CurrentHP = 100.f;
		NewPlayerData.CurrentST = 100.f;
		NewPlayerData.CurrentMP = 100.f;
		NewPlayerData.Gold = 0;

		PlayerId = GI->CreatePlayerData(NewPlayerData);

		if (PlayerId <= 0)
		{
			if (TB_Popup)
			{
				TB_Popup->SetText(
					FText::FromString(TEXT("플레이어 생성에 실패했습니다."))
				);
			}

			return;
		}
		if (TB_Popup)
		{
			TB_Popup->SetText(
				FText::FromString(TEXT("새 플레이어를 생성했습니다."))
			);
		}

		UE_LOG(
			LogTemp,
			Log,
			TEXT("[StartWidget] 새 플레이어 생성 / Nickname=%s PlayerId=%d"),
			*Nickname,
			PlayerId
		);
	}
	GI->SetCurrentPlayerId(PlayerId);
	UGameplayStatics::OpenLevel(this, FName(TEXT("GameLevel")));
}
