// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInteractionComponent.h"

#include "BaseCharacter.h"
#include "InteractionObject.h"
#include "Engine/OverlapResult.h"

// Sets default values for this component's properties
UPlayerInteractionComponent::UPlayerInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}




// Called when the game starts
void UPlayerInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
	
}
AActor* UPlayerInteractionComponent::FindBestInteractTarget() const
{
	if (!OwnerCharacter) return nullptr;
	TArray<FOverlapResult> OverlapResults;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);

	const FVector Origin = OwnerCharacter->GetActorLocation();
	
	const bool bHit = GetWorld()->OverlapMultiByChannel(OverlapResults, Origin, FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeSphere(InteractionRadius), Params);
	if (!bHit) return nullptr;
	
	AActor* BestTarget = nullptr;
	float BestScore = -9999.f;
	const FVector Forward = OwnerCharacter->GetActorForwardVector();
	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* Target = Result.GetActor();
		if (!Target || Target == OwnerCharacter) continue;
		if (!Target->GetClass()->ImplementsInterface(UInteractionObject::StaticClass())) continue;
		if (!IInteractionObject::Execute_CanInteract(Target, OwnerCharacter)) continue;
		
		FVector ToTarget = Target->GetActorLocation() - Origin;
		ToTarget.Z = 0.f;
		const float Distance = ToTarget.Size();
		if (Distance <= KINDA_SMALL_NUMBER) continue;
		
		const FVector Direction = ToTarget.GetSafeNormal();
		const float Dot = FVector::DotProduct(Forward, Direction);
		if (Dot < InteractionDotThreshold) continue;
		
		const float Score = Dot * 2.f - Distance / InteractionRadius;
		
		if (Score > BestScore)
		{
			BestScore = Score;
			BestTarget = Target;
		}
	}
	return BestTarget;
}

void UPlayerInteractionComponent::SetCurrentInteractTarget(AActor* NewTarget)
{
	if (CurrentInteractTarget == NewTarget) return;
	CurrentInteractTarget = NewTarget;
	OnInteractTargetChanged.Broadcast(CurrentInteractTarget);
	if (CurrentInteractTarget && CurrentInteractTarget.GetClass()->ImplementsInterface(UInteractionObject::StaticClass()))
	{
		const FText InteractionText = IInteractionObject::Execute_GetInteractionText(CurrentInteractTarget);
		OnInteractionTextChanged.Broadcast(InteractionText);
	}
	else OnInteractionTextChanged.Broadcast(FText::GetEmpty());
}

void UPlayerInteractionComponent::UpdateInteractTarget()
{
	AActor* NewTarget = FindBestInteractTarget();
	SetCurrentInteractTarget(NewTarget);
}

void UPlayerInteractionComponent::Interact()
{
	if (!CurrentInteractTarget) return;
	if (!CurrentInteractTarget.GetClass()->ImplementsInterface(UInteractionObject::StaticClass())) return;
	if (!IInteractionObject::Execute_CanInteract(CurrentInteractTarget, OwnerCharacter))
	{
		SetCurrentInteractTarget(nullptr);
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("%s interaction"), *CurrentInteractTarget.GetName());
	IInteractionObject::Execute_Interact(CurrentInteractTarget, OwnerCharacter);
}

AActor* UPlayerInteractionComponent::GetCurrentInteractTarget() const
{
	return CurrentInteractTarget;
}