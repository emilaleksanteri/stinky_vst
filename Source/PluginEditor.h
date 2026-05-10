/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"
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

  void parentHierarchyChanged() override {
    if (isShowing())
      keyboard.grabKeyboardFocus();
  }

private:
  juce::ComboBox oscillatorType;

  Stinky_vstAudioProcessor &audioProcessor;

  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
      oscillatorTypeAttachment;

  juce::AudioProcessorValueTreeState::SliderAttachment attackConfigAttachment;

  juce::AudioProcessorValueTreeState::SliderAttachment decayConfigAttachment;

  juce::AudioProcessorValueTreeState::SliderAttachment sustainConfigAttachment;

  juce::AudioProcessorValueTreeState::SliderAttachment releaseConfigAttachment;

  juce::MidiKeyboardComponent keyboard{
      audioProcessor.getKeyboardState(),
      juce::MidiKeyboardComponent::horizontalKeyboard,
  };

  juce::Slider attackConfig;
  juce::Label attackLabel{"AttackLabel", "Attack"};

  juce::Slider decayConfig;
  juce::Label decayLabel{"DecayLabel", "Decay"};

  juce::Slider sustainConfig;
  juce::Label sustainLabel{"SustainLabel", "Sustain"};

  juce::Slider releaseConfig;
  juce::Label releaseLabel{"ReleaseLabel", "Release"};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Stinky_vstAudioProcessorEditor)
};
