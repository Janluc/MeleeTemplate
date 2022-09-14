// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/FixedHitboxNotify.h"

#include "Interfaces/CombatInterface.h"

void UFixedHitboxNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp || !MeshComp->GetOwner())
		return;
	if (MeshComp->GetOwner()->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		ICombatInterface::Execute_FixedHitbox(MeshComp->GetOwner());
}
