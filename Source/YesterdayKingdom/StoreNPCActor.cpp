// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreNPCActor.h"

#include "BaseCharacter.h"
#include "PlayerCharacter.h"
#include "StoreComponent.h"
#include "Kismet/GameplayStatics.h"

AStoreNPCActor::AStoreNPCActor()
{
	StoreComponent = CreateDefaultSubobject<UStoreComponent>(TEXT("StoreComponent"));

	InteractionText = FText::FromString(TEXT("상점 열기"));
}


void AStoreNPCActor::Interact_Implementation(AActor* Interactor)
{
	if (!IInteractionObject::Execute_CanInteract(this, Interactor))
	{
		return;
	}

	Super::Interact_Implementation(Interactor);

	if (!StoreComponent) return;

	UE_LOG(LogTemp, Log, TEXT("[StoreNPCActor] Open Store"));
	
	if (APlayerCharacter* Character = Cast<APlayerCharacter>(Interactor))
	{
		if (StoreOpenSound) UGameplayStatics::PlaySound2D(this, StoreOpenSound, StoreOpenSoundVolume);
		StoreComponent->DebugLogStoreItems();
		Character->OpenStoreUI(StoreComponent);
	}

}

FText AStoreNPCActor::GetInteractionText_Implementation() const
{
	return InteractionText;
}

UStoreComponent* AStoreNPCActor::GetStoreComponent() const
{
	return StoreComponent;
}
