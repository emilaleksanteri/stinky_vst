// PluginParameters.h
#ifndef WAVETYPE_H
#define WAVETYPE_H

enum OscillatorTypes { Sine = 1, Saw, Triangle };

namespace WaveType {
constexpr const char *SINE = "sine";
constexpr const char *SAW = "saw";
constexpr const char *TRIANGLE = "triangle";
} // namespace WaveType

#endif // WAVETYPE_H
