// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "BaseStatComponent.h"
#include "CombatBaseComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// ========================================================
	// CharacterMovement 기본 값
	// ========================================================
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->bOrientRotationToMovement = false;
		MovementComponent->bUseControllerDesiredRotation = false;
		
		MovementComponent->MaxWalkSpeed = 600.f;
		MovementComponent->RotationRate = FRotator(0.f, 720.f, 0.f);
		
		MovementComponent->JumpZVelocity = 0.f;
		MovementComponent->AirControl = 0.f;

		MovementComponent->BrakingDecelerationWalking = 2048.f;
		MovementComponent->GravityScale = 1.f;
	}
	// ========================================================
	// 캡슐 기본값
	// ========================================================
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (Capsule)
	{
		Capsule->InitCapsuleSize(42.f, 96.f);

		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Capsule->SetCollisionObjectType(ECC_Pawn);

		Capsule->SetCollisionResponseToAllChannels(ECR_Block);
		Capsule->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		Capsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}
	// ========================================================
	// Mesh 기본 값
	// ========================================================
	if (USkeletalMeshComponent* CharacterMesh = GetMesh())
	{
		CharacterMesh->SetupAttachment(Capsule);

		CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CharacterMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		
		CharacterMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		CharacterMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}
	
	// ========================================================
	// Components
	// ========================================================
	StatComponent = CreateDefaultSubobject<UBaseStatComponent>(TEXT("StatComponent"));
	CombatBaseComponent = CreateDefaultSubobject<UCombatBaseComponent>(TEXT("CombatComponent"));
	
	// ========================================================
	// 무기 루트
	// ========================================================
	WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
	WeaponRoot->SetupAttachment(GetMesh());
}

void ABaseCharacter::InitializeWeaponRoot()
{
	if (!GetMesh() || WeaponSocketName.IsNone()) return;
	if (GetMesh()->DoesSocketExist(WeaponSocketName))
	{
		WeaponRoot->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s : WeaponSocket Missing"), *GetName());
	}
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCharacter::ApplyDamage_Implementation(float Damage, AActor* DamageCauser, const FVector& DamageLocation,
	const FVector& DamageImpulse)
{
	IDamagable::ApplyDamage_Implementation(Damage, DamageCauser, DamageLocation, DamageImpulse);
	if (!StatComponent) return;
	// 체력 깍는 함수
	StatComponent->ApplyDamage(Damage);
	// 데미지 확인
	NotifyDamage(DamageLocation, DamageCauser);
	// 죽었는지 확인
	if (StatComponent->IsDead()) HandleDeath();
}

void ABaseCharacter::NotifyDamage_Implementation(const FVector& DamageLocation, AActor* DamageSource)
{
	IDamagable::NotifyDamage_Implementation(DamageLocation, DamageSource);
}

void ABaseCharacter::HandleDeath_Implementation()
{
	IDamagable::HandleDeath_Implementation();
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->DisableMovement();
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ABaseCharacter::BeginAttackTrace_Implementation()
{
	IAttacker::BeginAttackTrace_Implementation();
	if (CombatBaseComponent) CombatBaseComponent->BeginAttackTrace();
}

void ABaseCharacter::DoAttackTrace_Implementation()
{
	IAttacker::DoAttackTrace_Implementation();
	if (CombatBaseComponent) CombatBaseComponent->DoAttackTrace();
}

void ABaseCharacter::EndAttackTrace_Implementation()
{
	IAttacker::EndAttackTrace_Implementation();
	if (CombatBaseComponent) CombatBaseComponent->EndAttackTrace();
}

void ABaseCharacter::CheckCombo_Implementation()
{
	IAttacker::CheckCombo_Implementation();
	if (CombatBaseComponent) CombatBaseComponent->CheckCombo();
}

void ABaseCharacter::ClearAttackAnimation_Implementation()
{
	if (CombatBaseComponent) CombatBaseComponent->ResetAttackState();
}

UBaseStatComponent* ABaseCharacter::GetStatComponent() const
{
	return StatComponent;
}

UCombatBaseComponent* ABaseCharacter::GetCombatComponent() const
{
	return CombatBaseComponent;
}

USceneComponent* ABaseCharacter::GetWeaponRoot() const
{
	return WeaponRoot;
}

