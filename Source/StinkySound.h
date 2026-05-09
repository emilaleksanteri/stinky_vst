
#include "juce_audio_basics/juce_audio_basics.h"

class StinkySound : public juce::SynthesiserSound {
public:
  bool appliesToNote(int midiNoteNumber) override { return true; }
  bool appliesToChannel(int midiChannel) override { return true; }
};
