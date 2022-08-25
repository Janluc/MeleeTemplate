// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum ECharacterState
{
	None	UMETA(DisplayName = "NONE"),
	Idle	UMETA(DisplayName = "Idle/Jog"),
	Attacking UMETA(DisplayName = "Attacking"),
	Dodging UMETA (DisplayName = "Dodging")
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
UCLASS()	
class MELEETEMPLATE_API ABaseCharacter : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

	
public:
	// Sets default values for this character's properties
	ABaseCharacter();
	
	FTimerHandle HitStunHandle;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* LeftLightHitAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* RightLightHitAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* DownToUpLightHitAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ThrustLightHitAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* HeavyAttackHitReaction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* DefaultHitParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULockOnComponent* LockOnComponent;

	UFUNCTION(BlueprintImplementableEvent)
	void DamageTaken(float DamageAmount);

	virtual void HitboxInit_Implementation() override;
	virtual void HitboxSweep_Implementation() override;
	virtual void HitboxEnd_Implementation() override;
	virtual void HitReaction_Implementation(FAttack IncomingAttack, ACharacter* AttackingCharacter) override;
	virtual void AOEAttack_Implementation() override;
	virtual void ShootProjectile_Implementation() override;

	virtual void StartAttack(UAnimMontage* AttackAnimation);
	void CalculateDamage(FAttack IncomingAttack, ACharacter* AttackingCharacter);
	void DetermineHitReactionAnimation(FAttack IncomingAttack, ACharacter* AttackingCharacter);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMotionWarpingComponent* MotionWarping;
	
	UFUNCTION(BlueprintNativeEvent)
	void BasicAttackHitReaction(FAttack IncomingAttack, ACharacter* AttackingCharacter);

	UFUNCTION(BlueprintNativeEvent)
	void SkillHitReaction(FAttack IncomingAttack, ACharacter* AttackingCharacter);


	virtual void LightAttackHitReaction(FAttack IncomingAttack, ACharacter* AttackingCharacter);
	virtual void InputBufferHandle_Implementation() override;
	virtual void EndAttack_Implementation() override;

	UFUNCTION()
	void HitStunStart(FAttack IncomingAttack, ACharacter* AttackingCharacter);

	UFUNCTION()
	void HitStunEnd(ACharacter* AttackingCharacter);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
