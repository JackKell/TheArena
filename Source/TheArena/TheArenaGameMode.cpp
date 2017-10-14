// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "TheArenaGameMode.h"
#include "TheArenaCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATheArenaGameMode::ATheArenaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
