
#ifndef SINEWAVE_H
#define SINEWAVE_H

#include "juce_audio_basics/juce_audio_basics.h"
#include <numbers>
class SineWave {
public:
  void prepare(double sampleRate);
  void process(float *output, const int numSamples);

  [[nodiscard]] float getAmplitude() const { return ampli; }
  [[nodiscard]] float getFrequency() { return smoothedFreq.getNextValue(); }
  void setAmplitude(const float amplitude) { ampli = amplitude; }
  void setFrequency(const float frequency) {
    smoothedFreq.setTargetValue(frequency);
  }

private:
  float ampli = 0.2f;
  float currSampleRate = 0.0f;
  float currTime = 0.0f;
  static constexpr float doublePi = 2.0f * std::numbers::pi_v<float>;
  juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative>
      smoothedFreq;
};

#endif // SINEWAVE_H
