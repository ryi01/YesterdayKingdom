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

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None	UMETA(DisplayName = "None"),
	Light	UMETA(DisplayName = "Light"),
	Heavy	UMETA(DisplayName = "Heavy"),
	Combo	UMETA(DisplayName = "Combo"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	APlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputMappingContext> InputMappingContext;
	//이동 매핑
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> MoveAction;
	//시선 매핑
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> LookAction;
	//점프 매핑
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> JumpAction;
	
	// --------------------------------------------------------------------------------------------- //
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float JumpZPower = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashSpeed = 900.f;
	
	// --------------------------------------------------------------------------------------------- //
	// 공격
	// --------------------------------------------------------------------------------------------- //
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> AttackAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> LightAttackAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> HeavyAttackAction;
	
	virtual void CheckCombo_Implementation() override;
	
	void DoComboAttack(const FInputActionValue& Value);
	void DoLightAttack(const FInputActionValue& Value);
	void DoHeavyAttack(const FInputActionValue& Value);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsHeavyAttack = false;
	
	// --------------------------------------------------------------------------------------------- //
	// --------------------------------------------------------------------------------------------- //
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<UAnimMontage*> ComboMontages;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CheckCombo(EAttackType AttackType);
	
protected:
	virtual void BeginPlay() override;
	
	// 캐릭터 이동 컴포넌트 참조
	UPROPERTY()
	UCharacterMovementComponent* MoveComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Pawn")
	bool bIsAttacking = false;
	
	UPROPERTY()
	int32 AttackIndex = 0;
	

	
public:
	// --------------------------------------------------------------------------------------------- //
	// 기본 이동
	// --------------------------------------------------------------------------------------------- //
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void DoJump();
	void Dash(const FInputActionValue& Value);
	void DoJumpStop();
	void DashStop(const FInputActionValue& Value);
	
};
