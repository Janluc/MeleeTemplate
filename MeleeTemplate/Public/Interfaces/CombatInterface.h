// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "CombatInterface.generated.h"

class UAttackAsset;
// This class does not need to be modified.
UINTERFACE()
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MELEETEMPLATE_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HitboxInit();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HitboxSweep();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HitboxEnd();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HitReaction(UAttackAsset* IncomingAttack, ACharacter* AttackingCharacter);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InputBufferHandle();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EndAttack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AOEAttack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ShootProjectile();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HealSkill();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnEnemyHit();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void FixedHitbox();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetLastHitCharacter(ACharacter* CharacterHit);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HitStunStart(float HitStunDuration);
};



