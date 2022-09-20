// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"


UCLASS(config=Game)
class APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY()
	ACharacter* LastCharacterHit;
	FTimerHandle ShootTimerHandle;
	FTimerDynamicDelegate ShootDelegate;

	virtual void ShootLoop();


	virtual void TryAttack_Implementation() override;
	virtual void ReleaseAttack();
	void ActivateSkill(UAttackAsset* SkillSlot);

	UFUNCTION(BlueprintCallable)
	virtual void TrySkillSlot1();

	UFUNCTION(BlueprintCallable)
	virtual void TrySkillSlot2();

	UFUNCTION(BlueprintCallable)
	virtual void TrySkillSlot3();


	virtual void OnEnemyHit_Implementation() override;
	
public:
	APlayerCharacter();

	UPROPERTY(BlueprintReadWrite)
	bool bCanRotateToTarget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAttackAsset* ShootAttack;
	UPROPERTY(BlueprintReadWrite)
	bool bSoftLockOn = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAttackAsset*> AirAttacks;

	bool bStickyEnemy;
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* AirSticky;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAttackAsset* SkillSlot1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAttackAsset* SkillSlot2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAttackAsset* SkillSlot3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = true))
	ULockOnComponent* LockOnComponent;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	virtual void Jump() override;
	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	virtual void BeginPlay() override;

	virtual void HitboxEnd_Implementation() override;
	virtual void InputBufferHandle_Implementation() override;
	bool CanAirAttack();
	virtual void SetAndStartAttack() override;
	virtual void StartAttack(UAnimMontage* AttackAnimation) override;
	virtual void EndAttack_Implementation() override;
	bool IsCharacterInDashAttackDistance();

	virtual void SetLastHitCharacter_Implementation(ACharacter* CharacterHit) override;
	
	


protected:
	
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMotionWarpingComponent* MotionWarping;

	
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

