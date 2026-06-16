// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"

struct FItemData;
class ACharacter;
class UCharacterMovementComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UPlayerInteractionComponent;
class UEquipmentComponent;
class UQuestComponent;
struct FQuestInstance;
class UStoreComponent;
class UPlayerHUDWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDead);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Buff",meta = (AllowPrivateAccess = "true"))
	bool bIsCastingBattleBuff = false;
	
	FString NickName;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Component")
	TObjectPtr<UQuestComponent> QuestComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Component")
	TObjectPtr<class UPlayerSkillComponent> SkillComponent;
	
	//===============================================================================================
	// 이동 관련
	//===============================================================================================
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MoveComp;
	
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
	// Buff 매핑
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<class UInputAction> BuffAction;
	// Guard 매핑
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> GuardAction;
	
	//===============================================================================================
	// 자원 소모
	//===============================================================================================
	UPROPERTY()
	bool bIsDashing = false;
	//===============================================================================================
	// 인터렉션 관련 
	//===============================================================================================
	FTimerHandle InteractionCheckTimerHandle;
	//===============================================================================================
	// 피격 관련 함수
	//===============================================================================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit|Feedback")
	TSubclassOf<class UCameraShakeBase> PlayerHitCameraShake;
	//===============================================================================================
	// 버프 관련
	//===============================================================================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Buff|Animation")
	TObjectPtr<UAnimMontage> BattleBuffMontage;

	UPROPERTY(EditDefaultsOnly, Category="Buff")
	float BuffDuration = 8.f;

	UPROPERTY(EditDefaultsOnly, Category="Buff")
	float BattleBuffCooldown = 15.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Buff")
	float BuffMPCost = 20.f;

	UPROPERTY(EditDefaultsOnly, Category="Buff")
	float BuffMoveSpeedMultiplier = 1.2f;

	UPROPERTY(EditDefaultsOnly, Category="Buff")
	float BattleBuffAttackBonus = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Buff")
	float BattleBuffDefenseBonus = 5.f;

	UPROPERTY(BlueprintReadOnly, Category="Buff")
	bool bIsBattleBuffActive = false;
	
	UPROPERTY(BlueprintReadOnly, Category="Buff")
	bool bIsBattleBuffOnCooldown = false;
	
	FTimerHandle BattleBuffCooldownTimerHandle;
	FTimerHandle BattleBuffTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category="FX|Buff")
	TObjectPtr<class UNiagaraSystem> BattleBuffFX;

	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> BattleBuffFXComponent;

	UPROPERTY(EditDefaultsOnly, Category="FX|Buff")
	FName BattleBuffFXSocketName = TEXT("None");
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputAction> TestSkillAction;

	UFUNCTION()
	void TestUnlockSkill();
	
	//===============================================================================================
	// 위잿 관련 
	//===============================================================================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<class UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<class UPlayerHUDWidget> PlayerHUDWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	bool bIsInventoryOpen = false;
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerDead OnPlayerDead;

	//===============================================================================================
	// 퀵슬롯
	//===============================================================================================
	UPROPERTY()
	TArray<FName> QuickSlotItemRowNames;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|QuickSlot")
	TObjectPtr<class UInputAction> QuickSlot1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|QuickSlot")
	TObjectPtr<class UInputAction> QuickSlot2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|QuickSlot")
	TObjectPtr<class UInputAction> QuickSlot3Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|QuickSlot")
	TObjectPtr<class UInputAction> QuickSlot4Action;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|QuickSlot")
	TObjectPtr<class UInputAction> QuickSlot5Action;
	
	// ========================================================
	// Wave Knockback
	// ========================================================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Knockback|Wave")
	TObjectPtr<UAnimMontage> WaveKnockbackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Knockback|Wave")
	FName WaveKnockbackStartSection = TEXT("Start");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Knockback|Wave")
	FName WaveKnockbackEndSection = TEXT("Weak");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Knockback|Wave")
	float WaveKnockbackDuration = 0.22f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Knockback|Wave")
	float WaveKnockbackDistance = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Knockback|Wave")
	float WaveKnockbackUpPower = 180.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Knockback|Wave")
	bool bIsWaveKnockback = false;

	FTimerHandle WaveKnockbackTimerHandle;

	FVector WaveKnockbackDirection = FVector::ZeroVector;
	float WaveKnockbackElapsedTime = 0.f;
protected:
	//===============================================================================================
	// 로드 저장
	//===============================================================================================
	UFUNCTION(BlueprintCallable, Category = "Save")
	void LoadPlayerData();

	UFUNCTION(BlueprintCallable, Category = "Save")
	void SavePlayerData();

	//===============================================================================================
	// 인터렉션 관련 
	//===============================================================================================
	void UpdateInteractionTarget();
	
	//===============================================================================================
	// 위잿 관련 
	//===============================================================================================
	
	void CreatePlayerHUD();
	//===============================================================================================
	// 퀵슬롯
	//===============================================================================================
	void UseQuickSlot1();
	void UseQuickSlot2();
	void UseQuickSlot3();
	void UseQuickSlot4();
	void UseQuickSlot5();
	
	//===============================================================================================
	// 사망처리
	//===============================================================================================
	virtual void OnDead() override;
	
	// ========================================================
	// Wave Knockback
	// ========================================================
	void UpdateWaveKnockback();
	void FinishWaveKnockback();
	
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	//===============================================================================================
	// 이동관련
	//===============================================================================================
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	
	UFUNCTION()
	void DoDash();
	UFUNCTION()
	void DoDashStop();
	
	UFUNCTION()
	void Interaction();
	UFUNCTION()
	void ToggleInventory();
	void CloseInventory();
	
	UFUNCTION(BlueprintCallable, Category="Movement")
	void RefreshMoveSpeed();
	//===============================================================================================
	// 아이템 관련 함수
	//===============================================================================================
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseConsumableItem(const FItemData& ItemData);
	
	//===============================================================================================
	// 피격 관련 함수
	//===============================================================================================
	virtual void NotifyDamage_Implementation(const FVector& DamageLocation, AActor* DamageSource) override;
	//===============================================================================================
	// 공격 관련 함수
	//===============================================================================================
	virtual void CheckCombo_Implementation() override;
	UFUNCTION()
	void DoChargedAttack();
	UFUNCTION()
	void DoChargeRelease();
	UFUNCTION()
	void DoLightAttack(const FInputActionValue& Value);
	UFUNCTION()
	void DoHeavyAttack(const FInputActionValue& Value);
	
	//===============================================================================================
	// 스킬 관련
	//===============================================================================================
	UFUNCTION(BlueprintCallable, Category="Buff")
	void ApplyBattleBuff();
	UFUNCTION()
	void DoBattleBuff();
	UFUNCTION()
	void EndBattleBuff();
	UFUNCTION()
	void EndBattleBuffCooldown();
	
	void StartBattleBuffFX();
    void StopBattleBuffFX();
	
	UFUNCTION(BlueprintCallable, Category="Buff")
	bool CanUseBattleBuff() const;
	UFUNCTION(BlueprintPure, Category = "Battle Buff")
	bool IsCastingBattleBuff() const;

	UFUNCTION(BlueprintCallable, Category = "Battle Buff")
	void FinishBattleBuffCasting();
	//===============================================================================================
	// 가드 관련
	//===============================================================================================
	UFUNCTION()
	void StartGuard();
	UFUNCTION()
	void EndGuard();

	//===============================================================================================
	// UI 관련
	//===============================================================================================
	void ShowBossHP(class AEnemyBase* Boss);
	void HideBossHP();
	UFUNCTION(BlueprintCallable, Category="UI")
	void SetUIMode(bool bEnableUI);
	
	//===============================================================================================
	// 퀵슬롯
	//===============================================================================================
	void TryAutoRegisterQuickSlot(FName ItemRowName);
	void RefreshQuickSlotUI(int32 QuickSlotIndex);
	void UseQuickSlot(int32 QuickSlotIndex);
	void RefreshQuickSlotByItem(FName ItemRowName);
	
	//===============================================================================================
	// 스토어
	//===============================================================================================
	void OpenStoreUI(UStoreComponent* InStoreComponent);
	void CloseStoreUI();
	
	//===============================================================================================
	// 저장
	//===============================================================================================
	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveGamePlay();
	// ========================================================
	// Wave Knockback
	// ========================================================
	void ApplyWaveKnockbackFromLocation(const FVector& SourceLocation);
	UFUNCTION(BlueprintCallable)
	void FinishWaveKnockbackAnimation();
	//===============================================================================================
	// Getter
	//===============================================================================================
	UGoldComponent* GetGoldComponent() const;
	UInventoryComponent* GetInventoryComponent() const;
	UPlayerInteractionComponent* GetInteractionComponent() const;
	UEquipmentComponent* GetEquipmentComponent() const;
	UQuestComponent* GetQuestComponent() const;
	UPlayerSkillComponent* GetSkillComponent() const;
	UFUNCTION(BlueprintPure, Category = "Quest")
	UPlayerHUDWidget* GetPlayerHUDWidget() const;
	FString GetNickName() const;
};
