// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/BaseCharacter.h"
#include "Components/CombatComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	
	FX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FX"));
	FX->SetupAttachment(GetRootComponent());

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	
}

void AProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{

	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	if (!Hit.GetActor() || Hit.GetActor() == GetOwner())
		return;

	if (CurrentAttack.HitEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			OwnerCharacter->CombatComponent->CurrentAttack.HitEffect,
			HitLocation, FRotator::ZeroRotator, FVector(.5));
	}
	else if (CurrentAttack.HitEffectNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			OwnerCharacter->CombatComponent->CurrentAttack.HitEffectNiagara,
			HitLocation,
			FRotator::ZeroRotator,
			FVector(.5));
	}
	if (Hit.GetActor()->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
	{
		ICombatInterface::Execute_HitReaction(
			Hit.GetActor(),
			OwnerCharacter->CombatComponent->CurrentAttack,
			OwnerCharacter);
	}

	Destroy();
	
}

void AProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		CurrentAttack = OwnerCharacter->CombatComponent->CurrentAttack;
	}
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}