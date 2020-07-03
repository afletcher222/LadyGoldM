// Copyright Epic Games, Inc. All Rights Reserved.

#include "LadyGameMode.h"
#include "LadyCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALadyGameMode::ALadyGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
