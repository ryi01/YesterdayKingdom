// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillTypes.h"
#include "PlayerSkillComponent.generated.h"


class UGoldComponent;
class UPlayerStatComponent;
class APlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillTreeChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YESTERDAYKINGDOM_API UPlayerSkillComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<UDataTable> SkillDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	TSet<FName> UnlockedSkillRowNames;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Skill|Unlock")
	bool bCanUseChargeAttack = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Skill|Unlock")
	bool bCanUseParry = false;
	
public:	
	// Sets default values for this component's properties
	UPlayerSkillComponent();
	UPROPERTY(BlueprintAssignable)
	FOnSkillTreeChanged OnSkillTreeChanged;
	
protected:
	const FSkillDataRow* FindSkillData(FName SkillRowName) const;

	bool HasRequiredSkill(const FSkillDataRow& SkillData) const;

	void ApplySkillEffect(const FSkillDataRow& SkillData);

public:	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Skill")
	bool TryUnlockSkill(FName SkillRowName);

	UFUNCTION(BlueprintPure, Category = "Skill")
	bool IsSkillUnlocked(FName SkillRowName) const;

	UFUNCTION(BlueprintPure, Category = "Skill")
	bool CanUnlockSkill(FName SkillRowName) const;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	bool GetSkillData(FName SkillRowName, FSkillDataRow& OutSkillData) const;

	UFUNCTION(BlueprintPure, Category = "Skill")
	const TSet<FName>& GetUnlockedSkillRowNames() const;
	
	UFUNCTION(BlueprintPure, Category="Skill|Unlock")
	bool CanUseChargeAttack() const;
	
	UFUNCTION(BlueprintPure, Category="Skill|Unlock")
	bool CanUseParry() const;
};
