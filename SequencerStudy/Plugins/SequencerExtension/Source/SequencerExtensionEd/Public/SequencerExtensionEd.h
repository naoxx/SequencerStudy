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
	//シーケンサーのツールバーメニュー拡張
	void AddToolBarExtention(FToolBarBuilder& ToolBarBuilder);
	TSharedRef<class SWidget> MakeToolbarExtensionMenu();
	void AddObjectBindingContextMenuExtention(FMenuBuilder& MenuBuilder);
	void AddSpawnablePropertyMenu(FMenuBuilder& MenuBuilder, UMovieScene* MovieScene, ISequencer* Sequencer);

	//LevelSequence右クリックメニュー
	void AddLevelSequenceAssetContextMenu();

	//LevelSequence取得
	class ISequencer* GetCurrentSequencer();

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	TSharedPtr<class FExtender> ToolBarExtender;
	TSharedPtr<FExtender> ObjectBindingContextMenuExtender;
	FDelegateHandle ObjectBindingDelegateHandle;

	FGuid SelectedObjectBinding;
};
