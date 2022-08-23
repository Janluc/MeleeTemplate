// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Structs/Attack.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class MELEETEMPLATE_API AWeaponBase : public AActor
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* WeaponMesh;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FTransform WeaponTransform;

	

public:
	// Sets default values for this actor's properties
	AWeaponBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttack DashAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAttack> BasicAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* Hitbox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageRangeMin = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageRangeMax = 12;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
