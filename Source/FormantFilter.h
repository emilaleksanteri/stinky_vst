
#include "juce_dsp/juce_dsp.h"

class FormantFilter {
public:
  void prepare(double sampleRate);
  void setFreq(float fundamentalFreq);
  float processSample(float input);

private:
  juce::dsp::IIR::Filter<float> formant1;
  juce::dsp::IIR::Filter<float> formant2;
  juce::dsp::IIR::Filter<float> formant3;

  double currSampleRate;
};
