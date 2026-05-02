/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "juce_graphics/juce_graphics.h"

//==============================================================================
Stinky_vstAudioProcessorEditor::Stinky_vstAudioProcessorEditor(
    Stinky_vstAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.

  addAndMakeVisible(square);

  setSize(800, 800);
}

Stinky_vstAudioProcessorEditor::~Stinky_vstAudioProcessorEditor() {}

//==============================================================================
void Stinky_vstAudioProcessorEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(juce::Colours::black);

  g.setColour(juce::Colours::white);
  g.setFont(juce::FontOptions(15.0f));
  g.drawFittedText("Hello World!", getLocalBounds(),
                   juce::Justification::centred, 1);
}

void Stinky_vstAudioProcessorEditor::resized() {
  square.setBounds(100, 100, 100, 100);
}
