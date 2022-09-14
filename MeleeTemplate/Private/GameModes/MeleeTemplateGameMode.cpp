// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameModes/MeleeTemplateGameMode.h"
#include "Character/PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMeleeTemplateGameMode::AMeleeTemplateGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/Player/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
