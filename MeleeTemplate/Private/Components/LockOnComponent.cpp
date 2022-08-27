// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LockOnComponent.h"

#include <string>

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
ULockOnComponent::ULockOnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void ULockOnComponent::SetRightLeftInput(float AxisValue)
{
	PlayerInput.X = AxisValue;
}

void ULockOnComponent::SetUpDownInput(float AxisValue)
{
	PlayerInput.Y = AxisValue;
}

void ULockOnComponent::RotateOwnerToTarget()
{
	if(!LockedOnActor)
		return;
	
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
		GetOwner()->GetActorLocation(),
		LockedOnActor->GetActorLocation());
	GetOwner()->SetActorRotation(FRotator(0, LookAtRot.Yaw, 0));
}

void ULockOnComponent::ScanRightLeft(FHitResult Hit, float EnemyAngleRelativeToOwner)
{
	if(IsPlayerMovingRight())
	{
		if (IsTargetRightOfOwner(EnemyAngleRelativeToOwner))
		{
			ActorsScanned.Add(Hit.GetActor());
		}
	}
	else if (IsPlayerMovingLeft())
	{
		if (IsTargetLeftOfOwner(EnemyAngleRelativeToOwner))
		{
			ActorsScanned.Add(Hit.GetActor());
		}
	}
}



void ULockOnComponent::ScanForwardBack(FHitResult Hit, float EnemyAngleRelativeToOwner)
{
	if (IsPlayerMovingForward())
	{
		if (IsTargetInFrontOfOwner(EnemyAngleRelativeToOwner))
		{
			ActorsScanned.Add(Hit.GetActor());
		}
	}
	else if (IsPlayerMovingBack())
	{
		if (IsTargetBehindOwner(EnemyAngleRelativeToOwner))
		{
			ActorsScanned.Add(Hit.GetActor());
		}
			
	}
}

void ULockOnComponent::ScanEnemiesInInputDirection(TArray<FHitResult> HitResults)
{
	for (FHitResult Hit : HitResults)
	{
		FRotator EnemyLookAtRot = UKismetMathLibrary::FindLookAtRotation(
			Hit.GetActor()->GetActorLocation(),
			GetOwner()->GetActorLocation()
		);

		float EnemyAngleRelativeToOwner = UKismetMathLibrary::NormalizedDeltaRotator(
			EnemyLookAtRot,
			GetOwner()->GetInstigatorController()->GetControlRotation()).Yaw;

		if (PlayerInput.IsZero())
		{
			ActorsScanned.AddUnique(Hit.GetActor());
			continue;
		}
		
		ScanRightLeft(Hit, EnemyAngleRelativeToOwner);
		ScanForwardBack(Hit, EnemyAngleRelativeToOwner);
		GEngine->AddOnScreenDebugMessage(1, 2, FColor::Magenta, FString::SanitizeFloat(EnemyAngleRelativeToOwner));
		
	}
}

void ULockOnComponent::ResetScanSize()
{
	MaxLockOnDistance = ScanRadius;
	ActorsScanned.Empty();
}

void ULockOnComponent::LockOnToClosestScannedTarget()
{
	for (AActor* Actor : ActorsScanned)
	{
		float CurrentDistance = FVector::Dist(Actor->GetActorLocation(), GetOwner()->GetActorLocation());
		if (CurrentDistance < MaxLockOnDistance)
		{
			MaxLockOnDistance = CurrentDistance;
			LockedOnActor = Actor;
		}
	}
}

void ULockOnComponent::TrySoftLockToClosestActorInInputDirection()
{
	ActorsScanned.Empty();

	TArray<FHitResult> HitResults = SphereTraceAroundOwner();
	ScanEnemiesInInputDirection(HitResults);
	LockOnToClosestScannedTarget();
	ResetScanSize();
}

AActor* ULockOnComponent::GetLockedOnActor()
{
	return LockedOnActor;
}


bool ULockOnComponent::IsTargetRightOfOwner(float EnemyAngleRelativeToOwner)
{
	return EnemyAngleRelativeToOwner < -45 && EnemyAngleRelativeToOwner > -135;
}

bool ULockOnComponent::IsTargetLeftOfOwner(float EnemyAngleRelativeToOwner)
{
	return EnemyAngleRelativeToOwner < 135 && EnemyAngleRelativeToOwner > 45;
}


bool ULockOnComponent::IsTargetBehindOwner(float EnemyAngleRelativeToOwner)
{
	return EnemyAngleRelativeToOwner <= 45 && EnemyAngleRelativeToOwner >= -45;
}

bool ULockOnComponent::IsTargetInFrontOfOwner(float EnemyAngleRelativeToOwner)
{
	return EnemyAngleRelativeToOwner <= 180 && EnemyAngleRelativeToOwner >= 135 || EnemyAngleRelativeToOwner <= -135 &&
		EnemyAngleRelativeToOwner >= -180;
}

bool ULockOnComponent::IsPlayerMovingRight()
{
	return PlayerInput.X > 0;
}

bool ULockOnComponent::IsPlayerMovingLeft()
{
	return PlayerInput.X < 0;
}

bool ULockOnComponent::IsPlayerMovingForward()
{
	return PlayerInput.Y > 0;
}

bool ULockOnComponent::IsPlayerMovingBack()
{
	return PlayerInput.Y < 0;
}

TArray<FHitResult> ULockOnComponent::SphereTraceAroundOwner()
{
	TArray<FHitResult> HitResults;
	
	FCollisionShape Sphere = FCollisionShape::MakeSphere(ScanRadius);
	FCollisionQueryParams Params;

	
	if (bHitboxDebug)
	{
		const FName TraceTag("TraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		Params.TraceTag = TraceTag;
	}
	
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	Params.AddIgnoredActor(GetOwner());

	GetWorld()->SweepMultiByObjectType(
		HitResults,
		GetOwner()->GetActorLocation(),
		GetOwner()->GetActorLocation(),
		FRotator::ZeroRotator.Quaternion(),
		TraceObjects,
		Sphere,
		Params);

	return HitResults;
}


// Called when the game starts
void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

