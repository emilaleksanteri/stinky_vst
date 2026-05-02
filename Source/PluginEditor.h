/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include "Square.h"
#include <JuceHeader.h>

//==============================================================================
/**
 */
class Stinky_vstAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  Stinky_vstAudioProcessorEditor(Stinky_vstAudioProcessor &);
  ~Stinky_vstAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  Square square;
  Stinky_vstAudioProcessor &audioProcessor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Stinky_vstAudioProcessorEditor)
};
