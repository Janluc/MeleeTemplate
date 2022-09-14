// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/CombatComponent.h"
#include "AttackAsset.h"
#include "Character/PlayerCharacter.h"
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
	ProjectileSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn"));
	ProjectileSpawnLocation->SetupAttachment(GetRootComponent());

	FixedHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Fixed Hitbox"));
	FixedHitbox->SetupAttachment(GetRootComponent());
	
}


// Combat Interface
void ABaseCharacter::FixedHitbox_Implementation()
{
	CombatComponent->FixedHitboxAttack(FixedHitbox->GetUnscaledBoxExtent(), FixedHitbox->GetComponentLocation());
}

void ABaseCharacter::TryAttack_Implementation()
{
}

void ABaseCharacter::AOEAttack_Implementation()
{
	CombatComponent->AOEAttack();
}

void ABaseCharacter::ShootProjectile_Implementation()
{
	CombatComponent->ShootProjectile();
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
	
	CombatComponent->HitEnemies.Empty();
	
}


void ABaseCharacter::HealSkill_Implementation()
{
	ModifyHealth(CombatComponent->CurrentAttack->HealAmount);
}

void ABaseCharacter::DeathReaction_Implementation()
{
}

void ABaseCharacter::EndAttack_Implementation()
{
	CharacterState = Idle;
	CombatComponent->ResetCombat();
	
}

void ABaseCharacter::HitStunStart_Implementation(float HitStunDuration)
{
	HitStunDelegate.Unbind();
	CustomTimeDilation = 0.01f;
	
	HitStunDelegate.BindUFunction(this, "HitStunEnd");
	
	GetWorldTimerManager().SetTimer(HitStunHandle, HitStunDelegate, HitStunDuration, false);
}
void ABaseCharacter::HitStunEnd()
{
	CustomTimeDilation = 1;
}


void ABaseCharacter::SetAndStartAttack()
{

	UAttackAsset* Attack = CombatComponent->ChooseComboAttack(EquippedWeapon->BasicAttackList);
	if (Attack)
	{
		StartAttack(Attack->Animation);
	}

}

void ABaseCharacter::ModifyHealth(float ChangeAmount)
{
	DamageTaken(ChangeAmount);
	float ClampedDamage = FMath::Clamp(CharacterStats.FindChecked(HP) + ChangeAmount, 0, CharacterStats.FindChecked(MAXHP));
	CharacterStats.Add(HP, ClampedDamage);
}

bool ABaseCharacter::IsCharacterDead()
{
	return CharacterStats.FindChecked(HP) <= 0;
}

void ABaseCharacter::RotateTowardsAttacker(ACharacter* AttackingCharacter)
{
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AttackingCharacter->GetActorLocation());
	SetActorRotation(FRotator(
			GetActorRotation().Pitch,
			LookAtRot.Yaw,
			GetActorRotation().Roll)
	);
}

void ABaseCharacter::ApplyHitEffects_Implementation(UAttackAsset* IncomingAttack)
{
	SpawnHitParticle(IncomingAttack);
	DetermineHitReactionAnimation(IncomingAttack);
	HitCameraShake();
}

void ABaseCharacter::HitReaction_Implementation(UAttackAsset* IncomingAttack, ACharacter* AttackingCharacter)
{
	if (CharacterState == Dodging || bIsDead)
		return;

	EndAttack_Implementation();
	
	CalculateDamage(IncomingAttack, AttackingCharacter);
	PreHit();
	HitStunStart_Implementation(IncomingAttack->HitStunDuration);
	Execute_HitStunStart(AttackingCharacter, IncomingAttack->HitStunDuration);
	
	if(IsCharacterDead())
	{
		OnDeath(AttackingCharacter);
		return;
	}
	if (CombatComponent->CurrentAttack && CombatComponent->CurrentAttack->bHasHyperArmor)
		return;
	
	CharacterState = Hurt;
	Knockback(IncomingAttack, AttackingCharacter);
	RotateTowardsAttacker(AttackingCharacter);
	GetMovementComponent()->Velocity = FVector::ZeroVector;
	StopAnimMontage();

	ApplyHitEffects(IncomingAttack);
	
}

void ABaseCharacter::CalculateDamage(UAttackAsset* IncomingAttack, ACharacter* AttackingCharacter)
{
	ABaseCharacter* Attacker = Cast<ABaseCharacter>(AttackingCharacter);

	float IncomingDmg = Attacker->CharacterStats.FindChecked(STR);
	float AttackMod = IncomingAttack->AttackModifier;
	float WeaponDmg = UKismetMathLibrary::RandomFloatInRange(
		Attacker->EquippedWeapon->DamageRangeMin,
		Attacker->EquippedWeapon->DamageRangeMax
	);

	float FinalDmg = UKismetMathLibrary::FCeil(((IncomingDmg / CharacterStats.FindChecked(DEF) + WeaponDmg) * AttackMod));

	ModifyHealth( -1 * FinalDmg);
}

void ABaseCharacter::OnDeath(ACharacter* AttackingCharacter)
{
	CharacterState = Hurt;
	RotateTowardsAttacker(AttackingCharacter);
	APlayerCharacter* AttackerPlayer = Cast<APlayerCharacter>(AttackingCharacter);
	if (AttackerPlayer)
	{
		AttackerPlayer->LockOnComponent->LockedOnActor = nullptr;
	}
	
	bIsDead = true;
	UGameplayStatics::SpawnEmitterAtLocation(this, DefaultHitParticle, GetActorLocation(), FRotator::ZeroRotator, FVector(.5));
	DeathReaction();
}



void ABaseCharacter::StartAttack(UAnimMontage* AttackAnimation)
{
	CharacterState = Attacking;
	
	
	
	PlayAnimMontage(AttackAnimation);
}

void ABaseCharacter::DetermineHitReactionAnimation(UAttackAsset* IncomingAttack)
{
	switch (IncomingAttack->AttackType)
	{
	case Light:
		LightAttackHitReaction(IncomingAttack->AttackDirection);
		break;

	case Medium: case Heavy:
		PlayAnimMontage(HeavyAttackHitReaction);
		break;

	default: break;
	}
}

void ABaseCharacter::SpawnHitParticle(UAttackAsset* IncomingAttack)
{
	if (IncomingAttack->AttackCategory == Skill)
	{
		if (IncomingAttack->HitEffectNiagara)
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, IncomingAttack->HitEffectNiagara, GetActorLocation(), FRotator::ZeroRotator, FVector(.5));
		else
			UGameplayStatics::SpawnEmitterAtLocation(this, IncomingAttack->HitEffect, GetActorLocation(), FRotator::ZeroRotator, FVector(.5));
	}
	else
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, DefaultHitParticle, GetActorLocation(), FRotator::ZeroRotator, FVector(.5));

	}
	
}

void ABaseCharacter::Knockback(UAttackAsset* IncomingAttack, ACharacter* AttackingCharacter)
{
	FVector KnockbackDirection = GetActorLocation() - AttackingCharacter->GetActorLocation() ;
	KnockbackDirection.Normalize();

	FVector Knockback = KnockbackDirection * IncomingAttack->KnockbackAmount;

	GetCharacterMovement()->AddImpulse(FVector(Knockback.X, Knockback.Y, 1), true);
}

void ABaseCharacter::LightAttackHitReaction(TEnumAsByte<EAttackDirectionReaction> AttackDirection)
{
	switch (AttackDirection)
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


// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterStats.Add(HP, CharacterStats.FindChecked(MAXHP));
}
// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

