// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"


void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterOwner = Cast<ACharacter>(GetOwner());
	if (CharacterOwner)
	{
		MoveComp = CharacterOwner->GetCharacterMovement();
		if (MoveComp)
		{
			MoveComp->bOrientRotationToMovement = true;
			MoveComp->RotationRate = FRotator(0.0f, RotateSpeed, 0.0f);
			MoveComp->MaxWalkSpeed = NormalSpeed;
			CharacterOwner->bUseControllerRotationYaw = false;
		}
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
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::StopJumping);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
	
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(-LookAxisVector.Y);
}

void APlayerCharacter::Jump(const FInputActionValue& Value)
{
	if (CharacterOwner) CharacterOwner->Jump();
}

void APlayerCharacter::JumpStop(const FInputActionValue& Value)
{
	if (CharacterOwner) CharacterOwner->StopJumping();
}

void APlayerCharacter::Dash(const FInputActionValue& Value)
{
	if (MoveComp) MoveComp->MaxWalkSpeed = DashSpeed;
}

void APlayerCharacter::DashStop(const FInputActionValue& Value)
{
	if (MoveComp) MoveComp->MaxWalkSpeed = NormalSpeed;
}

void APlayerCharacter::CheckCombo_Implementation()
{
	Super::CheckCombo_Implementation();
}

