// Copyright Epic Games, Inc. All Rights Reserved.

#include "SequencerExtensionCommands.h"

#define LOCTEXT_NAMESPACE "FSequencerExtensionModule"

void FSequencerExtensionCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "SequencerExtension", "Execute SequencerExtension action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
