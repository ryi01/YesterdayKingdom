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
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "InventoryComponent.h"
#include "PlayerCombatComponent.h"
#include "PlayerDefinition.h"
#include "PlayerInteractionComponent.h"
#include "PlayerStatComponent.h"

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
		MoveComp->bOrientRotationToMovement = true;
	}
	// 인터렉션 대상 체크
	GetWorld()->GetTimerManager().SetTimer(InteractionCheckTimerHandle, this, &APlayerCharacter::UpdateInteractionTarget, 0.1f, true);
	InventoryComponent->AddItem(TEXT("WP_Sword"), 1);
	EquipmentComponent->EquipItem(TEXT("WP_Sword"));
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

//===============================================================================================
// 인벤토리
//===============================================================================================
void APlayerCharacter::ToggleInventory()
{
	UE_LOG(LogTemp, Log, TEXT("Inventory"));	
}

//===============================================================================================
// 이동관련
//===============================================================================================
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
		PlayerCombat->RequestAttack(EAttackType::Charge);
	}
}

void APlayerCharacter::DoChargeRelease()
{
	if (CombatBaseComponent) CombatBaseComponent->ChargedAttack();
}

void APlayerCharacter::CheckCombo_Implementation()
{
	if (CombatBaseComponent)
	{
		CombatBaseComponent->CheckCombo();
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
