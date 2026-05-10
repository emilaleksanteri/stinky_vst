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

namespace {
const auto kAccent = juce::Colour::fromRGB(170, 240, 130);
const auto kAccentDim = juce::Colour::fromRGB(70, 110, 60);
const auto kPanel = juce::Colour::fromRGB(28, 36, 30);
const auto kPanelDeep = juce::Colour::fromRGB(15, 20, 16);
const auto kBg = juce::Colour::fromRGB(20, 26, 22);
const auto kText = juce::Colour::fromRGB(220, 235, 210);
const auto kSubtle = juce::Colour::fromRGB(140, 170, 130);
} // namespace

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
                              PluginParameters::RELEASE_CONFIG,
                              releaseConfig) {

  // ---- oscillator combo
  oscillatorType.addItem(WaveType::SINE, OscillatorTypes::Sine);
  oscillatorType.addItem(WaveType::SAW, OscillatorTypes::Saw);
  oscillatorType.addItem(WaveType::TRIANGLE, OscillatorTypes::Triangle);
  oscillatorTypeAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
          audioProcessor.getState(), PluginParameters::OSCILLATOR_TYPE,
          oscillatorType);

  oscillatorType.setColour(juce::ComboBox::backgroundColourId, kPanel);
  oscillatorType.setColour(juce::ComboBox::textColourId, kText);
  oscillatorType.setColour(juce::ComboBox::outlineColourId, kAccentDim);
  oscillatorType.setColour(juce::ComboBox::arrowColourId, kAccent);
  oscillatorType.setColour(juce::ComboBox::buttonColourId, kPanel);
  addAndMakeVisible(oscillatorType);

  // ---- ADSR knobs
  auto styleKnob = [&](juce::Slider &s, bool skewToShortValues) {
    s.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 16);
    s.setColour(juce::Slider::rotarySliderFillColourId, kAccent);
    s.setColour(juce::Slider::rotarySliderOutlineColourId, kAccentDim);
    s.setColour(juce::Slider::thumbColourId, kText);
    s.setColour(juce::Slider::textBoxTextColourId, kText);
    s.setColour(juce::Slider::textBoxBackgroundColourId,
                juce::Colours::transparentBlack);
    s.setColour(juce::Slider::textBoxOutlineColourId,
                juce::Colours::transparentBlack);
    s.setNumDecimalPlacesToDisplay(2);
    if (skewToShortValues)
      s.setSkewFactorFromMidPoint(0.3); // finer control near 0
    addAndMakeVisible(s);
  };

  styleKnob(attackConfig, true);
  styleKnob(decayConfig, true);
  styleKnob(sustainConfig, false); // sustain is a level — linear feels right
  styleKnob(releaseConfig, true);

  // ---- labels
  auto styleLabel = [&](juce::Label &l) {
    l.setColour(juce::Label::textColourId, kSubtle);
    l.setJustificationType(juce::Justification::centred);
    l.setFont(juce::FontOptions(12.0f));
    addAndMakeVisible(l);
  };
  styleLabel(attackLabel);
  styleLabel(decayLabel);
  styleLabel(sustainLabel);
  styleLabel(releaseLabel);

  // ---- keyboard
  keyboard.setColour(juce::MidiKeyboardComponent::whiteNoteColourId, kPanel);
  keyboard.setColour(juce::MidiKeyboardComponent::blackNoteColourId,
                     kPanelDeep);
  keyboard.setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId,
                     kAccentDim);
  keyboard.setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId,
                     kAccent.withAlpha(0.4f));
  keyboard.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId,
                     kAccent.withAlpha(0.7f));
  keyboard.setColour(juce::MidiKeyboardComponent::textLabelColourId, kSubtle);
  keyboard.setKeyPressBaseOctave(4);
  keyboard.setLowestVisibleKey(48);
  setWantsKeyboardFocus(true);
  addAndMakeVisible(keyboard);

  setSize(560, 420);
}

Stinky_vstAudioProcessorEditor::~Stinky_vstAudioProcessorEditor() {}

//==============================================================================
void Stinky_vstAudioProcessorEditor::paint(juce::Graphics &g) {
  g.fillAll(kBg);

  // header strip
  auto header = getLocalBounds().removeFromTop(56);
  g.setColour(kPanelDeep);
  g.fillRect(header);
  g.setColour(kAccentDim);
  g.fillRect(header.removeFromBottom(1)); // 1px separator under header

  // title
  auto headerInner = getLocalBounds().removeFromTop(56).reduced(20, 0);
  g.setColour(kAccent);
  g.setFont(juce::FontOptions(28.0f).withStyle("Bold"));
  g.drawFittedText("STINKY", headerInner, juce::Justification::centredLeft, 1);

  g.setColour(kSubtle);
  g.setFont(juce::FontOptions(12.0f));
  g.drawFittedText("polyphonic synth", headerInner,
                   juce::Justification::centredRight, 1);

  // adsr panel background
  auto bounds = getLocalBounds();
  bounds.removeFromTop(56);                         // header
  bounds.removeFromBottom(110);                     // keyboard
  auto adsrPanel = bounds.reduced(16, 8);
  g.setColour(kPanel.withAlpha(0.45f));
  g.fillRoundedRectangle(adsrPanel.toFloat(), 8.0f);
  g.setColour(kAccentDim);
  g.drawRoundedRectangle(adsrPanel.toFloat(), 8.0f, 1.0f);
}

void Stinky_vstAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds();

  bounds.removeFromTop(56); // header (drawn in paint)

  // oscillator combo: top-right of the working area
  auto oscRow = bounds.removeFromTop(48).reduced(20, 8);
  oscillatorType.setBounds(oscRow.removeFromRight(140));

  // keyboard pinned to the bottom
  keyboard.setBounds(bounds.removeFromBottom(110));

  // ADSR knobs row (one column per knob, label on top, knob below)
  auto adsr = bounds.reduced(24, 12);
  const int colW = adsr.getWidth() / 4;
  const int labelH = 18;

  auto layoutKnob = [&](juce::Label &lbl, juce::Slider &knob,
                        juce::Rectangle<int> col) {
    lbl.setBounds(col.removeFromTop(labelH));
    col.removeFromTop(2);
    knob.setBounds(col.reduced(6, 0));
  };

  layoutKnob(attackLabel, attackConfig, adsr.removeFromLeft(colW));
  layoutKnob(decayLabel, decayConfig, adsr.removeFromLeft(colW));
  layoutKnob(sustainLabel, sustainConfig, adsr.removeFromLeft(colW));
  layoutKnob(releaseLabel, releaseConfig, adsr);
}
