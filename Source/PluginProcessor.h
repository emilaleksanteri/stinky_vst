/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "Oscillator.h"
#include "WaveType.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"
#include <JuceHeader.h>
#include <memory>

//==============================================================================
/**
 */
class Stinky_vstAudioProcessor
    : public juce::AudioProcessor,
      public juce::AudioProcessorValueTreeState::Listener,
      public juce::AsyncUpdater {
public:
  //==============================================================================
  Stinky_vstAudioProcessor();
  ~Stinky_vstAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  //==============================================================================
  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  //==============================================================================
  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState &getState() { return state; }

  void parameterChanged(const juce::String &id, float newValue) override;
  void handleAsyncUpdate() override;

private:
  std::vector<std::unique_ptr<Oscillator>> oscillators;
  std::atomic<double> currSampleRate;

  juce::AudioProcessorValueTreeState state;
  juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
  std::atomic<float> *freqParam;
  std::atomic<float> *playParam;
  std::atomic<float> *oscTypeParam;

  struct OscillatorSwapCommand {
    int channel;
    std::unique_ptr<Oscillator> newOscillator;
  };

  static constexpr int FIFO_SIZE = 16;
  juce::AbstractFifo fifo{FIFO_SIZE};
  OscillatorSwapCommand swapQueue[FIFO_SIZE];

  std::unique_ptr<Oscillator> makeOscillator(OscillatorTypes type);

  void onOscillatorTypeChanged(OscillatorTypes type);
  std::atomic<OscillatorTypes> pendingOscillatorType{OscillatorTypes::Sine};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Stinky_vstAudioProcessor)
};
