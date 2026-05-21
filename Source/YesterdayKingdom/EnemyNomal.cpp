// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyNomal.h"
#include "EnemyDefinition.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

AEnemyNomal::AEnemyNomal()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// 매쉬 설정
	
	// 무기 생성
	
	// 기본적인 세팅
}

void AEnemyNomal::BeginAttackTrace_Implementation()
{
	//Super::BeginAttackTrace_Implementation();
	
}

void AEnemyNomal::ApplyDamage_Implementation(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse)
{
	
}

void AEnemyNomal::HandleDeath_Implementation()
{
	
}

void AEnemyNomal::NotifyDamage_Implementation(const FVector& DamageLocation, AActor* DamageSource)
{
	
}




