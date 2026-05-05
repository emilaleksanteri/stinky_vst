// Oscillator.h
#ifndef OSCILLATOR_H
#define OSCILLATOR_H

class Oscillator {
public:
  virtual ~Oscillator() = default;

  virtual void prepare(double sampleRate) = 0;
  virtual void process(float *output, int numSamples) = 0;

  [[nodiscard]] virtual float getAmplitude() const = 0;
  [[nodiscard]] virtual float getFrequency() = 0;
  virtual void setAmplitude(float amplitude) = 0;
  virtual void setFrequency(float frequency) = 0;
};

#endif // OSCILLATOR_H
