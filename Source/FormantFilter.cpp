#include "FormantFilter.h"
#include "juce_dsp/juce_dsp.h"

void FormantFilter::prepare(double sampleRate) {
  currSampleRate = sampleRate;
  formant1.reset();
  formant2.reset();
  formant3.reset();
}

void FormantFilter::setFreq(float fundamentalFreq) {
  auto c1 = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
      currSampleRate, fundamentalFreq,
      3.0f, // Q
      2.0f  // gain in DB
  );

  auto c2 = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
      currSampleRate,
      fundamentalFreq * 2.0f, // 1st overtone
      2.0f, 1.0f);

  auto c3 = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
      currSampleRate, fundamentalFreq * 3.0f, 1.5f, 0.5f);

  *formant1.coefficients = *c1;
  *formant2.coefficients = *c2;
  *formant3.coefficients = *c3;
}

float FormantFilter::processSample(float input) {
  return formant1.processSample(
      formant2.processSample(formant3.processSample(input)));
}
