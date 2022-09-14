// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Projectile.generated.h"

class UAttackAsset;
class USphereComponent;
class UNiagaraComponent;
class UProjectileMovementComponent;
class ABaseCharacter;

UCLASS()
class MELEETEMPLATE_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* FX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY()
	ABaseCharacter* OwnerCharacter;
	UPROPERTY()
	UAttackAsset* CurrentAttack;

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void FireInDirection(const FVector& ShootDirection);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
