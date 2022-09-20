// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifies/EndAttackNotify.h"

#include "Interfaces/CombatInterface.h"

void UEndAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation);
	
	if (!MeshComp || !MeshComp->GetOwner())
		return;
	if (MeshComp->GetOwner()->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		ICombatInterface::Execute_EndAttack(MeshComp->GetOwner());
	
}
