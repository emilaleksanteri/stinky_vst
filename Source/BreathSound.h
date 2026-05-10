#ifndef STINKYBREATH_H
#define STINKYBREATH_H

#include "juce_core/juce_core.h"
#include "juce_dsp/juce_dsp.h"
class StinkyBreath {
public:
  void prepare(double sampleRate);
  void setFreq(float frequency);
  void setAmount(float amount);
  float getNextSample();

private:
  double currSampleRage;
  float stinkAmount = 0.7f;
  // 0.5 - 2.0 is a wide band making an airy sound more breath like
  // above e.g., +10 would make a whistly / resonant sound
  // controls the frequency band for the breath
  float iirQFactor = 2.0f;

  juce::Random random;
  juce::dsp::IIR::Filter<float> filter;
  juce::dsp::IIR::Filter<float> filterLow;
};

#endif // BREATHSOUND_H
