// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FSequencerExtensionEdModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();
	
private:

	void RegisterMenus();
	//�V�[�P���T�[�̃c�[���o�[���j���[�g��
	void AddToolBarExtention(FToolBarBuilder& ToolBarBuilder);
	TSharedRef<class SWidget> MakeToolbarExtensionMenu();

	//LevelSequence�擾
	class ISequencer* GetCurrentSequencer();

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	TSharedPtr<class FExtender> ToolBarExtender;
};
