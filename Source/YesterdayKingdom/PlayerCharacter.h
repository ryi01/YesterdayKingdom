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
	// 대쉬 매핑
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> DashAction;
	
	// Charge Attack 매핑
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> AttackAction;
	// Light Attack 매핑
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> LightAttackAction;
	// Heavy Attack 매핑
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> HeavyAttackAction;
	
	// --------------------------------------------------------------------------------------------- //
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float JumpZPower = 500.f;
	
	// --------------------------------------------------------------------------------------------- //
	// 공격
	// --------------------------------------------------------------------------------------------- //
	
	virtual void CheckCombo_Implementation() override;
	virtual void Charged_Implementation();
	
	void DoChargedAttack();
	void DoLightAttack(const FInputActionValue& Value);
	void DoHeavyAttack(const FInputActionValue& Value);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsHeavyAttack = false;
	
	// --------------------------------------------------------------------------------------------- //
	// --------------------------------------------------------------------------------------------- //
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<UAnimMontage*> ComboMontages;
	
protected:
	virtual void BeginPlay() override;
	
	// 캐릭터 이동 컴포넌트 참조
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MoveComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Pawn")
	bool bIsAttacking = false;
	
	UPROPERTY()
	int32 AttackIndex = 0;
	
	// --- 애니메이션 몽타주 변수 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LightAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* HeavyAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* ChargedAttackMontage;
	
public:
	// --------------------------------------------------------------------------------------------- //
	// 기본 이동
	// --------------------------------------------------------------------------------------------- //
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void DoJump();
	void DoDash(const FInputActionValue& Value);
	void DoJumpStop();
	void DoDashStop(const FInputActionValue& Value);
	
};
