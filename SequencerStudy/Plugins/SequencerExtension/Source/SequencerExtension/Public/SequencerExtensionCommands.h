// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SequencerExtensionStyle.h"

class FSequencerExtensionCommands : public TCommands<FSequencerExtensionCommands>
{
public:

	FSequencerExtensionCommands()
		: TCommands<FSequencerExtensionCommands>(TEXT("SequencerExtension"), NSLOCTEXT("Contexts", "SequencerExtension", "SequencerExtension Plugin"), NAME_None, FSequencerExtensionStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
