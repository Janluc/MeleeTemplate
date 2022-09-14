// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UAttackAsset;
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
	UAttackAsset* DashAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* Hitbox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageRangeMin = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageRangeMax = 12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAttackAsset*> BasicAttackList;
	
};
