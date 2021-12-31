// Copyright Epic Games, Inc. All Rights Reserved.

#include "SequencerExtensionEd.h"
#include "SequencerExtensionEdStyle.h"
#include "SequencerExtensionEdCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "ObjectBindingExtension.h"

//シーケンサー
#include "Sequencer/Public/ISequencerModule.h"
#include "LevelSequence.h"
#include "LevelSequenceEditor/Private/LevelSequenceEditorToolkit.h"

//AssetRegistry
#include "AssetRegistryModule.h"

//構造体の詳細表示
#include "IStructureDetailsView.h"

//AssetContextMenu
#include "ContentBrowserMenuContexts.h"



static const FName SequencerExtensionTabName("SequencerExtension");

#define LOCTEXT_NAMESPACE "FSequencerExtensionModule"

void CheckSequencer()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> AssetDatas;
	AssetRegistry.GetAssetsByClass(ULevelSequence::StaticClass()->GetFName(), AssetDatas);

	for (FAssetData AssetData : AssetDatas)
	{
		ULevelSequence* LevelSequence = Cast<ULevelSequence>(AssetData.GetAsset());
		UMovieScene* MovieScene = LevelSequence->GetMovieScene();
		for (int i = 0; i < MovieScene->GetSpawnableCount(); i++)
		{
			FMovieSceneSpawnable& Spawnable = MovieScene->GetSpawnable(i);
			FMovieSceneBinding* Binding = MovieScene->FindBinding(Spawnable.GetGuid());
			Binding->GetTracks();
			//～～～なんかする～～～
		}
	}
}


void FSequencerExtensionEdModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FSequencerExtensionEdStyle::Initialize();
	FSequencerExtensionEdStyle::ReloadTextures();

	FSequencerExtensionEdCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FSequencerExtensionEdCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FSequencerExtensionEdModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSequencerExtensionEdModule::RegisterMenus));

	//Toolbar
	TSharedPtr<FUICommandList> ToolbarCommands = MakeShareable(new FUICommandList);
	ISequencerModule& SequencerModule = FModuleManager::GetModuleChecked<ISequencerModule>("Sequencer");
	ToolBarExtender = MakeShareable(new FExtender);
	ToolBarExtender->AddToolBarExtension(
		"Level Sequence Separator",
		EExtensionHook::After,
		ToolbarCommands,
		FToolBarExtensionDelegate::CreateRaw(this, &FSequencerExtensionEdModule::AddToolBarExtention)
	);
	SequencerModule.GetToolBarExtensibilityManager()->AddExtender(ToolBarExtender);
	ObjectBindingDelegateHandle = SequencerModule.RegisterEditorObjectBinding(FOnCreateEditorObjectBinding::CreateStatic(&FObjectBindingExtension::OnCreateActorBinding));


	//ObjectBindingContext
	ObjectBindingContextMenuExtender = MakeShareable(new FExtender);
	ObjectBindingContextMenuExtender->AddMenuExtension(
		"Spawnable",
		EExtensionHook::After,
		PluginCommands,
		FNewMenuDelegate::CreateRaw(this, &FSequencerExtensionEdModule::AddObjectBindingContextMenuExtention)
	);
	SequencerModule.GetObjectBindingContextMenuExtensibilityManager()->AddExtender(ObjectBindingContextMenuExtender);

}

void FSequencerExtensionEdModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//ToolBar
	if (ToolBarExtender.IsValid() && FModuleManager::Get().IsModuleLoaded("Sequencer"))
	{
		ISequencerModule& SequencerModule = FModuleManager::GetModuleChecked<ISequencerModule>("Sequencer");
		SequencerModule.GetObjectBindingContextMenuExtensibilityManager()->RemoveExtender(ObjectBindingContextMenuExtender);
		SequencerModule.UnRegisterEditorObjectBinding(ObjectBindingDelegateHandle);
		SequencerModule.GetToolBarExtensibilityManager()->RemoveExtender(ToolBarExtender);
	}
	ToolBarExtender = nullptr;

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FSequencerExtensionEdStyle::Shutdown();

	FSequencerExtensionEdCommands::Unregister();
}

void FSequencerExtensionEdModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
			LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
			FText::FromString(TEXT("FSequencerExtensionModule::PluginButtonClicked()")),
			FText::FromString(TEXT("SequencerExtension.cpp"))
		);
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	CheckSequencer();
}

void FSequencerExtensionEdModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FSequencerExtensionEdCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FSequencerExtensionEdCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
	

	{
		//右クリックメニュー
		FToolMenuOwnerScoped ContextMenuOwner("LevelSequenceContextMenuExtension");
		UToolMenus* ToolMenus = UToolMenus::Get();
		UToolMenu* AssetContextMenu = ToolMenus->ExtendMenu("ContentBrowser.AssetContextMenu.LevelSequence");
		if (!AssetContextMenu)
		{
			return;
		}

		FToolMenuSection& Section = AssetContextMenu->FindOrAddSection("GetAssetActions");
		Section.AddDynamicEntry("LevelSequenceContextMenuExtension", FNewToolMenuSectionDelegate::CreateLambda([this](FToolMenuSection& InSection)
			{
				UContentBrowserAssetContextMenuContext* Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
				if (!Context)
				{
					return;
				}
				//一つのアセットを選択中のみ有効
				ULevelSequence* LevelSequence = Context->SelectedObjects.Num() == 1 ? Cast<ULevelSequence>(Context->SelectedObjects[0]) : nullptr;
				if (LevelSequence)
				{
					InSection.AddMenuEntry(
						"LevelSequenceContextMenuExtension",
						FText::FromString(TEXT("LevelSequenceContextMenuExtension")),
						FText::FromString(TEXT("LevelSequenceContextMenuExtension")),
						FSlateIcon(),
						FExecuteAction::CreateLambda(
							[this, LevelSequence]
							{
								FText DialogText = FText::FromString("LevelSequenceContextMenuExtension");
								FMessageDialog::Open(EAppMsgType::Ok, DialogText);
							}));
				}
			}));
	}
}

//ToolBar
void FSequencerExtensionEdModule::AddToolBarExtention(FToolBarBuilder& ToolBarBuilder)
{
	ToolBarBuilder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FSequencerExtensionEdModule::MakeToolbarExtensionMenu),
		LOCTEXT("SequencerExtension", "ToolBarExtension"),
		LOCTEXT("SequencerExtension", "ToolBarExtension"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "Sequencer.Actions"),	//仮でアクションと同じアイコン
		false);
}

TSharedRef<class SWidget> FSequencerExtensionEdModule::MakeToolbarExtensionMenu()
{
	FMenuBuilder MenuBuilder(true, MakeShareable(new FUICommandList));
	MenuBuilder.BeginSection("SequencerToolBarExtension");

	MenuBuilder.AddMenuEntry(
		LOCTEXT("SequencerExtension", "ToolBarMenu"),
		LOCTEXT("SequencerExtension", "ToolBarMenu"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "Sequencer.Actions"),
		FUIAction(FExecuteAction::CreateLambda(
			[this]()
			{
				ISequencer* Sequencer = GetCurrentSequencer();
				FText DialogText = FText::FromString("ToolBarMenuExtension");
				FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			}
	)));

	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}


void FSequencerExtensionEdModule::AddObjectBindingContextMenuExtention(FMenuBuilder& MenuBuilder)
{
	ISequencer* OpenSequencer = GetCurrentSequencer();
	if (OpenSequencer == nullptr)
	{
		return;
	}
	SelectedObjectBinding = FGuid();
	TArray<FGuid> guids;
	OpenSequencer->GetSelectedObjects(guids);
	if (guids.Num() == 1)
	{
		SelectedObjectBinding = guids[0];
	}

	if (!SelectedObjectBinding.IsValid())
	{
		return;
	}

	UMovieScene* MovieScene = OpenSequencer->GetFocusedMovieSceneSequence()->GetMovieScene();
	FMovieSceneSpawnable* Spawnable = MovieScene->FindSpawnable(SelectedObjectBinding);
	if (!Spawnable)
	{
		return;
	}
	MenuBuilder.BeginSection("OverrideObjectBinding", FText::FromString("OverrideObjectBinding"));

	AddSpawnablePropertyMenu(MenuBuilder, MovieScene, OpenSequencer);

	MenuBuilder.EndSection();
}


void FSequencerExtensionEdModule::AddSpawnablePropertyMenu(FMenuBuilder& MenuBuilder, UMovieScene* MovieScene, ISequencer* Sequencer)
{

	FMovieSceneSpawnable* Spawnable = MovieScene->FindSpawnable(SelectedObjectBinding);

	auto PopulateSubMenu = [this, Spawnable](FMenuBuilder& SubMenuBuilder)
	{
		FPropertyEditorModule& PropertyEditor = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea, true);
		DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Automatic;
		DetailsViewArgs.bShowOptions = false;
		DetailsViewArgs.ColumnWidth = 0.55f;

		FStructOnScope* StructOnScope = new FStructOnScope(FMovieSceneSpawnable::StaticStruct(), (uint8*)Spawnable);
		TSharedRef<IStructureDetailsView> StructureDetailsView = PropertyEditor.CreateStructureDetailView(
			DetailsViewArgs,
			FStructureDetailsViewArgs(),
			MakeShareable(StructOnScope)
		);
		
		TSharedRef< SWidget > DetailsViewWidget = StructureDetailsView->GetWidget().ToSharedRef();

		SubMenuBuilder.AddWidget(DetailsViewWidget, FText(), true, false);
	};

	MenuBuilder.AddSubMenu(FText::FromString("SpawnableProperty"), FText(), FNewMenuDelegate::CreateLambda(PopulateSubMenu));
}


//LevelSequence右クリックメニュー
void FSequencerExtensionEdModule::AddLevelSequenceAssetContextMenu()
{

}


ISequencer* FSequencerExtensionEdModule::GetCurrentSequencer()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> AssetDatas;
	AssetRegistry.GetAssetsByClass(ULevelSequence::StaticClass()->GetFName(), AssetDatas);

	for (FAssetData AssetData : AssetDatas)
	{
		ULevelSequence* LevelSequence = Cast<ULevelSequence>(AssetData.GetAsset());

		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		IAssetEditorInstance* AssetEditor = AssetEditorSubsystem->FindEditorForAsset(LevelSequence, true);
		FLevelSequenceEditorToolkit* LevelSequenceEditor = (FLevelSequenceEditorToolkit*)AssetEditor;
		if (LevelSequenceEditor != nullptr)
		{
			ISequencer* Sequencer = LevelSequenceEditor->GetSequencer().Get();
			return Sequencer;
		}
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSequencerExtensionEdModule, SequencerExtensionEd)