/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginParameters.h"
#include "PluginProcessor.h"
#include "WaveType.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"

//==============================================================================
Stinky_vstAudioProcessorEditor::Stinky_vstAudioProcessorEditor(
    Stinky_vstAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      attackConfigAttachment(audioProcessor.getState(),
                             PluginParameters::ATTACK_CONFIG, attackConfig),
      decayConfigAttachment(audioProcessor.getState(),
                            PluginParameters::DECAY_CONFIG, decayConfig),
      sustainConfigAttachment(audioProcessor.getState(),
                              PluginParameters::SUSTAIN_CONFIG, sustainConfig),
      releaseConfigAttachment(audioProcessor.getState(),
                              PluginParameters::RELEASE_CONFIG, releaseConfig) {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.

  oscillatorType.addItem(WaveType::SINE, OscillatorTypes::Sine);
  oscillatorType.addItem(WaveType::SAW, OscillatorTypes::Saw);
  oscillatorType.addItem(WaveType::TRIANGLE, OscillatorTypes::Triangle);
  oscillatorTypeAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
          audioProcessor.getState(), PluginParameters::OSCILLATOR_TYPE,
          oscillatorType);

  addAndMakeVisible(keyboard);
  keyboard.setKeyPressBaseOctave(4); // C4
  keyboard.setLowestVisibleKey(48);  // Start C3
  setWantsKeyboardFocus(true);
  keyboard.grabKeyboardFocus();

  addAndMakeVisible(oscillatorType);

  attackConfig.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
  attackConfig.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
  addAndMakeVisible(attackConfig);

  decayConfig.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
  decayConfig.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
  addAndMakeVisible(decayConfig);

  sustainConfig.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
  sustainConfig.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
  addAndMakeVisible(sustainConfig);

  releaseConfig.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
  releaseConfig.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 50);
  addAndMakeVisible(releaseConfig);

  attackLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(attackLabel);

  decayLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(decayLabel);

  sustainLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(sustainLabel);

  releaseLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(releaseLabel);

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
  oscillatorType.setBounds(getWidth() / 2 - 50, 4, 100, 60);

  attackConfig.setBounds(getWidth() / 2 - 200, getHeight() / 2 - 100, 100, 200);
  attackLabel.setBounds(getWidth() / 2 - 200, getHeight() / 2 - 140, 100, 20);

  decayConfig.setBounds(getWidth() / 2 - 100, getHeight() / 2 - 100, 100, 200);
  decayLabel.setBounds(getWidth() / 2 - 100, getHeight() / 2 - 140, 100, 20);

  sustainConfig.setBounds(getWidth() / 2, getHeight() / 2 - 100, 100, 200);
  sustainLabel.setBounds(getWidth() / 2, getHeight() / 2 - 140, 100, 20);

  releaseConfig.setBounds(getWidth() / 2 + 100, getHeight() / 2 - 100, 100,
                          200);
  releaseLabel.setBounds(getWidth() / 2 + 100, getHeight() / 2 - 140, 100, 20);

  auto bounds = getLocalBounds();
  auto kbHeight = 80;
  keyboard.setBounds(bounds.removeFromBottom(kbHeight));
}
