// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attack.generated.h"

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
USTRUCT(BlueprintType)
struct FAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAttackCategory> AttackCategory = Basic;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackModifier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAttackType> AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAttackDirectionReaction> AttackDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLauncher = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAirAttack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitStunDuration = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float KnockbackAmount = 500.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "AttackCategory == EAttackCategory::Skill", EditConditionHides))
	UParticleSystem* HitEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "AttackCategory == EAttackCategory::Skill", EditConditionHides))
	UNiagaraSystem* HitEffectNiagara;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "AttackCategory == EAttackCategory::Skill", EditConditionHides))
	float Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "AttackCategory == EAttackCategory::Skill", EditConditionHides))
	UTexture2D* AbilityIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "AttackCategory == EAttackCategory::Skill", EditConditionHides))
	bool SkillHeals;
	
};
