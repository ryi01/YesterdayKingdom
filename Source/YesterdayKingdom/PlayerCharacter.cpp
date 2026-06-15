// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "BaseStatComponent.h"
#include "CombatBaseComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EquipmentComponent.h"
#include "GoldComponent.h"
#include "InputActionValue.h"
#include "InventoryComponent.h"
#include "PlayerCombatComponent.h"
#include "PlayerDefinition.h"
#include "PlayerHUDWidget.h"
#include "PlayerInteractionComponent.h"
#include "PlayerSkillComponent.h"
#include "PlayerStatComponent.h"
#include "QuestComponent.h"
#include "YesterdayKingdomGameInstance.h"
#include "Kismet/GameplayStatics.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) 
: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerCombatComponent>(TEXT("CombatComponent")).SetDefaultSubobjectClass<UPlayerStatComponent>(TEXT("StatComponent")))
{
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(WeaponRoot);
	
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	GoldComponent = CreateDefaultSubobject<UGoldComponent>(TEXT("GoldComponent"));
	InteractionComponent = CreateDefaultSubobject<UPlayerInteractionComponent>(TEXT("InteractionComponent"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
	QuestComponent = CreateDefaultSubobject<UQuestComponent>(TEXT("QuestComponent"));
	SkillComponent = CreateDefaultSubobject<UPlayerSkillComponent>(TEXT("SkillTreeComponent"));
	
	bDestroyOnDeath = false;
}



void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
	
	if (WeaponMesh) WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
	MoveComp = GetCharacterMovement();

	QuickSlotItemRowNames.SetNum(5);

	StatComponent->ResetSkillBonuses();
	
	LoadPlayerData();
	
	if (InventoryComponent)
	{
		InventoryComponent->LoadInventoryData();
	}
	if (EquipmentComponent)
	{
		EquipmentComponent->LoadEquipmentData();
	}
	if (SkillComponent)
	{
		SkillComponent->LoadSkillTreeData();
	}
	
	MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->MaxWalkSpeed = GetStatComponent()->GetMoveSpeed();
		MoveComp->bOrientRotationToMovement = true;
	}
	
	CreatePlayerHUD();
	
	SetUIMode(false);
	// 인터렉션 대상 체크
	GetWorld()->GetTimerManager().SetTimer(InteractionCheckTimerHandle, this, &APlayerCharacter::UpdateInteractionTarget, 0.1f, true);
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bIsDashing || !GetStatComponent()) return;
	UPlayerStatComponent* PlayerStat = Cast<UPlayerStatComponent>(GetStatComponent());
	if (!PlayerStat || !PlayerStat->GetPlayerDefinition()) return;
	const float Cost = PlayerStat->GetPlayerDefinition()->DashSTCostPerSecond * DeltaSeconds;
	if (!GetStatComponent()->ConsumeST(Cost)) DoDashStop();
}

//===============================================================================================
// 로드
//===============================================================================================
void APlayerCharacter::LoadPlayerData()
{
	UYesterdayKingdomGameInstance* GameInstance = GetGameInstance<UYesterdayKingdomGameInstance>();
	if (!GameInstance || !StatComponent || !GoldComponent) return;
	const int32 PlayerId = GameInstance->GetCurrentPlayerId();
	if (PlayerId <= 0) return;
	FPlayerSaveData LoadData;
	if (GameInstance->LoadPlayerData(PlayerId, LoadData))
	{
		StatComponent->LoadCurrentStats(LoadData.CurrentHP, LoadData.CurrentST, LoadData.CurrentMP);
		GoldComponent->LoadGold(LoadData.Gold);
	}
	
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[Player] 데이터 적용 완료 / ID=%d Nickname=%s"),
		LoadData.PlayerId,
		*LoadData.Nickname
	);
}

void APlayerCharacter::SavePlayerData()
{
	UYesterdayKingdomGameInstance* GameInstance = GetGameInstance<UYesterdayKingdomGameInstance>();
	if (!GameInstance || !StatComponent || !GoldComponent) return;

	const int32 PlayerId = GameInstance->GetCurrentPlayerId();
	if (PlayerId <= 0) return;

	FPlayerSaveData SaveData;
	SaveData.PlayerId = PlayerId;
	SaveData.CurrentHP = StatComponent->GetCurrentHP();
	SaveData.CurrentST = StatComponent->GetCurrentST();
	SaveData.CurrentMP = StatComponent->GetCurrentMP();
	SaveData.Gold = GoldComponent->GetGold();

	GameInstance->SavePlayerData(SaveData);
}
void APlayerCharacter::SaveGamePlay()
{
	SavePlayerData();
	if (InventoryComponent)
	{
		InventoryComponent->SaveInventoryData();
	}
	if (EquipmentComponent)
	{
		EquipmentComponent->SaveEquipmentData();
	}
	if (SkillComponent)
	{
		SkillComponent->SaveSkillTreeData();
	}
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	// 움직임 화면시선처리 
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	
	// 인터렉션
	EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &APlayerCharacter::Interaction);
	
	// 인벤토리
	EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleInventory);
	
	// 대쉬
	EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &APlayerCharacter::DoDash);
	EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &APlayerCharacter::DoDashStop);
	
	// 공격
	EnhancedInputComponent->BindAction(ChargeAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::DoChargedAttack);
	EnhancedInputComponent->BindAction(ChargeAttackAction, ETriggerEvent::Completed, this, &APlayerCharacter::DoChargeRelease);
	EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::DoLightAttack);
	EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::DoHeavyAttack);
	
	EnhancedInputComponent->BindAction(BuffAction, ETriggerEvent::Started, this, &APlayerCharacter::DoBattleBuff);
	
	EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Started, this, &APlayerCharacter::StartGuard);
	EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Completed, this, &APlayerCharacter::EndGuard);

	EnhancedInputComponent->BindAction(QuickSlot1Action, ETriggerEvent::Started, this, &APlayerCharacter::UseQuickSlot1);
	EnhancedInputComponent->BindAction(QuickSlot2Action, ETriggerEvent::Started, this, &APlayerCharacter::UseQuickSlot2);
	EnhancedInputComponent->BindAction(QuickSlot3Action, ETriggerEvent::Started, this, &APlayerCharacter::UseQuickSlot3);
	EnhancedInputComponent->BindAction(QuickSlot4Action, ETriggerEvent::Started, this, &APlayerCharacter::UseQuickSlot4);
	EnhancedInputComponent->BindAction(QuickSlot5Action, ETriggerEvent::Started, this, &APlayerCharacter::UseQuickSlot5);
	
	EnhancedInputComponent->BindAction(TestSkillAction, ETriggerEvent::Started, this, &APlayerCharacter::TestUnlockSkill);
}
//===============================================================================================
// 위잿
//===============================================================================================
void APlayerCharacter::CreatePlayerHUD()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;
	if (!PlayerHUDWidgetClass) return;
	PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(PC, PlayerHUDWidgetClass);
	if (!PlayerHUDWidget) return;
	PlayerHUDWidget->AddToViewport();
	PlayerHUDWidget->BindPlayer(this);
	PlayerHUDWidget->SetInventoryVisible(false);
}

void APlayerCharacter::UseQuickSlot1()
{
	UseQuickSlot(0);
}

void APlayerCharacter::UseQuickSlot2()
{
	UseQuickSlot(1);
}

void APlayerCharacter::UseQuickSlot3()
{
	UseQuickSlot(2);
}

void APlayerCharacter::UseQuickSlot4()
{
	UseQuickSlot(3);
}

void APlayerCharacter::UseQuickSlot5()
{
	UseQuickSlot(4);
}

void APlayerCharacter::SetUIMode(bool bEnableUI)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;
	if (bEnableUI)
	{
		PC->bShowMouseCursor = true;

		FInputModeGameAndUI InputModeGameAndUI;
		InputModeGameAndUI.SetHideCursorDuringCapture(false);
		InputModeGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputModeGameAndUI);

		if (MoveComp)
		{
			MoveComp->StopMovementImmediately();
		}

		bIsDashing = false;
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
	else
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		PC->bShowMouseCursor = false;

		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
}

//===============================================================================================
// 이동
//===============================================================================================
void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (bIsCastingBattleBuff) return;
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
		
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

//===============================================================================================
// 인벤토리
//===============================================================================================
void APlayerCharacter::ToggleInventory()
{
	if (!PlayerHUDWidget) return;
	bIsInventoryOpen = !bIsInventoryOpen;
	PlayerHUDWidget->SetInventoryVisible(bIsInventoryOpen);
	SetUIMode(bIsInventoryOpen);
}

void APlayerCharacter::CloseInventory()
{
	bIsInventoryOpen = false;

	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->SetInventoryVisible(false);
	}

	SetUIMode(false);
}


//===============================================================================================
// 이동관련
//===============================================================================================
void APlayerCharacter::DoDash()
{
	if (bIsCastingBattleBuff) return;
	if (GetStatComponent()->GetCurrentST() <= 0.f) return;
	bIsDashing = true;
	RefreshMoveSpeed();
}

void APlayerCharacter::DoDashStop()
{
	bIsDashing = false;
	RefreshMoveSpeed();
}

void APlayerCharacter::RefreshMoveSpeed()
{
	if (!MoveComp || !GetStatComponent()) return;
	float NewSpeed = bIsDashing ? GetStatComponent()->GetRunSpeed() : GetStatComponent()->GetMoveSpeed();
	if (bIsBattleBuffActive) NewSpeed *= BuffMoveSpeedMultiplier;
	MoveComp->MaxWalkSpeed = NewSpeed;
	MoveComp->MaxWalkSpeedCrouched = GetStatComponent()->GetGuardMoveSpeed();
}

bool APlayerCharacter::UseConsumableItem(const FItemData& ItemData)
{
	if (!GetStatComponent()) return false;
	if (ItemData.ItemType != EItemType::Consumable) return false;

	bool bAppliedEffect = false;

	if (ItemData.InstantHeal > 0)
	{
		GetStatComponent()->Heal(ItemData.InstantHeal);
		bAppliedEffect = true;
	}

	if (ItemData.InstantMana > 0)
	{
		GetStatComponent()->RecoverMP(ItemData.InstantMana);
		bAppliedEffect = true;
	}
	
	if (ItemData.InstantStamina > 0)
	{
		GetStatComponent()->RecoverST(ItemData.InstantStamina);
		bAppliedEffect = true;
	}

	return bAppliedEffect;
	
}

//===============================================================================================
// 피격 관련 함수
//===============================================================================================
void APlayerCharacter::NotifyDamage_Implementation(const FVector& DamageLocation, AActor* DamageSource)
{
	Super::NotifyDamage_Implementation(DamageLocation, DamageSource);
	PlayHitFlash();

	if (PlayerHitCameraShake)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->ClientStartCameraShake(PlayerHitCameraShake);
		}
	}
}


void APlayerCharacter::PlayHitFlash()
{
	if (!HitOverlayMaterial) return;
	GetMesh()->SetOverlayMaterial(HitOverlayMaterial);
	
	GetWorld()->GetTimerManager().ClearTimer(HitFlashTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(HitFlashTimerHandle, this, &APlayerCharacter::EndHitFlash, HitFlashDuration, false);
}

void APlayerCharacter::EndHitFlash()
{
	GetMesh()->SetOverlayMaterial(nullptr);
}

//===============================================================================================
// 공격 관련
//===============================================================================================
void APlayerCharacter::DoLightAttack(const FInputActionValue& Value)
{
	if (bIsCastingBattleBuff) return;
	if (UPlayerCombatComponent* PlayerCombat = Cast<UPlayerCombatComponent>(CombatBaseComponent))
	{
		PlayerCombat->RequestAttack(EAttackType::Light);
	}
}

void APlayerCharacter::DoHeavyAttack(const FInputActionValue& Value)
{
	if (bIsCastingBattleBuff) return;
	if (UPlayerCombatComponent* PlayerCombat = Cast<UPlayerCombatComponent>(CombatBaseComponent))
	{
		PlayerCombat->RequestAttack(EAttackType::Heavy);
	}
}
void APlayerCharacter::DoChargedAttack()
{
	if (bIsCastingBattleBuff) return;
	if (UPlayerCombatComponent* PlayerCombat = Cast<UPlayerCombatComponent>(CombatBaseComponent))
	{
		PlayerCombat->StartChargeAttack();
	}
}

void APlayerCharacter::DoChargeRelease()
{
	if (UPlayerCombatComponent* PlayerCombat = Cast<UPlayerCombatComponent>(CombatBaseComponent))
	{
		PlayerCombat->ReleaseChargeAttack();
	}
}

void APlayerCharacter::CheckCombo_Implementation()
{
	if (CombatBaseComponent)
	{
		CombatBaseComponent->CheckCombo();
	}
}
//===============================================================================================
// 버프 스킬 관련
//===============================================================================================
void APlayerCharacter::ApplyBattleBuff()
{
	if (bIsBattleBuffActive) return;
	if (!GetStatComponent()) return;
	
	bIsBattleBuffActive = true;
	
	GetStatComponent()->AddBuffAttack(BattleBuffAttackBonus);
	GetStatComponent()->AddBuffDefense(BattleBuffDefenseBonus);
	
	RefreshMoveSpeed();

	GetWorld()->GetTimerManager().ClearTimer(BattleBuffTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(BattleBuffTimerHandle, this, &APlayerCharacter::EndBattleBuff, BuffDuration, false);

}

void APlayerCharacter::DoBattleBuff()
{
	if (!CanUseBattleBuff() || !GetStatComponent()) return;
	if (!SkillComponent || !SkillComponent->CanUseBattleBuff()) return;
	if (BuffMPCost > 0.f && !GetStatComponent()->ConsumeMP(BuffMPCost)) return;
	
	bIsCastingBattleBuff = true;
	bIsBattleBuffOnCooldown = true;
	bIsDashing = false;
	if (MoveComp)
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}

	if (CombatBaseComponent && CombatBaseComponent->IsGuarding())
	{
		CombatBaseComponent->EndGuard();
	}
	
	if (BattleBuffMontage)
	{
		const float PlayedLength = PlayAnimMontage(BattleBuffMontage);

		if (PlayedLength <= 0.f)
		{
			FinishBattleBuffCasting();
		}
	}
	else
	{
		ApplyBattleBuff();
		FinishBattleBuffCasting();
	}
	
	GetWorld()->GetTimerManager().ClearTimer(BattleBuffCooldownTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(BattleBuffCooldownTimerHandle, this, &APlayerCharacter::EndBattleBuffCooldown, BattleBuffCooldown, false);

}
void APlayerCharacter::EndBattleBuff()
{
	if (!bIsBattleBuffActive) return;

	bIsBattleBuffActive = false;

	if (StatComponent)
	{
		StatComponent->ClearAllBuffStats();
	}

	RefreshMoveSpeed();
	
	UE_LOG(LogTemp, Warning, TEXT("buffEnd : %f"), MoveComp->MaxWalkSpeed);
}

void APlayerCharacter::EndBattleBuffCooldown()
{
	bIsBattleBuffOnCooldown = false;
	UE_LOG(LogTemp, Warning, TEXT("BuffCoolDown End"));
}

bool APlayerCharacter::CanUseBattleBuff() const
{
	if (bIsCastingBattleBuff) return false;
	if (bIsBattleBuffActive) return false;
	if (bIsBattleBuffOnCooldown) return false;
	if (!StatComponent) return false;
	if (StatComponent->GetCurrentMP() < BuffMPCost) return false;
	if (CombatBaseComponent && CombatBaseComponent->IsGuarding()) return false; 
	if (CombatBaseComponent && CombatBaseComponent->IsAttacking()) return false; 
	if (CombatBaseComponent && CombatBaseComponent->IsCharging()) return false; 
	return true;
}

bool APlayerCharacter::IsCastingBattleBuff() const
{
	return bIsCastingBattleBuff;
}

void APlayerCharacter::FinishBattleBuffCasting()
{
	if (!bIsCastingBattleBuff) return;

	bIsCastingBattleBuff = false;

	if (!bIsBattleBuffActive)
	{
		ApplyBattleBuff();
	}

	if (MoveComp)
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}

	RefreshMoveSpeed();
}

//===============================================================================================
// 가드 관련 
//===============================================================================================
void APlayerCharacter::StartGuard()
{
	if (bIsCastingBattleBuff) return;
	if (!SkillComponent) return;
	if (!SkillComponent->CanUseParry()) return;
	if (CombatBaseComponent) CombatBaseComponent->StartGuard();
}

void APlayerCharacter::EndGuard()
{
	if (CombatBaseComponent) CombatBaseComponent->EndGuard();
}
//===============================================================================================
// UI 관련
//===============================================================================================
void APlayerCharacter::ShowBossHP(AEnemyBase* Boss)
{
	if (!PlayerHUDWidget) return;

	PlayerHUDWidget->BindBoss(Boss);
}

void APlayerCharacter::HideBossHP()
{
	if (!PlayerHUDWidget) return;

	PlayerHUDWidget->UnbindBoss();
}

void APlayerCharacter::OnDead()
{
	Super::OnDead();
	OnPlayerDead.Broadcast();
	bIsDashing = false;

	if (MoveComp)
	{
		MoveComp->StopMovementImmediately();
	}
	
}

//===============================================================================================
// 인터렉션 관련
//===============================================================================================
void APlayerCharacter::Interaction()
{
	if (InteractionComponent)
	{
		InteractionComponent->Interact();
	}	
}

void APlayerCharacter::TestUnlockSkill()
{
	if (!SkillComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SkillTest] SkillComponent is null"));
		return;
	}

	if (!GoldComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SkillTest] GoldComponent is null"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[SkillTest] Attack Before: %f"), StatComponent->GetFinalAttack());
	UE_LOG(LogTemp, Warning, TEXT("[SkillTest] Gold Before: %d"), GoldComponent->GetGold());
	
	const float AttackBefore = StatComponent ? StatComponent->GetFinalAttack() : 0.f;
	const int32 GoldBefore = GoldComponent->GetGold();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
			FString::Printf(TEXT("[SkillTest] Attack Before: %.1f"), AttackBefore));

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
			FString::Printf(TEXT("[SkillTest] Gold Before: %d"), GoldBefore));
	}
	const bool bResult = SkillComponent->TryUnlockSkill(TEXT("SK_Charge_01"));

	UE_LOG(LogTemp, Warning, TEXT("[SkillTest] Unlock Result: %s"), bResult ? TEXT("Success") : TEXT("Failed"));
	UE_LOG(LogTemp, Warning, TEXT("[SkillTest] Gold After: %d"), GoldComponent->GetGold());
	UE_LOG(LogTemp, Warning, TEXT("[SkillTest] FinalAttack: %f"), StatComponent->GetFinalAttack());
	const int32 GoldAfter = GoldComponent->GetGold();
	const float AttackAfter = StatComponent ? StatComponent->GetFinalAttack() : 0.f;
	if (GEngine)
	{
		const FColor ResultColor = bResult ? FColor::Green : FColor::Red;

		GEngine->AddOnScreenDebugMessage(-1, 3.f, ResultColor,
			FString::Printf(TEXT("[SkillTest] Unlock Result: %s"), bResult ? TEXT("Success") : TEXT("Failed")));

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
			FString::Printf(TEXT("[SkillTest] Gold After: %d"), GoldAfter));

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
			FString::Printf(TEXT("[SkillTest] FinalAttack: %.1f"), AttackAfter));
	}
}

void APlayerCharacter::UpdateInteractionTarget()
{
	if (InteractionComponent)
	{
		InteractionComponent->UpdateInteractTarget();
	}
}
//===============================================================================================
// 퀵슬롯
//===============================================================================================
void APlayerCharacter::TryAutoRegisterQuickSlot(FName ItemRowName)
{
	if (ItemRowName.IsNone()) return;
	if (!InventoryComponent) return;

	const FItemData* ItemData = InventoryComponent->GetItemData(ItemRowName);
	if (!ItemData) return;

	if (ItemData->ItemType != EItemType::Consumable) return;

	for (int32 i = 0; i < QuickSlotItemRowNames.Num(); i++)
	{
		if (QuickSlotItemRowNames[i] == ItemRowName)
		{
			RefreshQuickSlotUI(i);
			return;
		}
	}

	for (int32 i = 0; i < QuickSlotItemRowNames.Num(); i++)
	{
		if (QuickSlotItemRowNames[i].IsNone())
		{
			QuickSlotItemRowNames[i] = ItemRowName;
			RefreshQuickSlotUI(i);
			return;
		}
	}
}
void APlayerCharacter::UseQuickSlot(int32 QuickSlotIndex)
{
	if (!QuickSlotItemRowNames.IsValidIndex(QuickSlotIndex)) return;
	if (!InventoryComponent) return;

	const FName ItemRowName = QuickSlotItemRowNames[QuickSlotIndex];
	if (ItemRowName.IsNone()) return;

	const FItemData* ItemData = InventoryComponent->GetItemData(ItemRowName);
	if (!ItemData) return;

	const bool bUsed = UseConsumableItem(*ItemData);
	if (!bUsed) return;

	InventoryComponent->RemoveItem(ItemRowName, 1);

	if (!InventoryComponent->HasItem(ItemRowName, 1))
	{
		QuickSlotItemRowNames[QuickSlotIndex] = NAME_None;
	}

	RefreshQuickSlotUI(QuickSlotIndex);
}

void APlayerCharacter::RefreshQuickSlotByItem(FName ItemRowName)
{
	if (ItemRowName.IsNone()) return;
	if (!InventoryComponent) return;

	for (int32 QuickSlotIndex = 0; QuickSlotIndex < QuickSlotItemRowNames.Num(); QuickSlotIndex++)
	{
		if (QuickSlotItemRowNames[QuickSlotIndex] != ItemRowName) continue;

		if (!InventoryComponent->HasItem(ItemRowName, 1))
		{
			QuickSlotItemRowNames[QuickSlotIndex] = NAME_None;
		}

		RefreshQuickSlotUI(QuickSlotIndex);
	}
}

void APlayerCharacter::RefreshQuickSlotUI(int32 QuickSlotIndex)
{
	if (!PlayerHUDWidget) return;
	if (!InventoryComponent) return;
	if (!QuickSlotItemRowNames.IsValidIndex(QuickSlotIndex)) return;

	PlayerHUDWidget->UpdateQuickSlot(QuickSlotIndex, QuickSlotItemRowNames[QuickSlotIndex]);
}
//===============================================================================================
// 스토어
//===============================================================================================
void APlayerCharacter::OpenStoreUI(UStoreComponent* InStoreComponent)
{
	if (!PlayerHUDWidget || !InStoreComponent) return;

	PlayerHUDWidget->OpenStore(InStoreComponent);
	SetUIMode(true);
}

void APlayerCharacter::CloseStoreUI()
{
	SetUIMode(false);
}

//===============================================================================================
// 컴포넌트 Getter
//===============================================================================================
UGoldComponent* APlayerCharacter::GetGoldComponent() const
{
	return GoldComponent;
}

UInventoryComponent* APlayerCharacter::GetInventoryComponent() const
{
	return  InventoryComponent;
}
UPlayerInteractionComponent* APlayerCharacter::GetInteractionComponent() const
{
	return InteractionComponent;
}

UEquipmentComponent* APlayerCharacter::GetEquipmentComponent() const
{
	return EquipmentComponent;
}

UQuestComponent* APlayerCharacter::GetQuestComponent() const
{
	return QuestComponent;
}

UPlayerSkillComponent* APlayerCharacter::GetSkillComponent() const
{
	return SkillComponent;
}

UPlayerHUDWidget* APlayerCharacter::GetPlayerHUDWidget() const
{
	return PlayerHUDWidget;
}

