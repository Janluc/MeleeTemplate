// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameModes/MeleeTemplateGameMode.h"
#include "Character/MeleeTemplateCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMeleeTemplateGameMode::AMeleeTemplateGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
