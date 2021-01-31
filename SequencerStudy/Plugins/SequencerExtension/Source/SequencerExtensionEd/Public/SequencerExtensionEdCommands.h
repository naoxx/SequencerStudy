// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SequencerExtensionEdStyle.h"

class FSequencerExtensionEdCommands : public TCommands<FSequencerExtensionEdCommands>
{
public:

	FSequencerExtensionEdCommands()
		: TCommands<FSequencerExtensionEdCommands>(TEXT("SequencerExtension"), NSLOCTEXT("Contexts", "SequencerExtension", "SequencerExtension Plugin"), NAME_None, FSequencerExtensionEdStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
