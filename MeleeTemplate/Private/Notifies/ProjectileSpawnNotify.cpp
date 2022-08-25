// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/ProjectileSpawnNotify.h"

#include "Interfaces/CombatInterface.h"

void UProjectileSpawnNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp || !MeshComp->GetOwner())
		return;
	
	if (MeshComp->GetOwner()->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		ICombatInterface::Execute_ShootProjectile(MeshComp->GetOwner());
}
