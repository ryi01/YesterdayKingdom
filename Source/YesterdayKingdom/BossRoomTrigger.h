// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossRoomTrigger.generated.h"

UCLASS()
class YESTERDAYKINGDOM_API ABossRoomTrigger : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, Category="Boss")
	TObjectPtr<class ABaseCharacter> BossCharacter;
	
	UPROPERTY()
	bool bAlreadyTriggered = false;
	
	UPROPERTY()
	TObjectPtr<class ABossRoomEntrance> BossRoomEntrance;
	
	UPROPERTY()
	TObjectPtr<class AEnemyBase> Boss;
public:	
	// Sets default values for this actor's properties
	ABossRoomTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
public:
	void SetTriggerExtent(const FVector& NewExtent);
	void SetBossRoomEntrance(class ABossRoomEntrance* EntranceActor);
	void SetBoss(AEnemyBase* InBoss);
};
