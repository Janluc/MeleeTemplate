// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CombatComponent.h"

#include "Projectile.h"
#include "AttackAsset.h"
#include "Character/BaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Props/WeaponBase.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	// ...
}




UAttackAsset* UCombatComponent::ChooseAttack(int32& Counter, TArray<UAttackAsset*> ComboAttacks)
{
	CheckComboCounter(ComboAttacks);
	CurrentAttack = ComboAttacks[Counter];
	Counter++;
	return CurrentAttack;
}

UAttackAsset* UCombatComponent::ChooseComboAttack(TArray<UAttackAsset*> ComboAttacks)
{
	if (ComboAttacks.IsEmpty())
		return nullptr;
	
	return ChooseAttack(ComboCounter,ComboAttacks);
}

UAttackAsset* UCombatComponent::TrySkill(UAttackAsset* IncomingSkill)
{
	if (!IncomingSkill)
		return nullptr;
	
	CurrentAttack = IncomingSkill;
	bInputBuffer = false;

	return CurrentAttack;
	
}

void UCombatComponent::CheckComboCounter(TArray<UAttackAsset*> AttackList)
{
	if (ComboCounter >= AttackList.Num())
		ComboCounter = 0;

}


void UCombatComponent::AOEAttack()
{
	HitSweepedEnemies(AOESphereTrace());
}

TArray<FHitResult> UCombatComponent::AOESphereTrace()
{
	TArray<FHitResult> HitResults;
	
	FCollisionShape Sphere = FCollisionShape::MakeSphere(800);
	FCollisionQueryParams Params;

#if WITH_EDITOR
	if (bHitboxDebug)
	{
		const FName TraceTag("TraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		Params.TraceTag = TraceTag;
	}
#endif
	
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

void UCombatComponent::FixedHitboxAttack(FVector BoxExtent, FVector BoxLocation)
{
	HitSweepedEnemies(FixedHitboxTrace(BoxExtent, BoxLocation));
	HitEnemies.Empty();
}

TArray<FHitResult> UCombatComponent::FixedHitboxTrace(FVector BoxExtent, FVector BoxLocation)
{
	TArray<FHitResult> HitResults;
	
	FCollisionShape Box = FCollisionShape::MakeBox(BoxExtent);
	FCollisionQueryParams Params;

#if WITH_EDITOR
	if (bHitboxDebug)
	{
		const FName TraceTag("TraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		Params.TraceTag = TraceTag;
	}
#endif
	
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	Params.AddIgnoredActor(GetOwner());

	GetWorld()->SweepMultiByObjectType(
		HitResults,
		BoxLocation,
		BoxLocation,
		FRotator::ZeroRotator.Quaternion(),
		TraceObjects,
		Box,
		Params);

	return HitResults;
}

void UCombatComponent::SetHitboxLocation()
{
	HitboxPreviousLocation = OwnerWeapon->Hitbox->GetComponentLocation();
}

void UCombatComponent::HitSweepedEnemies(TArray<FHitResult> HitActors)
{
	for (FHitResult Hit : HitActors)
	{
		if (HitEnemies.Contains(Hit.GetActor()))
			return;
		
		if (Hit.GetActor()->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		{

			HitEnemies.Add(Hit.GetActor());
			
			ICombatInterface::Execute_HitReaction(Hit.GetActor(), CurrentAttack, Owner);
		}
	}
}


void UCombatComponent::SweepTraces()
{
	TArray<FHitResult> HitActors = TraceAndProvideHit();
	HitSweepedEnemies(HitActors);
}

void UCombatComponent::ResetCombat()
{
	ComboCounter = 0;
}

void UCombatComponent::ShootProjectile()
{
	
	UWorld* World = GetWorld();
	AProjectile* Projectile;
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = GetOwner()->GetInstigator();

		 Projectile = World->SpawnActor<AProjectile>(
				CurrentAttack->ProjectileClass,
				Owner->ProjectileSpawnLocation->GetComponentLocation(),
				FRotator::ZeroRotator,
				SpawnParams);
		if (Projectile)
		{
			Projectile->FireInDirection(GetOwner()->GetActorRotation().Vector());
		}
	}
}


TArray<FHitResult> UCombatComponent::TraceAndProvideHit()
{
	TArray<FHitResult> HitResults;
	
	FCollisionShape Capsule = FCollisionShape::MakeCapsule(
		OwnerWeapon->Hitbox->GetUnscaledCapsuleRadius(),
		OwnerWeapon->Hitbox->GetUnscaledCapsuleHalfHeight()
		);
	FCollisionQueryParams Params;

#if WITH_EDITOR	
	if (bHitboxDebug)
	{
		const FName TraceTag("TraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		Params.TraceTag = TraceTag;
	}
#endif
	
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	Params.AddIgnoredActor(GetOwner());

	GetWorld()->SweepMultiByObjectType(
		HitResults,
		HitboxPreviousLocation,
		OwnerWeapon->Hitbox->GetComponentLocation(),
		OwnerWeapon->GetActorRotation().Quaternion(),
		TraceObjects,
		Capsule,
		Params);

	return HitResults;
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ABaseCharacter>(GetOwner());
	// ...
	
}