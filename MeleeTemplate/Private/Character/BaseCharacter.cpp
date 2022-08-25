// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"

#include "MotionWarpingComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/CombatComponent.h"
#include "Components/LockOnComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Props/WeaponBase.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("Lock-On Component"));
	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping"));

	ProjectileSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn"));
	ProjectileSpawnLocation->SetupAttachment(GetRootComponent());
}

void ABaseCharacter::HitboxInit_Implementation()
{
	CombatComponent->SetHitboxLocation();
}

void ABaseCharacter::HitboxSweep_Implementation()
{
	CombatComponent->SweepTraces();
}

void ABaseCharacter::HitboxEnd_Implementation()
{
	LockOnComponent->LockedOnActor = nullptr;
	CombatComponent->HitEnemies.Empty();
	MotionWarping->RemoveWarpTarget(FName("AttackTarget"));
}

void ABaseCharacter::HitReaction_Implementation(FAttack IncomingAttack, ACharacter* AttackingCharacter)
{
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AttackingCharacter->GetActorLocation()));
	switch (IncomingAttack.AttackCategory)
	{
		case Basic:
			BasicAttackHitReaction(IncomingAttack, AttackingCharacter);
		break;
		
		case Skill:
			SkillHitReaction(IncomingAttack, AttackingCharacter);
		break;
		default: break;
	}
	
}

void ABaseCharacter::AOEAttack_Implementation()
{
	CombatComponent->AOEAttack();
}

void ABaseCharacter::ShootProjectile_Implementation()
{
	CombatComponent->ShootProjectile();
}

void ABaseCharacter::StartAttack(UAnimMontage* AttackAnimation)
{
	CharacterState = Attacking;
	
	if (IsValid(LockOnComponent->GetLockedOnActor()))
	{
		if (GetDistanceTo(LockOnComponent->GetLockedOnActor()) < CombatComponent->DashAttackDistance )
		{
			FMotionWarpingTarget MotionWarpingTarget;
			MotionWarpingTarget.Transform.SetLocation(LockOnComponent->GetLockedOnActor()->GetActorLocation());
			MotionWarping->AddOrUpdateWarpTarget(
				FName("AttackTarget"),
				MotionWarpingTarget);
		}
		
		
		if (GetDistanceTo(LockOnComponent->GetLockedOnActor()) > CombatComponent->MinDashAttackDistance &&
			GetDistanceTo(LockOnComponent->GetLockedOnActor()) < CombatComponent->DashAttackDistance)
		{
			if (!CombatComponent->CurrentAttack.AttackCategory == Skill)
			{
				CombatComponent->CurrentAttack = EquippedWeapon->DashAttack;
				
			}
			CombatComponent->ComboCounter = 0;
			PlayAnimMontage(CombatComponent->CurrentAttack.Animation);
			return;
		}
	}
	
	PlayAnimMontage(AttackAnimation);
}


void ABaseCharacter::CalculateDamage(FAttack IncomingAttack, ACharacter* AttackingCharacter)
{
	ABaseCharacter* Attacker = Cast<ABaseCharacter>(AttackingCharacter);

	float IncomingDmg = Attacker->CharacterStats.FindChecked(STR);
	float AttackMod = IncomingAttack.AttackModifier;
	float WeaponDmg = UKismetMathLibrary::RandomFloatInRange(
		Attacker->EquippedWeapon->DamageRangeMin,
		Attacker->EquippedWeapon->DamageRangeMax
	);

	float FinalDmg = ((IncomingDmg / CharacterStats.FindChecked(DEF) + WeaponDmg) * AttackMod);
	DamageTaken(FinalDmg);
	CharacterStats.Add(HP, FinalDmg);
}

void ABaseCharacter::DetermineHitReactionAnimation(FAttack IncomingAttack, ACharacter* AttackingCharacter)
{
	switch (IncomingAttack.AttackType)
	{
	case Light:
		LightAttackHitReaction(IncomingAttack, AttackingCharacter);
		break;

	case Medium: case Heavy:
		PlayAnimMontage(HeavyAttackHitReaction);
		break;

	default: break;
	}
}

void ABaseCharacter::BasicAttackHitReaction_Implementation(FAttack IncomingAttack, ACharacter* AttackingCharacter)
{
	UGameplayStatics::SpawnEmitterAtLocation(this, DefaultHitParticle, GetActorLocation(), FRotator::ZeroRotator, FVector(.5));
	CalculateDamage(IncomingAttack, AttackingCharacter);
	DetermineHitReactionAnimation(IncomingAttack, AttackingCharacter);
	HitStunStart(IncomingAttack, AttackingCharacter);
}


void ABaseCharacter::SkillHitReaction_Implementation(FAttack IncomingAttack, ACharacter* AttackingCharacter)
{
	if (IncomingAttack.HitEffectNiagara)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, IncomingAttack.HitEffectNiagara, GetActorLocation(), FRotator::ZeroRotator, FVector(.5));
	else
		UGameplayStatics::SpawnEmitterAtLocation(this, IncomingAttack.HitEffect, GetActorLocation(), FRotator::ZeroRotator, FVector(.5));
	
	CalculateDamage(IncomingAttack, AttackingCharacter);
	DetermineHitReactionAnimation(IncomingAttack, AttackingCharacter);
	HitStunStart(IncomingAttack, AttackingCharacter);
}

void ABaseCharacter::LightAttackHitReaction(FAttack IncomingAttack, ACharacter* AttackingCharacter)
{
	FVector KnockbackDirection = GetActorLocation() - AttackingCharacter->GetActorLocation() ;
	KnockbackDirection.Normalize();

	FVector Knockback = KnockbackDirection * IncomingAttack.KnockbackAmount;

	GetCharacterMovement()->AddImpulse(FVector(Knockback.X, Knockback.Y, 0), true);
	switch (IncomingAttack.AttackDirection)
	{
	case Left:
		PlayAnimMontage(LeftLightHitAnimation);
		break;

	case Right:
		PlayAnimMontage(RightLightHitAnimation);
		break;

	case DownToUp:
		PlayAnimMontage(DownToUpLightHitAnimation);
		break;

	case Thrust:
		PlayAnimMontage(ThrustLightHitAnimation);
		break;

	default: break;
	}
}

void ABaseCharacter::InputBufferHandle_Implementation()
{
	CombatComponent->DetermineComboExecution();
}

void ABaseCharacter::EndAttack_Implementation()
{
	CharacterState = Idle;
	CombatComponent->ResetCombat();
}

void ABaseCharacter::HitStunStart(FAttack IncomingAttack, ACharacter* AttackingCharacter)
{
	CustomTimeDilation = 0.01f;
	AttackingCharacter->CustomTimeDilation = 0.01f;

	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "HitStunEnd", AttackingCharacter);
	
	GetWorldTimerManager().SetTimer(HitStunHandle, Delegate, IncomingAttack.HitStunDuration, false);
}

void ABaseCharacter::HitStunEnd(ACharacter* AttackingCharacter)
{
	CustomTimeDilation = 1;
	AttackingCharacter->CustomTimeDilation = 1;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LockOnComponent->TrySoftLockToClosestActorInInputDirection();

}

