// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyFSMTypes.h"
#include "FSMStateComponent.h"
#include "JumpAttackStateComponent.generated.h"

class UAnimMontage;

UENUM()
enum class EJumpAttackStep : uint8
{
	None,
	JumpStart,
	Flying,
	Attack
};
UCLASS()
class YESTERDAYKINGDOM_API UJumpAttackStateComponent : public UFSMStateComponent
{
	GENERATED_BODY()
private:
	float ElapsedTime = 0.f;

	bool bDidJumpToPlayer = false;
	bool bStartedAttack = false;

	EJumpAttackStep CurrentStep = EJumpAttackStep::None;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|Next")
	EEnemyFSMStateType NextState = EEnemyFSMStateType::Cooldown;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|JumpAttack")
	TObjectPtr<UAnimMontage> JumpAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|JumpAttack|Section")
	FName JumpStartSectionName = TEXT("JumpStart");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|JumpAttack|Section")
	FName SpinMoveSectionName = TEXT("SpinMove");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|JumpAttack|Section")
	FName AttackSectionName = TEXT("Attack");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|JumpAttack|Move")
	float JumpUpPower = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|JumpAttack|Move")
	float JumpForwardPower = 1700.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|JumpAttack|Attack")
	float AttackTriggerDistance = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|JumpAttack|Attack")
	float AttackTriggerHeight = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM|JumpAttack")
	float MaxJumpAttackTime = 3.0f;
	
private:
	void TryStartAttackSection();
	void StartAttackSection();
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void SetMontageRootMotionEnabled(bool bEnabled);
public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate(float DeltaTime) override;
	virtual void OnStateExit() override;

	// AnimNotify 1개만 사용
	void NotifyJumpToPlayer();
	
};
