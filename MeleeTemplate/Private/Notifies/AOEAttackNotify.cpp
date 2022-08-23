// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/AOEAttackNotify.h"

#include "Interfaces/CombatInterface.h"

void UAOEAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
		return;
	if (MeshComp->GetOwner()->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		ICombatInterface::Execute_AOEAttack(MeshComp->GetOwner());
}
