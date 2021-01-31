// Copyright Epic Games, Inc. All Rights Reserved.

#include "SequencerExtensionEdCommands.h"

#define LOCTEXT_NAMESPACE "FSequencerExtensionModule"

void FSequencerExtensionEdCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "SequencerExtension", "Execute SequencerExtension action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
