/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
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

private:
  juce::Slider frequencySlider;
  juce::TextButton playBtn;
  juce::Label frequencyLabel{"FrequencyLabel", "Frequency"};
  juce::ComboBox oscillatorType;

  Stinky_vstAudioProcessor &audioProcessor;

  juce::AudioProcessorValueTreeState::SliderAttachment freqSliderAttachment;
  juce::AudioProcessorValueTreeState::ButtonAttachment playBtnAttachment;

  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
      oscillatorTypeAttachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Stinky_vstAudioProcessorEditor)
};
