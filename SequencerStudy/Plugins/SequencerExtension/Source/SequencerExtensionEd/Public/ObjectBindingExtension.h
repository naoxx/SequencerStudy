// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ISequencerEditorObjectBinding.h"
/**
 * 
 */
class ISequencer;
class AActor;

class FObjectBindingExtension : public ISequencerEditorObjectBinding
{
public:

	FObjectBindingExtension(TSharedRef<ISequencer> InSequencer);

	static TSharedRef<ISequencerEditorObjectBinding> OnCreateActorBinding(TSharedRef<ISequencer> inSequencer);

	// ISequencerEditorObjectBinding interface
	virtual void BuildSequencerAddMenu(FMenuBuilder& MenuBuilder) override;
	virtual bool SupportsSequence(UMovieSceneSequence* InSequence) const override;

private:

	void AddObjectBindingMenuExtension();

private:
	TWeakPtr<ISequencer> Sequencer;
};
