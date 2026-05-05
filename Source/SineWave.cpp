#include "SineWave.h"
#include <cmath>

void SineWave::prepare(double sampleRate) {
  currSampleRate = static_cast<float>(sampleRate);
  smoothedFreq.reset(sampleRate, 1.0f);
  smoothedFreq.setCurrentAndTargetValue(getFrequency());
}

void SineWave::process(float *output, const int numSamples) {
  for (int sample = 0; sample < numSamples; ++sample) {
    float freq = smoothedFreq.getNextValue();
    const float phaseInc = (doublePi * freq) / currSampleRate;

    output[sample] = ampli * std::sinf(currTime);
    currTime += phaseInc;
  }
}
