// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "BaseStatComponent.h"
#include "CombatBaseComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GoldComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "InventoryComponent.h"
#include "PlayerCombatComponent.h"
#include "PlayerDefinition.h"
#include "PlayerStatComponent.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) 
: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerCombatComponent>(TEXT("CombatComponent")).SetDefaultSubobjectClass<UPlayerStatComponent>(TEXT("StatComponent")))
{
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(WeaponRoot);
	
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	GoldComponent = CreateDefaultSubobject<UGoldComponent>(TEXT("GoldComponent"));
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
	
	MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->MaxWalkSpeed = GetStatComponent()->GetMoveSpeed();
		MoveComp->MaxWalkSpeedCrouched = GetStatComponent()->GetCrouchMoveSpeed();
		MoveComp->bOrientRotationToMovement = true;
		MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	}

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
}

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

void APlayerCharacter::Interaction()
{
	UE_LOG(LogTemp, Log, TEXT("Interaction"));	
}

void APlayerCharacter::ToggleInventory()
{
	UE_LOG(LogTemp, Log, TEXT("Inventory"));	
}

UGoldComponent* APlayerCharacter::GetGoldComponent() const
{
	return GoldComponent;
}

UInventoryComponent* APlayerCharacter::GetInventoryComponent() const
{
	return  InventoryComponent;
}

void APlayerCharacter::DoDash()
{
	if (GetStatComponent()->GetCurrentST() <= 0.f) return;
	bIsDashing = true;
	if (MoveComp) MoveComp->MaxWalkSpeed = GetStatComponent()->GetRunSpeed();
}

void APlayerCharacter::DoDashStop()
{
	bIsDashing = false;
	if (MoveComp) MoveComp->MaxWalkSpeed = GetStatComponent()->GetMoveSpeed(); // 임시로 600 설정
}

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
<<<<<<< Updated upstream
=======

//===============================================================================================
// 버프 스킬 관련
//===============================================================================================

void APlayerCharacter::DoBattleBuff()
{
	if (!CanUseBattleBuff() || !GetStatComponent()) return;
	if (BuffMPCost > 0.f && !GetStatComponent()->ConsumeMP(BuffMPCost)) return;
	
	bIsBattleBuffActive = true;
	bIsBattleBuffOnCooldown = true;
	
	GetStatComponent()->AddBuffAttack(BattleBuffAttackBonus);
	GetStatComponent()->AddBuffDefense(BattleBuffDefenseBonus);
	
	if (MoveComp) MoveComp->MaxWalkSpeed = GetStatComponent()->GetMoveSpeed() * BuffMoveSpeedMultiplier;
	UE_LOG(LogTemp, Warning, TEXT("BuffStart : %f"), MoveComp->MaxWalkSpeed);
	GetWorld()->GetTimerManager().ClearTimer(BattleBuffTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(BattleBuffTimerHandle, this, &APlayerCharacter::EndBattleBuff, BuffDuration, false);

	GetWorld()->GetTimerManager().ClearTimer(BattleBuffCooldownTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(BattleBuffCooldownTimerHandle, this, &APlayerCharacter::EndBattleBuffCooldown, BattleBuffCooldown, false);

}
void APlayerCharacter::EndBattleBuff()
{
	if (!bIsBattleBuffActive) return;

	bIsBattleBuffActive = false;

	if (GetStatComponent())
	{
		GetStatComponent()->ClearAllBuffStats();
	}

	if (MoveComp && GetStatComponent())
	{
		MoveComp->MaxWalkSpeed = GetStatComponent()->GetMoveSpeed();
	}
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
	if (!GetStatComponent()) return false;
	if (GetStatComponent()->GetCurrentMP() < BuffMPCost) return false;

	return true;
}
//===============================================================================================
// 가드 관련 
//===============================================================================================
void APlayerCharacter::StartGuard()
{
	if (CombatBaseComponent) CombatBaseComponent->StartGuard();
}

void APlayerCharacter::EndGuard()
{
	if (CombatBaseComponent) CombatBaseComponent->EndGuard();
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
>>>>>>> Stashed changes
