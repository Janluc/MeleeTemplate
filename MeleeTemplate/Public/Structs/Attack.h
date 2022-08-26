// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackAsset.h"
#include "Attack.generated.h"

class UNiagaraSystem;

/**
 * 
 */
class AProjectile;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsProjectile = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "IsProjectile == true", EditConditionHides))
	TSubclassOf<AProjectile> ProjectileClass;
};
