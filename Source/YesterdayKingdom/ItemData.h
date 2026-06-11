// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

//===============================================================================================
// 아이템 전용 타입
//===============================================================================================
UENUM(BlueprintType)
enum class EItemType : uint8
{
	None		UMETA(DisplayName = "None"),
	Consumable	UMETA(DisplayName = "Consumable"),
	Weapon		UMETA(DisplayName = "Weapon"),
	Armor 		UMETA(DisplayName = "Armor"),
	Quest		UMETA(DisplayName = "Quest")
};
// 아이템 희귀도
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common    UMETA(DisplayName = "Common"),
	Uncommon  UMETA(DisplayName = "Uncommon"),
	Rare      UMETA(DisplayName = "Rare"),
	Epic      UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary")
};
// 무기 장착 종류
UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	None      UMETA(DisplayName = "None"),
	MainHand  UMETA(DisplayName = "MainHand"),   // 주로 오른손
	OffHand   UMETA(DisplayName = "OffHand"),    // 보조 (방패 등)
	BothHands UMETA(DisplayName = "BothHands")   // 양손 무기 (활, 양손검 등)
};

// 무기 종류
UENUM(BlueprintType)
enum class EWeaponCategory : uint8
{
	None    UMETA(DisplayName = "None"),
	Sword   UMETA(DisplayName = "Sword"),
	Axe     UMETA(DisplayName = "Axe"),
	Bow     UMETA(DisplayName = "Bow"),
	Dagger  UMETA(DisplayName = "Dagger"),
	Staff   UMETA(DisplayName = "Staff"),
	Shield  UMETA(DisplayName = "Shield")
};
// 버프 종류
UENUM(BlueprintType)
enum class EPotionBuffType : uint8
{
	None    UMETA(DisplayName = "None"),
	HpRegen UMETA(DisplayName = "HpRegen"),   // 체력 초당 회복
	STRegen UMETA(DisplayName = "STRegen"),	  // 스테미나 초당 회복
	MpRegen UMETA(DisplayName = "MpRegen"),   // 마나 초당 회복
	AtkUp   UMETA(DisplayName = "AtkUp"),     // 공격력 증가
	DefUp   UMETA(DisplayName = "DefUp"),     // 방어력 증가
	SpeedUp UMETA(DisplayName = "SpeedUp")    // 이동속도 증가
};
//===============================================================================================
// 무기창 전용
//===============================================================================================
UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
	None        UMETA(DisplayName = "None"),

	Weapon      UMETA(DisplayName = "Weapon"),

	Helmet      UMETA(DisplayName = "Helmet"),
	Armor       UMETA(DisplayName = "Armor"),
	Gloves      UMETA(DisplayName = "Gloves"),
	Boots       UMETA(DisplayName = "Boots"),

	Accessory   UMETA(DisplayName = "Accessory")
};

//===============================================================================================
// 아이템 구조체 
//===============================================================================================
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
    GENERATED_BODY()

//===============================================================================================
// 공통 속셩
//===============================================================================================
    // 아이템 종류
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common")
    EItemType ItemType = EItemType::None;

    // 인벤토리 슬롯 및 툴팁에 표시되는 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common")
    FText ItemName;

    // 툴팁에 표시될 아이템 설명 문구
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common")
    FText ItemDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common")
	FText EffectDescription;

    // 아이템 희귀도 — Common / Uncommon / Rare / Epic / Legendary
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common")
    EItemRarity Rarity = EItemRarity::Common;
    
    // 아이템 텍스쳐
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common")
    TObjectPtr<UTexture2D> Icon = nullptr;

    // 슬롯 아이템 보유 갯수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common", meta = (ClampMin = "1"))
    int32 MaxStackCount = 1;

	// 아이템 판매 가격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common", meta = (ClampMin = "0"))
	int32 BuyPrice  = 0;
	
    // 아이템 판매 가격
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common", meta = (ClampMin = "0"))
    int32 SellPrice = 0;

    // 드랍 가중 치
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common", meta = (ClampMin = "1"))
    int32 DropWeight = 10;

//===============================================================================================
// 무기 전용
//===============================================================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::None;
	
     // 무기 종류 — 공격 애니메이션·사운드 분기에 활용
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    EWeaponCategory WeaponCategory = EWeaponCategory::None;

    // 장착 슬롯 (MainHand / OffHand / BothHands)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    EWeaponSlot WeaponSlot = EWeaponSlot::None;

    // 기본 공격력 수치
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (ClampMin = "0"))
    int32 AttackPower = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (ClampMin = "0"))
	int32 DefensePower = 0;
	
    // 공격 속도 배율 (1.0 = 기본 속도 / 값이 클수록 빠름)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (ClampMin = "0.1", ClampMax = "5.0"))
    float AttackSpeed = 1.0f;

    // 장착 가능한 요구 캐릭터 레벨
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", meta = (ClampMin = "1"))
    int32 RequiredLevel = 1;

    // 장착 시 캐릭터 소켓에 붙일 무기 스켈레탈 메시
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    TObjectPtr<USkeletalMesh> WeaponMesh = nullptr;
	
//===============================================================================================
// 포션 전용
//===============================================================================================

    // 섭취 즉시 회복하는 HP 량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable", meta = (ClampMin = "0"))
    int32 InstantHeal = 0;

    // 섭취 즉시 회복하는 MP 량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable", meta = (ClampMin = "0"))
    int32 InstantMana = 0;

    // 섭취 후 지속적으로 적용될 버프 종류 (None이면 즉시 효과만 적용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
    EPotionBuffType BuffType = EPotionBuffType::None;

    // 버프 수치 (예: SpeedUp이면 +0.3 배율, AtkUp이면 +15 공격력)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable", meta = (ClampMin = "0.0"))
    float BuffValue = 0.0f;

    // 버프가 유지되는 시간 (초 단위 / 0이면 지속 버프 없음)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable", meta = (ClampMin = "0.0"))
    float BuffDuration = 0.0f;
};
