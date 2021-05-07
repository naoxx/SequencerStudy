// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectBindingExtension.h"

#include "Misc/Guid.h"
#include "EditorStyleSet.h"
#include "LevelSequence.h"

#define LOCTEXT_NAMESPACE "FObjectBindingExtension"

FObjectBindingExtension::FObjectBindingExtension(TSharedRef<ISequencer> InSequencer)
    : Sequencer(InSequencer)
{ }

TSharedRef<ISequencerEditorObjectBinding> FObjectBindingExtension::OnCreateActorBinding(TSharedRef<ISequencer> inSequencer)
{
	return MakeShareable(new FObjectBindingExtension(inSequencer));
}

void FObjectBindingExtension::BuildSequencerAddMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		LOCTEXT("ObjectBindingEx", "ObjectBindingEx"),
		LOCTEXT("ObjectBindingToolTip", "ObjectBindingToolTip"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.ColumnViewAssetIcon"),
		FUIAction(FExecuteAction::CreateRaw(this, &FObjectBindingExtension::AddObjectBindingMenuExtension)));
}

bool FObjectBindingExtension::SupportsSequence(UMovieSceneSequence* InSequence) const
{
	return InSequence->GetClass() == ULevelSequence::StaticClass();
}

void FObjectBindingExtension::AddObjectBindingMenuExtension()
{
	FText DialogText = FText::FromString("ObjectBindingMenuExtension");
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

#undef LOCTEXT_NAMESPACE
