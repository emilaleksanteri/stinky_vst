# stinky_vst

A JUCE-based audio plug-in (VST3 + AU).

## Prerequisites

- macOS with Xcode 15+ command line tools (`xcode-select --install`)
- [JUCE](https://github.com/juce-framework/JUCE) checked out somewhere on your machine
- [Projucer](https://juce.com/download/) (ships inside the JUCE repo at `extras/Projucer/`)

## First-time setup

1. Open `stinky_vst/stinky_vst.jucer` in Projucer.
2. In the **Modules** panel, make sure all required JUCE modules point at your local `JUCE/modules/` directory.
3. **File → Save Project** (⌘S). This regenerates `stinky_vst/Builds/MacOSX/stinky_vst.xcodeproj` and the files in `stinky_vst/JuceLibraryCode/`.

The Xcode project is gitignored on purpose — it contains absolute paths that are specific to your machine. The `.jucer` file is the source of truth.

## Building from the CLI

```bash
make            # build VST3 + AU (Release)
make vst3       # VST3 only
make au         # AU only
make standalone # Standalone .app
make clean      # clean all build products
make list       # show xcode schemes
make help
```

Override the config with `CONFIG=`:

```bash
make CONFIG=Debug vst3
```

Build artifacts land in `stinky_vst/Builds/MacOSX/build/<Config>/`. Xcode also auto-installs the plug-ins to:

- VST3 → `~/Library/Audio/Plug-Ins/VST3/`
- AU   → `~/Library/Audio/Plug-Ins/Components/`

Your DAW should pick them up after a rescan.

## Running the Standalone (dev loop)

The Standalone target wraps the plug-in in a small host so you can play with it without a DAW.

```bash
make debug     # build Debug standalone and launch it
make run       # same, but Release config
make run-bin   # same as run, but exec the raw binary so stdout/stderr stay in
               # your terminal (handy for printf/DBG debugging from nvim)
```

The built app lives at `stinky_vst/Builds/MacOSX/build/<Config>/stinky_vst.app`. The raw executable is `stinky_vst.app/Contents/MacOS/stinky_vst` — you can launch it under `lldb` directly if you want to attach a debugger.

## Editor / LSP setup (clangd, e.g. nvim + Mason)

clangd needs a compile database to know about include paths (`<JuceHeader.h>`, all `<juce_*/...>` headers) and the project's preprocessor defines (`JUCE_MODULE_AVAILABLE_*`, `JucePlugin_*`). Without one you'll see hundreds of "header not found" / "unknown type" errors in nvim even though the project builds cleanly.

```bash
make compile-db
```

This invokes `scripts/gen-compile-flags.sh`, which extracts the include paths, preprocessor defines, SDK, and C++ standard from `xcodebuild -showBuildSettings` and writes them to `compile_flags.txt` at the repo root. clangd auto-discovers it from any source file in the tree. Restart the LSP (`:LspRestart` in nvim) after regenerating.

> **Why not `bear` / `compile_commands.json`?** Bear works by injecting a dynamic library to intercept compiler invocations, but macOS SIP blocks DYLD injection into Apple-signed binaries (including `xcodebuild` and the Xcode toolchain), so it produces an empty database on stock macOS. The `-showBuildSettings` approach sidesteps this by asking xcodebuild for the flags directly.

When to regenerate:
- After adding/removing source files in Projucer
- After changing module includes in the `.jucer`
- After upgrading JUCE

The file is gitignored — it contains absolute paths and must be regenerated per machine.

## Known issue: JUCE 8.0.12 + Xcode 15 `StrideIterator` error

Building the Standalone target against the macOS 14.2 SDK fails with:

```
No type named 'value_type' in 'std::iterator_traits<juce::CoreAudioClasses::CoreAudioInternal::StrideIterator<const float *>>'
```

JUCE's `StrideIterator` in `modules/juce_audio_devices/native/juce_CoreAudio_mac.cpp` is missing the iterator typedefs that the stricter libc++ shipped in Xcode 15 requires. Patch the struct definition (around line 1062) by adding these lines at the top:

```cpp
using iterator_category = std::random_access_iterator_tag;
using value_type        = typename std::iterator_traits<Iterator>::value_type;
using difference_type   = ptrdiff_t;
using pointer           = typename std::iterator_traits<Iterator>::pointer;
using reference         = typename std::iterator_traits<Iterator>::reference;
```

The patch lives in your local JUCE checkout, not this repo. It will need to be re-applied if you upgrade JUCE.
