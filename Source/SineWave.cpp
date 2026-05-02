#include "SineWave.h"
#include <cmath>
#include <numbers>

void SineWave::prepare(double sampleRate) {
  currSampleRate = static_cast<float>(sampleRate);
  timeIncr = 1.0f / currSampleRate;
}

void SineWave::process(float *output, const int numSamples) {
  for (int sample = 0; sample < numSamples; ++sample) {
    output[sample] =
        ampli * std::sinf(2.0f * std::numbers::pi_v<float> * freq * currTime);
    currTime += timeIncr;
  }
}
