// Copyright Epic Games, Inc. All Rights Reserved.


#include "farmPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "farmCameraManager.h"

AfarmPlayerController::AfarmPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AfarmCameraManager::StaticClass();
}

void AfarmPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}
}
