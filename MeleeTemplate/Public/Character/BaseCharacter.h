// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackAsset.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum ECharacterState
{
	None	UMETA(DisplayName = "NONE"),
	Idle	UMETA(DisplayName = "Idle/Jog"),
	Attacking UMETA(DisplayName = "Attacking"),
	Dodging UMETA (DisplayName = "Dodging"),
	Hurt	UMETA(DisplayName = "Taking Damage")
};

UENUM(BlueprintType)
enum EPlayerStats
{
	HP	UMETA(DisplayName="Health"),
	MAXHP UMETA(DisplayName="Max HP"),
	MP UMETA(DisplayName="Mana"),
	STR UMETA(DisplayName="Strength"),
	DEF UMETA(DisplayName = "Defense"),
	MAG UMETA(DisplayName="Power"),
	RES	UMETA(DisplayName="Resistence"),
	AGI UMETA(DisplayName = "Agility"),
	CON	UMETA(DisplayName="Constitution"),
	WIS UMETA(DisplayName="Wisdom"),
	SPD UMETA(DisplayName="Speed"),
};

class UMotionWarpingComponent;
class ULockOnComponent;
class AWeaponBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDodgeDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackDelegate);

UCLASS()	
class MELEETEMPLATE_API ABaseCharacter : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()



	void Knockback(UAttackAsset* IncomingAttack, ACharacter* AttackingCharacter);
	void CalculateDamage(UAttackAsset* IncomingAttack, ACharacter* AttackingCharacter);
	void DetermineHitReactionAnimation(UAttackAsset* IncomingAttack);
	void SpawnHitParticle(UAttackAsset* IncomingAttack);

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = true))
	UAnimMontage* LeftLightHitAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = true))
	UAnimMontage* RightLightHitAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = true))
	UAnimMontage* DownToUpLightHitAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = true))
	UAnimMontage* ThrustLightHitAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = true))
	UAnimMontage* HeavyAttackHitReaction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = true))
	UParticleSystem* DefaultHitParticle;

	FTimerHandle HitStunHandle;
	

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnDodgeDelegate OnDodge;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnAttackDelegate OnAttack;

	UFUNCTION(BlueprintNativeEvent)
	void TryAttack();

	UFUNCTION(BlueprintImplementableEvent)
	void HitCameraShake();


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimMontage* AirHitReaction;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* FixedHitbox;
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsDead;

	

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* ProjectileSpawnLocation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<TEnumAsByte<EPlayerStats>, float> CharacterStats;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<ECharacterState> CharacterState = Idle;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AWeaponBase* EquippedWeapon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UCombatComponent* CombatComponent;


protected:

	//Interface
	virtual void FixedHitbox_Implementation() override;
	virtual void TryAttack_Implementation();
	virtual void HealSkill_Implementation() override;
	virtual void HitboxInit_Implementation() override;
	virtual void HitboxSweep_Implementation() override;
	virtual void HitboxEnd_Implementation() override;

	virtual void SetAndStartAttack();

	UPROPERTY(BlueprintReadWrite)
	FTimerHandle SkillSlot1TimerHandle;
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle SkillSlot2TimerHandle;
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle SkillSlot3TimerHandle;
	
	UFUNCTION(BlueprintImplementableEvent)
	void DamageTaken(float DamageAmount);

	UFUNCTION(BlueprintCallable)
	void ModifyHealth(float ChangeAmount);
	
	UFUNCTION(BlueprintImplementableEvent)
	void PreHit();
	bool IsCharacterDead();
	void RotateTowardsAttacker(ACharacter* AttackingCharacter);
	UFUNCTION(BlueprintNativeEvent)
	void ApplyHitEffects(UAttackAsset* IncomingAttack);
	virtual void OnDeath(ACharacter* AttackingCharacter);
	virtual void HitReaction_Implementation(UAttackAsset* IncomingAttack, ACharacter* AttackingCharacter) override;
	virtual void AOEAttack_Implementation() override;
	virtual void ShootProjectile_Implementation() override;
	

	UFUNCTION(BlueprintCallable)
	virtual void StartAttack(UAnimMontage* AttackAnimation);


	
	virtual void LightAttackHitReaction(TEnumAsByte<EAttackDirectionReaction> AttackDirection);
	virtual void EndAttack_Implementation() override;


	UFUNCTION(BlueprintNativeEvent)
	void DeathReaction();
	
	FTimerDelegate HitStunDelegate;

	void HitStunStart_Implementation(float HitStunDuration);

	UFUNCTION()
	void HitStunEnd();
	void BeginPlay();

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
