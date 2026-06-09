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
	if (MoveComp)
	{
		MoveComp->MaxWalkSpeed = GetStatComponent()->GetMoveSpeed();
		MoveComp->MaxWalkSpeedCrouched = GetStatComponent()->GetCrouchMoveSpeed();
		MoveComp->bOrientRotationToMovement = true;
		MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	}
	
	CreatePlayerHUD();
	
	if (GoldComponent)
	{
		GoldComponent->AddGold(1000);
	}
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
	}
	else
	{
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

//===============================================================================================
// 이동관련
//===============================================================================================
void APlayerCharacter::DoDash()
{
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
	MoveComp->MaxWalkSpeedCrouched = GetStatComponent()->GetCrouchMoveSpeed();
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
	if (UPlayerCombatComponent* PlayerCombat = Cast<UPlayerCombatComponent>(CombatBaseComponent))
	{
		PlayerCombat->RequestAttack(EAttackType::Light);
	}
}

void APlayerCharacter::DoHeavyAttack(const FInputActionValue& Value)
{
	if (UPlayerCombatComponent* PlayerCombat = Cast<UPlayerCombatComponent>(CombatBaseComponent))
	{
		PlayerCombat->RequestAttack(EAttackType::Heavy);
	}
}
void APlayerCharacter::DoChargedAttack()
{
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
	if (BuffMPCost > 0.f && !GetStatComponent()->ConsumeMP(BuffMPCost)) return;
	
	bIsBattleBuffOnCooldown = true;
	
	if (BattleBuffMontage)
	{
		PlayAnimMontage(BattleBuffMontage);
	}
	else
	{
		ApplyBattleBuff();
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
	if (bIsBattleBuffActive) return false;
	if (bIsBattleBuffOnCooldown) return false;
	if (!StatComponent) return false;
	if (StatComponent->GetCurrentMP() < BuffMPCost) return false;
	if (CombatBaseComponent && CombatBaseComponent->IsGuarding()) return false; 
	if (CombatBaseComponent && CombatBaseComponent->IsAttacking()) return false; 
	if (CombatBaseComponent && CombatBaseComponent->IsCharging()) return false; 
	return true;
}
//===============================================================================================
// 가드 관련 
//===============================================================================================
void APlayerCharacter::StartGuard()
{
	if (!SkillComponent) return;
	if (!SkillComponent->CanUseParry()) return;
	if (CombatBaseComponent) CombatBaseComponent->StartGuard();
}

void APlayerCharacter::EndGuard()
{
	if (CombatBaseComponent) CombatBaseComponent->EndGuard();
}

void APlayerCharacter::OnDead()
{
	Super::OnDead();
	bIsDashing = false;

	if (MoveComp)
	{
		MoveComp->StopMovementImmediately();
	}

	SetUIMode(true);
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

