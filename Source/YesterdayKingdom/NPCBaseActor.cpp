// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCBaseActor.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ANPCBaseActor::ANPCBaseActor()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// ACharacter는 기본 CapsuleComponent가 Root임
	GetCapsuleComponent()->SetCapsuleRadius(42.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);

	// NPC는 전투/이동이 없으면 이동 관련 Tick 최소화
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->DisableMovement();
		Movement->SetComponentTickEnabled(false);
	}

	// 기본 Mesh 사용
	GetMesh()->SetupAttachment(GetCapsuleComponent());
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -96.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// 상호작용 감지는 기본 Capsule을 써도 되고,
	// 별도 InteractionCollision을 추가해도 됨.
	InteractionCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(GetCapsuleComponent());

	InteractionCollision->SetCapsuleRadius(120.f);
	InteractionCollision->SetCapsuleHalfHeight(120.f);
	InteractionCollision->SetRelativeLocation(FVector::ZeroVector);

	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Called when the game starts or when spawned
void ANPCBaseActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANPCBaseActor::Interact_Implementation(AActor* Interactor)
{
	IInteractionObject::Interact_Implementation(Interactor);
	UE_LOG(LogTemp, Log, TEXT("[NPCBaseActor] Interact : %s"), *GetName());
}

bool ANPCBaseActor::CanInteract_Implementation(AActor* Interactor) const
{
	return bCanInteract && Interactor != nullptr;
}

FText ANPCBaseActor::GetInteractionText_Implementation() const
{
	return IInteractionObject::GetInteractionText_Implementation();
}

FText ANPCBaseActor::GetNPCName() const
{
	return NPCName;
}

