
#ifndef SAWWAVE_H
#define SAWWAVE_H

#include "Oscillator.h"
#include "juce_audio_basics/juce_audio_basics.h"

class SawWave : public Oscillator {
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
  juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative>
      smoothedFreq;
};

#endif // SAWWAVE_H
