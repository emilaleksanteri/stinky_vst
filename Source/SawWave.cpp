#include "SawWave.h"
#include <cmath>

void SawWave::prepare(double sampleRate) {
  currSampleRate = static_cast<float>(sampleRate);
  smoothedFreq.reset(sampleRate, 0.05f);
  smoothedFreq.setCurrentAndTargetValue(getFrequency());
}

// y(t) = A * (2 * (f * t mod 1) - 1)

void SawWave::process(float *output, const int numSamples) {
  for (int sample = 0; sample < numSamples; ++sample) {
    float freq = smoothedFreq.getNextValue();
    const float phaseInc = freq / currSampleRate;

    output[sample] = ampli * (2.0f * currTime - 1.0f);
    currTime = std::fmodf(currTime + phaseInc, 1.0f);
  }
}
