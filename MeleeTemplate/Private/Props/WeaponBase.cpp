// Fill out your copyright notice in the Description page of Project Settings.
#include "Props/WeaponBase.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
	SetRootComponent(WeaponMesh);

	Hitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Hitbox"));
	Hitbox->SetupAttachment(GetRootComponent());
	
}
