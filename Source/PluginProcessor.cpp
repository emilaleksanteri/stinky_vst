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
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_audio_processors_headless/juce_audio_processors_headless.h"
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
}

Stinky_vstAudioProcessor::~Stinky_vstAudioProcessor() {}

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

  oscillators.resize(getTotalNumOutputChannels());
  currSampleRate = sampleRate;

  freqParam = state.getRawParameterValue(PluginParameters::FREQ_HZ);
  playParam = state.getRawParameterValue(PluginParameters::PLAY);
  oscToggleParam =
      state.getRawParameterValue(PluginParameters::OSCILLATOR_TYPE);

  // default oscillator is SineWave
  for (auto &osc : oscillators) {
    osc = std::make_unique<SineWave>();
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

void Stinky_vstAudioProcessor::setOscillatorType(bool useSaw) {
  for (auto &osc : oscillators) {
    float freq = osc->getFrequency();
    float amp = osc->getAmplitude();

    if (useSaw) {
      osc = std::make_unique<SawWave>();
    } else {
      osc = std::make_unique<SineWave>();
    }
    osc->prepare(currSampleRate);
    osc->setFrequency(freq);
    osc->setAmplitude(amp);
  }
}

void Stinky_vstAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                            juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  float freq = freqParam->load();
  bool shouldPlay = static_cast<bool>(playParam->load());
  bool useSaw = static_cast<bool>(oscToggleParam->load());

  if (useSaw != currentlyUsingSaw) {
    currentlyUsingSaw = useSaw;
    oscillatorTypeChanged.store(true);
  }

  if (oscillatorTypeChanged.exchange(false)) {
    setOscillatorType(currentlyUsingSaw);
  }

  for (int channel = 0; channel < totalNumInputChannels; ++channel) {
    auto *channelData = buffer.getWritePointer(channel);
    oscillators[channel]->setFrequency(freq);
    oscillators[channel]->setAmplitude(shouldPlay ? 0.4f : 0.0f);
    oscillators[channel]->process(channelData, buffer.getNumSamples());
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
      std::make_unique<juce::AudioParameterBool>(
          juce::ParameterID{PluginParameters::OSCILLATOR_TYPE},
          "OscillatorType", false),
  };
}
