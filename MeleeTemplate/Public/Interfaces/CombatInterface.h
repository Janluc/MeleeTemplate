// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Structs/Attack.h"
#include "CombatInterface.generated.h"

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
	void HitReaction(FAttack IncomingAttack, ACharacter* AttackingCharacter);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InputBufferHandle();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EndAttack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AOEAttack();
};



