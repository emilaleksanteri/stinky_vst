#include "Square.h"
#include "juce_graphics/juce_graphics.h"

void Square::paint(juce::Graphics &g) {
  g.fillAll(juce::Colours::lime);

  g.setColour(juce::Colours::black);
  g.setFont(juce::FontOptions(10.0f));
  g.drawFittedText("Square Component", getLocalBounds(),
                   juce::Justification::centred, 1);
}

void Square::resized() {}
