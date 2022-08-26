// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/Attack.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ABaseCharacter;
class AWeaponBase;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEETEMPLATE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	

	UPROPERTY()
	ABaseCharacter* Owner;
	
	FVector HitboxPreviousLocation;

	
public:	
	// Sets default values for this component's properties
	UCombatComponent();

	bool bInputBuffer;

	int32 ComboCounter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AWeaponBase* OwnerWeapon;
	void RotateOwnerToTarget();
	void ChooseAttack(int32& Counter, TArray<UAttackAsset*> ComboAttacks);
	UFUNCTION(BlueprintCallable)
	virtual void ComboAttack(TArray<UAttackAsset*> ComboAttacks);
	UFUNCTION(BlueprintCallable)
	virtual void Skill(UAttackAsset* IncomingSkill);
	virtual void CheckComboCounter(TArray<UAttackAsset*> AttackList);

	virtual void AOEAttack();


	
	virtual void HandleSkillInput(UAttackAsset* IncomingSkill);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DashAttackDistance = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinDashAttackDistance = 100;

	virtual void SetHitboxLocation();
	virtual void SweepTraces();
	virtual void ResetCombat();

	virtual void ShootProjectile();

	virtual void DetermineComboExecution(TArray<UAttackAsset*> ComboAttacks);

	void HitSweepedEnemies(TArray<FHitResult> HitActors);

	virtual TArray<FHitResult> TraceAndProvideHit();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHitboxDebug;

	UPROPERTY()
	UAttackAsset* CurrentAttack;

	UPROPERTY()
	TArray<AActor*> HitEnemies;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
