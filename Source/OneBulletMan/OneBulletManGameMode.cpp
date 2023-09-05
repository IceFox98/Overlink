// Copyright Epic Games, Inc. All Rights Reserved.

#include "OneBulletManGameMode.h"
#include "OneBulletManCharacter.h"
#include "UObject/ConstructorHelpers.h"

AOneBulletManGameMode::AOneBulletManGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
