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
		//MoveComp->MaxWalkSpeed = GetStatComponent()->GetMovespeed();
		MoveComp->JumpZVelocity = JumpZPower;
		MoveComp->bOrientRotationToMovement = true;
	}
}

APlayerCharacter::APlayerCharacter()
{
	// 무기 mesh 생성
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
		
	UE_LOG(LogTemp, Warning, TEXT("Moving"));
		
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
	UE_LOG(LogTemp, Warning, TEXT("Move Value: %s"), *MovementVector.ToString());
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
	UE_LOG(LogTemp, Warning, TEXT("Jump"));
}

void APlayerCharacter::DoJumpStop()
{
	StopJumping();
	UE_LOG(LogTemp, Warning, TEXT("JumpStop"));
}

void APlayerCharacter::DoDash(const FInputActionValue& Value)
{
	if (MoveComp) MoveComp->MaxWalkSpeed = DashSpeed;
	UE_LOG(LogTemp, Warning, TEXT("Dash Start"));
}

void APlayerCharacter::DoDashStop(const FInputActionValue& Value)
{
	if (MoveComp) MoveComp->MaxWalkSpeed = 600.f; // 임시로 600 설정
	UE_LOG(LogTemp, Warning, TEXT("Dash Stop"));
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
	bIsHeavyAttack = false;
	IAttacker::Execute_CheckCombo(this);
	
	if (LightAttackMontage)
	{
		PlayAnimMontage(LightAttackMontage);
	}
	UE_LOG(LogTemp, Warning, TEXT("LightAttack Montage Played"));
}

void APlayerCharacter::DoHeavyAttack(const FInputActionValue& Value)
{
	bIsHeavyAttack = true;
	IAttacker::Execute_CheckCombo(this);
	
	if (HeavyAttackMontage)
	{
		PlayAnimMontage(HeavyAttackMontage);
	}
		UE_LOG(LogTemp, Warning, TEXT("HeavyAttack Montage Played"));
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
