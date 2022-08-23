// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/Attack.h"
#include "Character/BaseCharacter.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEETEMPLATE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	bool bInputBuffer;

	UPROPERTY()
	ABaseCharacter* Owner;

	
	
	FVector HitboxPreviousLocation;

	
public:	
	// Sets default values for this component's properties
	UCombatComponent();

	int32 ComboCounter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AWeaponBase* OwnerWeapon;
	virtual void HandleAttackInput();
	void RotateOwnerToTarget();
	void ChooseAttack(int32& Counter);
	virtual void Attack();
	virtual void Skill(FAttack IncomingSkill);
	virtual void CheckComboCounter(TArray<FAttack> AttackList);

	virtual void AOEAttack();


	
	virtual void HandleSkillInput(FAttack IncomingSkill);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DashAttackDistance = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinDashAttackDistance = 100;

	virtual void SetHitboxLocation();
	virtual void SweepTraces();
	virtual void ResetCombat();

	virtual void DetermineComboExecution();

	void HitSweepedEnemies(TArray<FHitResult> HitActors);

	virtual TArray<FHitResult> TraceAndProvideHit();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHitboxDebug;

	FAttack CurrentAttack;

	UPROPERTY()
	TArray<AActor*> HitEnemies;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
