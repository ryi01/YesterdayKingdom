// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "BaseStatComponent.h"
#include "CombatBaseComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "PlayerCombatComponent.h"


APlayerCharacter::APlayerCharacter()
{
	CombatBaseComponent = CreateDefaultSubobject<UPlayerCombatComponent>(TEXT("CombatComponent"));
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
		MoveComp->JumpZVelocity = JumpZPower;
		MoveComp->bOrientRotationToMovement = true;
	}
}


void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	// 움직임 화면시선처리 
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::DoJump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::DoJumpStop);
	EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &APlayerCharacter::DoDash);
	EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &APlayerCharacter::DoDashStop);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::DoChargedAttack);
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
	AddControllerPitchInput(-LookAxisVector.Y);
}

void APlayerCharacter::DoJump()
{
	Jump();
}

void APlayerCharacter::DoJumpStop()
{
	StopJumping();
}

void APlayerCharacter::DoDash(const FInputActionValue& Value)
{
	if (MoveComp) MoveComp->MaxWalkSpeed = GetStatComponent()->GetRunSpeed();
}

void APlayerCharacter::DoDashStop(const FInputActionValue& Value)
{
	if (MoveComp) MoveComp->MaxWalkSpeed = GetStatComponent()->GetMoveSpeed(); // 임시로 600 설정
}

void APlayerCharacter::DoChargedAttack()
{
	IAttacker::Execute_ChargeCombo(this);
	
	if (ChargedAttackMontage)
	{
		PlayAnimMontage(ChargedAttackMontage);
	}
		UE_LOG(LogTemp, Warning, TEXT("ChargeAttack Montage Played"));
}

void APlayerCharacter::DoLightAttack(const FInputActionValue& Value)
{
	if (CombatBaseComponent)  CombatBaseComponent->RequestAttack(TEXT("LightAttack"));
}

void APlayerCharacter::DoHeavyAttack(const FInputActionValue& Value)
{
	if (CombatBaseComponent)  CombatBaseComponent->RequestAttack(TEXT("HeavyAttack"));
}

void APlayerCharacter::CheckCombo_Implementation()
{
	Super::CheckCombo_Implementation();
	
	FString AttackType = bIsHeavyAttack ? TEXT("Heavy") : TEXT("Light");
	UE_LOG(LogTemp, Warning, TEXT("Player %s Attack Checked! Index: %d"), *AttackType, AttackIndex);
	
	bIsAttacking = true;
	
	if (CombatBaseComponent) 
	{
		CombatBaseComponent->CheckCombo();
	}
	
	if (ComboMontages.IsValidIndex(AttackIndex))
	{
		UAnimMontage* SelectedMontage = ComboMontages[AttackIndex];
		
		PlayAnimMontage(SelectedMontage);
		
		AttackIndex = (AttackIndex + 1) % ComboMontages.Num();
	}
}

void APlayerCharacter::Charged_Implementation()
{
	if (CombatBaseComponent) 
	{
		CombatBaseComponent->CheckCombo();
	}
}
