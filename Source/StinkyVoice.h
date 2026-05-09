

#include "Oscillator.h"
#include "SineWave.h"
#include "StinkySound.h"
#include "juce_audio_basics/juce_audio_basics.h"

class StinkyVoice : public juce::SynthesiserVoice {
public:
  void prepare(double sr, int maxBlock); // call from processor.prepareToPlay
  void swapOscillator(std::unique_ptr<Oscillator> newOsc); // audio thread

  bool canPlaySound(juce::SynthesiserSound *) override;
  void startNote(int note, float vel, juce::SynthesiserSound *, int) override;
  void stopNote(float vel, bool allowTailOff) override;
  void renderNextBlock(juce::AudioBuffer<float> &, int start, int n) override;
  void setCurrentPlaybackSampleRate(double sr) override;
  void pitchWheelMoved(int) override {}
  void controllerMoved(int, int) override {}

private:
  std::unique_ptr<Oscillator> osc = std::make_unique<SineWave>();
  juce::ADSR adsr;
  juce::ADSR::Parameters adsrParams; // refreshed from APVTS each block
  juce::AudioBuffer<float> scratch;

  float currentVelocity = 0.0f;
};
