#include "StinkyVoice.h"
#include "juce_audio_basics/juce_audio_basics.h"

void StinkyVoice::prepare(double sr, int maxBlock) {
  setCurrentPlaybackSampleRate(sr);
  scratch.setSize(1, maxBlock, false, false, true);
  scratch.clear();
  breath.prepare(sr);
}

void StinkyVoice::swapOscillator(std::unique_ptr<Oscillator> newOsc) {
  if (getCurrentlyPlayingNote() >= 0) {
    auto note = getCurrentlyPlayingNote();
    newOsc->setFrequency((float)juce::MidiMessage::getMidiNoteInHertz(note));
  }

  osc = std::move(newOsc);
}

bool StinkyVoice::canPlaySound(juce::SynthesiserSound *sound) {
  return dynamic_cast<StinkySound *>(sound) != nullptr;
}

void StinkyVoice::startNote(int note, float vel, juce::SynthesiserSound *,
                            int) {
  currentVelocity = vel;
  float freq = (float)juce::MidiMessage::getMidiNoteInHertz(note);
  osc->setFrequency(freq);
  osc->setAmplitude(1.0f); // default
  breath.setFreq(freq);
  adsr.setParameters(adsrParams);
  adsr.noteOn();
}

void StinkyVoice::stopNote(float vel, bool allowTailOff) {
  if (allowTailOff) {
    adsr.noteOff();
  } else {
    adsr.reset();
    clearCurrentNote();
  }
}

void StinkyVoice::renderNextBlock(juce::AudioBuffer<float> &buffer, int start,
                                  int n) {
  if (!adsr.isActive()) {
    return;
  }

  adsr.setParameters(adsrParams);

  auto *s = scratch.getWritePointer(0);
  osc->process(s, n);

  for (int i = 0; i < n; ++i) {
    s[i] += breath.getNextSample();
  }

  juce::FloatVectorOperations::multiply(s, currentVelocity * 0.4f, n);

  adsr.applyEnvelopeToBuffer(scratch, 0, n);

  for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
    buffer.addFrom(ch, start, scratch, 0, 0, n);
  }

  if (!adsr.isActive()) {
    clearCurrentNote();
  }
}

void StinkyVoice::setCurrentPlaybackSampleRate(double sr) {
  juce::SynthesiserVoice::setCurrentPlaybackSampleRate(sr);
  if (osc) {
    osc->prepare(sr);
  }
  adsr.setSampleRate(sr);
}
