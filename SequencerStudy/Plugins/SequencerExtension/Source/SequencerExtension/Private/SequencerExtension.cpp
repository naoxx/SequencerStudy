// Copyright Epic Games, Inc. All Rights Reserved.

#include "SequencerExtension.h"
#include "SequencerExtensionStyle.h"
#include "SequencerExtensionCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName SequencerExtensionTabName("SequencerExtension");

#define LOCTEXT_NAMESPACE "FSequencerExtensionModule"

void FSequencerExtensionModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FSequencerExtensionStyle::Initialize();
	FSequencerExtensionStyle::ReloadTextures();

	FSequencerExtensionCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FSequencerExtensionCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FSequencerExtensionModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSequencerExtensionModule::RegisterMenus));
}

void FSequencerExtensionModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FSequencerExtensionStyle::Shutdown();

	FSequencerExtensionCommands::Unregister();
}

void FSequencerExtensionModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FSequencerExtensionModule::PluginButtonClicked()")),
							FText::FromString(TEXT("SequencerExtension.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FSequencerExtensionModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FSequencerExtensionCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FSequencerExtensionCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSequencerExtensionModule, SequencerExtension)