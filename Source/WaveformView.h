#pragma once

#include "PluginParameters.h"
#include "WaveType.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <atomic>
#include <cmath>

// Animated preview of one envelope-shaped note for the currently selected
// oscillator. Reads the oscillator type and ADSR values from the plugin's
// APVTS and redraws at 30 Hz.
class WaveformView : public juce::Component, private juce::Timer {
public:
  explicit WaveformView(juce::AudioProcessorValueTreeState &state)
      : oscType(state.getRawParameterValue(PluginParameters::OSCILLATOR_TYPE)),
        attack(state.getRawParameterValue(PluginParameters::ATTACK_CONFIG)),
        decay(state.getRawParameterValue(PluginParameters::DECAY_CONFIG)),
        sustain(state.getRawParameterValue(PluginParameters::SUSTAIN_CONFIG)),
        release(state.getRawParameterValue(PluginParameters::RELEASE_CONFIG)) {
    startTimerHz(30);
  }

  void setColours(juce::Colour background, juce::Colour border,
                  juce::Colour wave, juce::Colour envelope) {
    bgCol = background;
    borderCol = border;
    waveCol = wave;
    envCol = envelope;
  }

  void paint(juce::Graphics &g) override {
    auto bounds = getLocalBounds().toFloat().reduced(1.5f);
    g.setColour(bgCol);
    g.fillRoundedRectangle(bounds, 6.0f);
    g.setColour(borderCol);
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

    auto plot = bounds.reduced(8.0f, 6.0f);
    const float w = plot.getWidth();
    const float h = plot.getHeight();
    const float midY = plot.getCentreY();
    const float halfH = h * 0.42f;

    // baseline
    g.setColour(borderCol.withAlpha(0.5f));
    g.drawHorizontalLine((int)midY, plot.getX(), plot.getRight());

    const auto type = (OscillatorTypes)(int)oscType->load();
    const float a = std::max(0.001f, attack->load());
    const float d = std::max(0.001f, decay->load());
    const float s = juce::jlimit(0.0f, 1.0f, sustain->load());
    const float r = std::max(0.001f, release->load());
    const float sustainHold = 0.7f;
    const float total = a + d + sustainHold + r;

    constexpr float numCyclesAcross = 14.0f;
    const float displayFreq = numCyclesAcross / total;

    auto envAt = [&](float t) {
      if (t < a)
        return t / a;
      if (t < a + d)
        return 1.0f - ((t - a) / d) * (1.0f - s);
      if (t < a + d + sustainHold)
        return s;
      const float relT = t - (a + d + sustainHold);
      return s * (1.0f - relT / r);
    };

    auto waveAt = [&](float phase) {
      const float p = phase - std::floor(phase);
      switch (type) {
      case OscillatorTypes::Sine:
        return std::sin(2.0f * juce::MathConstants<float>::pi * p);
      case OscillatorTypes::Saw:
        return 2.0f * p - 1.0f;
      case OscillatorTypes::Triangle:
        return 4.0f * std::abs(p - 0.5f) - 1.0f;
      }
      return 0.0f;
    };

    juce::Path wavePath, envUpper, envLower;
    constexpr int N = 360;
    for (int i = 0; i < N; ++i) {
      const float frac = (float)i / (N - 1);
      const float t = frac * total;
      const float env = juce::jlimit(0.0f, 1.0f, envAt(t));
      const float phase = t * displayFreq + scrollPhase;
      const float wave = waveAt(phase);
      const float x = plot.getX() + frac * w;
      const float y = midY - wave * env * halfH;

      if (i == 0) {
        wavePath.startNewSubPath(x, y);
        envUpper.startNewSubPath(x, midY - env * halfH);
        envLower.startNewSubPath(x, midY + env * halfH);
      } else {
        wavePath.lineTo(x, y);
        envUpper.lineTo(x, midY - env * halfH);
        envLower.lineTo(x, midY + env * halfH);
      }
    }

    g.setColour(envCol.withAlpha(0.35f));
    g.strokePath(envUpper, juce::PathStrokeType(1.0f));
    g.strokePath(envLower, juce::PathStrokeType(1.0f));

    g.setColour(waveCol);
    g.strokePath(wavePath, juce::PathStrokeType(1.4f));
  }

private:
  void timerCallback() override {
    scrollPhase += 0.07f;
    if (scrollPhase > 1024.0f)
      scrollPhase -= 1024.0f;
    repaint();
  }

  std::atomic<float> *oscType;
  std::atomic<float> *attack;
  std::atomic<float> *decay;
  std::atomic<float> *sustain;
  std::atomic<float> *release;

  float scrollPhase = 0.0f;

  juce::Colour bgCol = juce::Colour::fromRGB(15, 20, 16);
  juce::Colour borderCol = juce::Colour::fromRGB(70, 110, 60);
  juce::Colour waveCol = juce::Colour::fromRGB(170, 240, 130);
  juce::Colour envCol = juce::Colour::fromRGB(140, 170, 130);
};
