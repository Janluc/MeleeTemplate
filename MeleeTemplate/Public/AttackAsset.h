// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AttackAsset.generated.h"

class UNiagaraSystem;
UENUM(BlueprintType)
enum EAttackCategory
{
	Basic UMETA(DisplayName = "Basic Attack"),
	Skill UMETA(DisplayName = "Skill")
};
UENUM(BlueprintType)
enum EAttackType
{
	Light UMETA(DisplayName="Light Attack"),
	Medium UMETA(DisplayName="Medium Attack"),
	Heavy UMETA(DisplayName="Heavy Attack"),
	Flurry UMETA(DisplayName = "Flurry")
};

UENUM(BlueprintType)
enum EAttackDirectionReaction
{
	Right UMETA(DisplayName="Right Dir"),
	Left UMETA(DisplayName="Left Dir"),
	DownToUp UMETA(DisplayName = "Down To Up Dir"),
	Thrust UMETA(DisplayName = "Thrust Direction")
};
/**
 * 
 */
class AProjectile;
/**
 * 
 */
UCLASS()
class MELEETEMPLATE_API UAttackAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAttackCategory> AttackCategory = Basic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsProjectile = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "AttackCategory == EAttackCategory::Skill", EditConditionHides))
	bool SkillHeals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "SkillHeals == true", EditConditionHides))
	float HealAmount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasHyperArmor = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "AttackCategory == EAttackCategory::Skill && SkillHeals == false", EditConditionHides))
	UParticleSystem* HitEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "AttackCategory == EAttackCategory::Skill && SkillHeals == false", EditConditionHides))
	UNiagaraSystem* HitEffectNiagara;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "AttackCategory == EAttackCategory::Skill", EditConditionHides))
	float Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "AttackCategory == EAttackCategory::Skill", EditConditionHides))
	UTexture2D* AbilityIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "IsProjectile == true", EditConditionHides))
	TSubclassOf<AProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "SkillHeals == false", EditConditionHides))
	float AttackModifier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "SkillHeals == false", EditConditionHides))
	TEnumAsByte<EAttackType> AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "SkillHeals == false", EditConditionHides))
	TEnumAsByte<EAttackDirectionReaction> AttackDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "SkillHeals == false", EditConditionHides))
	bool bIsLauncher = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "SkillHeals == false", EditConditionHides))
	bool bIsAirAttack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "SkillHeals == false", EditConditionHides))
	float HitStunDuration = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "SkillHeals == false", EditConditionHides))
	float KnockbackAmount = 500.0f;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Attacks", GetFName());
	}
	
};
