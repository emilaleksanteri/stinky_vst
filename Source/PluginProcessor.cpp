/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginParameters.h"
#include "SawWave.h"
#include "SineWave.h"
#include "TriangleWave.h"
#include "WaveType.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_audio_processors_headless/juce_audio_processors_headless.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include <memory>

//==============================================================================
Stinky_vstAudioProcessor::Stinky_vstAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      state(*this, nullptr, "params", createParameters())
#endif
{
  state.addParameterListener(PluginParameters::OSCILLATOR_TYPE, this);
}

Stinky_vstAudioProcessor::~Stinky_vstAudioProcessor() {
  state.removeParameterListener(PluginParameters::OSCILLATOR_TYPE, this);
  cancelPendingUpdate();
}

//==============================================================================
const juce::String Stinky_vstAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool Stinky_vstAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool Stinky_vstAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool Stinky_vstAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double Stinky_vstAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int Stinky_vstAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int Stinky_vstAudioProcessor::getCurrentProgram() { return 0; }

void Stinky_vstAudioProcessor::setCurrentProgram(int index) {}

const juce::String Stinky_vstAudioProcessor::getProgramName(int index) {
  return {};
}

void Stinky_vstAudioProcessor::changeProgramName(int index,
                                                 const juce::String &newName) {}

//==============================================================================
void Stinky_vstAudioProcessor::prepareToPlay(double sampleRate,
                                             int samplesPerBlock) {

  oscillators.resize((size_t)getTotalNumOutputChannels());
  currSampleRate.store(sampleRate);

  freqParam = state.getRawParameterValue(PluginParameters::FREQ_HZ);
  playParam = state.getRawParameterValue(PluginParameters::PLAY);
  oscTypeParam = state.getRawParameterValue(PluginParameters::OSCILLATOR_TYPE);

  fifo.reset();

  auto type = static_cast<OscillatorTypes>((int)oscTypeParam->load());
  for (auto &osc : oscillators) {
    osc = makeOscillator(type);
    osc->prepare(sampleRate);
  }
}

void Stinky_vstAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Stinky_vstAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

std::unique_ptr<Oscillator>
Stinky_vstAudioProcessor::makeOscillator(OscillatorTypes type) {
  switch (type) {
  case OscillatorTypes::Sine:
    return std::make_unique<SineWave>();
  case OscillatorTypes::Saw:
    return std::make_unique<SawWave>();
  case OscillatorTypes::Triangle:
    return std::make_unique<TriangleWave>();
  default:
    return std::make_unique<SineWave>();
  }
}

void Stinky_vstAudioProcessor::onOscillatorTypeChanged(OscillatorTypes type) {
  int neededSize = (int)oscillators.size();

  int start1, size1, start2, size2;
  fifo.prepareToWrite(neededSize, start1, size1, start2, size2);

  if (size1 + size2 < neededSize) {
    fifo.finishedWrite(0);
    return;
  }

  for (int i = 0; i < size1; ++i) {
    auto newOsc = makeOscillator(type);
    newOsc->prepare(currSampleRate.load());
    newOsc->setFrequency(freqParam->load());
    newOsc->setAmplitude(static_cast<bool>(playParam->load()) ? 0.4f : 0.0f);
    swapQueue[start1 + i] = {i, std::move(newOsc)};
  }

  for (int i = 0; i < size2; ++i) {
    int oscIndex = size1 + i;
    auto newOsc = makeOscillator(type);
    newOsc->prepare(currSampleRate.load());
    newOsc->setFrequency(freqParam->load());
    newOsc->setAmplitude(static_cast<bool>(playParam->load()) ? 0.4f : 0.0f);
    swapQueue[start2 + i] = {oscIndex, std::move(newOsc)};
  }

  fifo.finishedWrite(size1 + size2);
}

void Stinky_vstAudioProcessor::parameterChanged(const juce::String &id,
                                                float newValue) {
  if (id == PluginParameters::OSCILLATOR_TYPE) {
    pendingOscillatorType.store(
        static_cast<OscillatorTypes>(static_cast<int>(newValue)));
    triggerAsyncUpdate();
  }
}

void Stinky_vstAudioProcessor::handleAsyncUpdate() {
  onOscillatorTypeChanged(pendingOscillatorType.load());
}

void Stinky_vstAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                            juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // Drain swap queue - no allocation, just pointer swaps
  int start1, size1, start2, size2;
  fifo.prepareToRead(fifo.getNumReady(), start1, size1, start2, size2);

  for (int i = 0; i < size1; ++i)
    oscillators[(size_t)swapQueue[start1 + i].channel] =
        std::move(swapQueue[start1 + i].newOscillator);

  for (int i = 0; i < size2; ++i)
    oscillators[(size_t)swapQueue[start2 + i].channel] =
        std::move(swapQueue[start2 + i].newOscillator);

  fifo.finishedRead(size1 + size2);

  float freq = freqParam->load();
  bool shouldPlay = static_cast<bool>(playParam->load());

  for (int channel = 0; channel < totalNumInputChannels; ++channel) {
    auto *channelData = buffer.getWritePointer(channel);
    oscillators[(size_t)channel]->setFrequency(freq);
    oscillators[(size_t)channel]->setAmplitude(shouldPlay ? 0.4f : 0.0f);
    oscillators[(size_t)channel]->process(channelData, buffer.getNumSamples());
  }
}

//==============================================================================
bool Stinky_vstAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *Stinky_vstAudioProcessor::createEditor() {
  return new Stinky_vstAudioProcessorEditor(*this);
}

//==============================================================================
void Stinky_vstAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
}

void Stinky_vstAudioProcessor::setStateInformation(const void *data,
                                                   int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new Stinky_vstAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout
Stinky_vstAudioProcessor::createParameters() {
  return {
      std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{PluginParameters::FREQ_HZ}, "Frequency", 20.0f,
          20000.0f, 220.0f),
      std::make_unique<juce::AudioParameterBool>(
          juce::ParameterID{PluginParameters::PLAY}, "Play", true),
      std::make_unique<juce::AudioParameterInt>(
          juce::ParameterID{PluginParameters::OSCILLATOR_TYPE},
          "OscillatorType", OscillatorTypes::Sine, OscillatorTypes::Triangle,
          OscillatorTypes::Sine),
  };
}
