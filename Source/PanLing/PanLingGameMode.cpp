// Copyright Epic Games, Inc. All Rights Reserved.

#include "PanLingGameMode.h"
#include "PanLingCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "DamageNumberPoolComponent.h"

APanLingGameMode::APanLingGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	DamageNumberPool = CreateDefaultSubobject<UDamageNumberPoolComponent>(TEXT("DamageNumberPool"));
}
