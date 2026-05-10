#include "BreathSound.h"
#include "juce_dsp/juce_dsp.h"

void StinkyBreath::prepare(double sampleRate) {
  currSampleRage = sampleRate;
  filter.reset();
}

void StinkyBreath::setFreq(float frequency) {
  auto coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(
      currSampleRage, frequency, iirQFactor);
  *filter.coefficients = *coefficients;
}

void StinkyBreath::setAmount(float amount) { stinkAmount = amount; }

float StinkyBreath::getNextSample() {
  // range between 0,1 to 0,2, shift to -1, 1
  float noise = random.nextFloat() * 2.0f - 1.0f;
  return filter.processSample(noise) * stinkAmount;
}
