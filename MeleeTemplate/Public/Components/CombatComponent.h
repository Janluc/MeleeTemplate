// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class UAttackAsset;
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
	UAttackAsset* ChooseAttack(int32& Counter, TArray<UAttackAsset*> ComboAttacks);
	UFUNCTION(BlueprintCallable)
	virtual UAttackAsset* ChooseComboAttack(TArray<UAttackAsset*> ComboAttacks);
	UFUNCTION(BlueprintCallable)
	virtual UAttackAsset* TrySkill(UAttackAsset* IncomingSkill);
	virtual void CheckComboCounter(TArray<UAttackAsset*> AttackList);
	TArray<FHitResult> AOESphereTrace();

	virtual void FixedHitboxAttack(FVector BoxExtent, FVector BoxLocation);
	virtual TArray<FHitResult> FixedHitboxTrace(FVector BoxExtent, FVector BoxLocation);

	virtual void AOEAttack();

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DashAttackDistance = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinDashAttackDistance = 100;

	virtual void SetHitboxLocation();
	virtual void SweepTraces();
	virtual void ResetCombat();

	virtual void ShootProjectile();

	void HitSweepedEnemies(TArray<FHitResult> HitActors);

	virtual TArray<FHitResult> TraceAndProvideHit();

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHitboxDebug;
#endif
	UPROPERTY()
	UAttackAsset* CurrentAttack;

	UPROPERTY()
	TArray<AActor*> HitEnemies;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
		
};
