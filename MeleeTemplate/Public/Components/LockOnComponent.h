// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEETEMPLATE_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AActor*> ActorsScanned;

	
public:	
	// Sets default values for this component's properties
	ULockOnComponent();

	UFUNCTION(BlueprintCallable)
	virtual void SetRightLeftInput(float AxisValue);

	UFUNCTION(BlueprintCallable)
	virtual void SetUpDownInput(float AxisValue);
	UFUNCTION(BlueprintCallable)
	void RotateOwnerToTarget();
	bool IsTargetRightOfOwner(float EnemyAngleRelativeToOwner);
	bool IsTargetLeftOfOwner(float EnemyAngleRelativeToOwner);
	void ScanRightLeft(FHitResult Hit, float EnemyAngleRelativeToOwner);
	bool IsTargetBehindOwner(float EnemyAngleRelativeToOwner);
	bool IsTargetInFrontOfOwner(float EnemyAngleRelativeToOwner);
	void ScanForwardBack(FHitResult Hit, float EnemyAngleRelativeToOwner);


	UFUNCTION(BlueprintCallable)
	virtual void TrySoftLockToClosestActorInInputDirection();

	UFUNCTION(BlueprintCallable)
	virtual AActor* GetLockedOnActor();


	virtual bool IsPlayerMovingRight();
	virtual bool IsPlayerMovingLeft();

	virtual bool IsPlayerMovingForward();
	virtual bool IsPlayerMovingBack();

	UFUNCTION(BlueprintCallable)
	virtual bool IsTargetInLockOnDistance();

	void ScanEnemiesInInputDirection(TArray<FHitResult> HitResults);
	void ResetScanSize();
	void LockOnToClosestScannedTarget();

	virtual TArray<FHitResult> SphereTraceAroundOwner();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2D PlayerInput;

#if WITH_EDITORONLY_DATA
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHitboxDebug;
#endif
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScanRadius = 1200;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MaxLockOnDistance = ScanRadius;

	UPROPERTY()
	AActor* LockedOnActor;
	
		
};
