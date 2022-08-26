// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CombatComponent.h"

#include "Projectile.h"
#include "Character/BaseCharacter.h"
#include "Components/LockOnComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Props/WeaponBase.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}


void UCombatComponent::RotateOwnerToTarget()
{
	if(!Owner->LockOnComponent->LockedOnActor)
		return;
	
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
		Owner->GetActorLocation(),
		Owner->LockOnComponent->LockedOnActor->GetActorLocation());
	GetOwner()->SetActorRotation(FRotator(0, LookAtRot.Yaw, 0));
}

void UCombatComponent::ChooseAttack(int32& Counter, TArray<UAttackAsset*> ComboAttacks)
{
	CheckComboCounter(ComboAttacks);
	CurrentAttack = ComboAttacks[Counter];
	Counter++;
}

void UCombatComponent::ComboAttack(TArray<UAttackAsset*> ComboAttacks)
{
	if (ComboAttacks.IsEmpty())
		return;
	
	RotateOwnerToTarget();
	ChooseAttack(ComboCounter,ComboAttacks);
	Owner->StartAttack(CurrentAttack->Animation);
}

void UCombatComponent::Skill(UAttackAsset* IncomingSkill)
{
	if (!IncomingSkill)
		return;
	
	RotateOwnerToTarget();
	CurrentAttack = IncomingSkill;
	bInputBuffer = false;
	Owner->StartAttack(CurrentAttack->Animation);
}

void UCombatComponent::CheckComboCounter(TArray<UAttackAsset*> AttackList)
{
	if (ComboCounter >= AttackList.Num())
		ComboCounter = 0;

}

void UCombatComponent::AOEAttack()
{
	TArray<FHitResult> HitResults;
	
	FCollisionShape Sphere = FCollisionShape::MakeSphere(800);
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

	HitSweepedEnemies(HitResults);
}

void UCombatComponent::HandleSkillInput(UAttackAsset* IncomingSkill)
{
	if (!Owner || !OwnerWeapon)
		return;
	
		Skill(IncomingSkill);
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

void UCombatComponent::DetermineComboExecution(TArray<UAttackAsset*> ComboAttacks)
{
	if(bInputBuffer)
	{
		bInputBuffer = false;
		ComboAttack(ComboAttacks);
	}
	else
		Owner->CharacterState = Idle;
}

TArray<FHitResult> UCombatComponent::TraceAndProvideHit()
{
	TArray<FHitResult> HitResults;
	
	FCollisionShape Capsule = FCollisionShape::MakeCapsule(
		OwnerWeapon->Hitbox->GetUnscaledCapsuleRadius(),
		OwnerWeapon->Hitbox->GetUnscaledCapsuleHalfHeight()
		);
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


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

