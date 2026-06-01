// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class ACharacter;
class UCharacterMovementComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UPlayerInteractionComponent;
class UEquipmentComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//===============================================================================================
	// 무기 관련
	//===============================================================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<class USkeletalMeshComponent> WeaponMesh;
	//===============================================================================================
	// 컴포넌트
	//===============================================================================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UInventoryComponent> InventoryComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UGoldComponent> GoldComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UPlayerInteractionComponent> InteractionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<UEquipmentComponent> EquipmentComponent;
	
	//===============================================================================================
	// 이동 관련
	//===============================================================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputMappingContext> InputMappingContext;
	//이동 매핑
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> MoveAction;
	//시선 매핑
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> LookAction;
	// 인터렉션 매핑
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> InteractionAction;
	// 인벤토리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> InventoryAction;
	// 대쉬 매핑
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> DashAction;
	// Charge Attack 매핑
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> ChargeAttackAction;
	// Light Attack 매핑
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> LightAttackAction;
	// Heavy Attack 매핑
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> HeavyAttackAction;
	//===============================================================================================
	// 자원 소모
	//===============================================================================================
	UPROPERTY()
	bool bIsDashing = false;
	//===============================================================================================
	// 공격 관련 함수
	//===============================================================================================
	virtual void CheckCombo_Implementation() override;
	
	void DoChargedAttack();
	void DoChargeRelease();
	void DoLightAttack(const FInputActionValue& Value);
	void DoHeavyAttack(const FInputActionValue& Value);
	//===============================================================================================
	// 인터렉션 관련 
	//===============================================================================================
	FTimerHandle InteractionCheckTimerHandle;
	
protected:
	// 캐릭터 이동 컴포넌트 참조
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MoveComp;
	//===============================================================================================
	// 인터렉션 관련 
	//===============================================================================================
	void UpdateInteractionTarget();
	
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	//===============================================================================================
	// 이동관련
	//===============================================================================================
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	void DoDash();
	void DoDashStop();
	
	void Interaction();
	void ToggleInventory();
	
	UGoldComponent* GetGoldComponent() const;
	UInventoryComponent* GetInventoryComponent() const;
	UPlayerInteractionComponent* GetInteractionComponent() const;
	UEquipmentComponent* GetEquipmentComponent() const;
};
