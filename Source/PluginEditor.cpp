/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "juce_events/juce_events.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"

//==============================================================================
Stinky_vstAudioProcessorEditor::Stinky_vstAudioProcessorEditor(
    Stinky_vstAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      freqSliderAttachment(audioProcessor.getState(), "freqHz",
                           frequencySlider),
      playBtnAttachment(audioProcessor.getState(), "play", playBtn),
      oscillatorToggleAttachment(audioProcessor.getState(), "oscillator-type",
                                 oscillatorToggle) {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.

  frequencySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
  frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
  addAndMakeVisible(frequencySlider);

  playBtn.setButtonText("Playing");
  playBtn.setToggleState(true, juce::NotificationType::dontSendNotification);
  playBtn.setClickingTogglesState(true);

  oscillatorToggle.setButtonText("SineWave");
  oscillatorToggle.setToggleState(false,
                                  juce::NotificationType::dontSendNotification);
  oscillatorToggle.setClickingTogglesState(true);

  playBtn.setColour(juce::TextButton::ColourIds::buttonOnColourId,
                    juce::Colours::green);
  playBtn.setColour(juce::TextButton::ColourIds::buttonColourId,
                    juce::Colours::red);

  playBtn.onClick = [this]() {
    const bool isPlaying = playBtn.getToggleState();
    playBtn.setButtonText(isPlaying ? "Playing" : "Play");
  };

  oscillatorToggle.onClick = [this]() {
    const bool useSaw = oscillatorToggle.getToggleState();
    oscillatorToggle.setButtonText(useSaw ? "SawWave" : "SineWave");
  };

  addAndMakeVisible(playBtn);
  addAndMakeVisible(oscillatorToggle);

  frequencyLabel.setJustificationType(juce::Justification::centred);

  addAndMakeVisible(frequencyLabel);

  setSize(500, 500);
}

Stinky_vstAudioProcessorEditor::~Stinky_vstAudioProcessorEditor() {}

//==============================================================================
void Stinky_vstAudioProcessorEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(juce::Colours::limegreen);

  g.setColour(juce::Colours::black);
  g.setFont(juce::FontOptions(30.0f));
  g.drawFittedText("STINKY VST", getLocalBounds(), juce::Justification::topLeft,
                   1);
}

void Stinky_vstAudioProcessorEditor::resized() {
  frequencyLabel.setBounds(getWidth() / 2 - 50, getHeight() / 2 - 120, 100, 20);
  frequencySlider.setBounds(getWidth() / 2 - 50, getHeight() / 2 - 100, 100,
                            200);
  playBtn.setBounds(getWidth() / 2 - 50, getHeight() / 2 + 120, 100, 20);
  oscillatorToggle.setBounds(getWidth() / 2 - 50, getHeight() / 2 + 150, 100,
                             20);
}
