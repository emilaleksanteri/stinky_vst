
#ifndef SINEWAVE_H
#define SINEWAVE_H

class SineWave {
public:
  void prepare(double sampleRate);
  void process(float *output, const int numSamples);

  [[nodiscard]] float getAmplitude() const { return ampli; }
  [[nodiscard]] float getFrequency() const { return freq; }
  void setAmplitude(const float amplitude) { ampli = amplitude; }
  void setFrequency(const float frequency) { freq = frequency; }

private:
  float ampli = 0.2f;
  float freq = 440.0f;
  float currSampleRate = 0.0f;
  float timeIncr = 0.0f;
  float currTime = 0.0f;
};

#endif // SINEWAVE_H
