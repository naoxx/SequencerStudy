// Copyright Epic Games, Inc. All Rights Reserved.

#include "SequencerExtension.h"
#include "SequencerExtensionStyle.h"
#include "SequencerExtensionCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

//シーケンサー
#include "Sequencer/Public/ISequencerModule.h"


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

	//Toolbar
	TSharedPtr<FUICommandList> ToolbarCommands = MakeShareable(new FUICommandList);
	ISequencerModule& SequencerModule = FModuleManager::GetModuleChecked<ISequencerModule>("Sequencer");
	ToolBarExtender = MakeShareable(new FExtender);
	ToolBarExtender->AddToolBarExtension(
		"Level Sequence Separator",
		EExtensionHook::After,
		ToolbarCommands,
		FToolBarExtensionDelegate::CreateRaw(this, &FSequencerExtensionModule::AddToolBarExtention)
	);
	SequencerModule.GetToolBarExtensibilityManager()->AddExtender(ToolBarExtender);

}

void FSequencerExtensionModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//ToolBar
	if (ToolBarExtender.IsValid() && FModuleManager::Get().IsModuleLoaded("Sequencer"))
	{
		ISequencerModule& SequencerModule = FModuleManager::GetModuleChecked<ISequencerModule>("Sequencer");
		SequencerModule.GetToolBarExtensibilityManager()->RemoveExtender(ToolBarExtender);
	}
	ToolBarExtender = nullptr;

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

//ToolBar
void FSequencerExtensionModule::AddToolBarExtention(FToolBarBuilder& ToolBarBuilder)
{
	ToolBarBuilder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FSequencerExtensionModule::MakeToolbarExtensionMenu),
		LOCTEXT("SequencerExtension", "ToolBarExtension"),
		LOCTEXT("SequencerExtension", "ToolBarExtension"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "Sequencer.Actions"),	//仮でアクションと同じアイコン
		false);
}

TSharedRef<class SWidget> FSequencerExtensionModule::MakeToolbarExtensionMenu()
{
	FMenuBuilder MenuBuilder(true, MakeShareable(new FUICommandList));
	MenuBuilder.BeginSection("SequencerToolBarExtension");

	MenuBuilder.AddMenuEntry(
		LOCTEXT("SequencerExtension", "ToolBarMenu"),
		LOCTEXT("SequencerExtension", "ToolBarMenu"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "Sequencer.Actions"),
		FUIAction(FExecuteAction::CreateLambda(
			[]()
			{
				FText DialogText = FText::FromString("ToolBarMenuExtension");
				FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			}
	)));

	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSequencerExtensionModule, SequencerExtension)