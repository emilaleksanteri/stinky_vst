PROJECT  := stinky_vst
CONFIG   ?= Release
JUCE_DIR ?= $(HOME)/Developer/JUCE

# One build dir per config so Debug and Release coexist without reconfiguring.
BUILD := build-$(shell echo $(CONFIG) | tr '[:upper:]' '[:lower:]')

CMAKE_FLAGS := \
  -DCMAKE_BUILD_TYPE=$(CONFIG) \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DJUCE_DIR=$(JUCE_DIR)

CMAKE_BUILD := cmake --build $(BUILD) --target

# JUCE places artifacts under <build>/<target>_artefacts/<Config>/<Format>/.
APP := $(BUILD)/$(PROJECT)_artefacts/$(CONFIG)/Standalone/$(PROJECT).app
BIN := $(APP)/Contents/MacOS/$(PROJECT)

.PHONY: all configure vst3 au standalone run run-bin debug debug-bin clean compile-db list help

all: vst3 au

# Configure once. Re-runs are a no-op when CMakeCache.txt is up to date.
$(BUILD)/CMakeCache.txt:
	cmake -S . -B $(BUILD) $(CMAKE_FLAGS)

configure: $(BUILD)/CMakeCache.txt

vst3: configure
	$(CMAKE_BUILD) $(PROJECT)_VST3

au: configure
	$(CMAKE_BUILD) $(PROJECT)_AU

standalone: configure
	$(CMAKE_BUILD) $(PROJECT)_Standalone

run: standalone
	@echo "Launching $(APP)"
	open $(APP)

run-bin: standalone
	@echo "Running $(BIN) (Ctrl-C to quit)"
	$(BIN)

debug:
	$(MAKE) CONFIG=Debug run

debug-bin:
	$(MAKE) CONFIG=Debug run-bin

# Symlink CMake's compile_commands.json from the Debug build dir to the
# repo root so clangd auto-discovers it. Always uses Debug — that's the
# config you want for editor flags (DEBUG=1, asserts active, etc.).
compile-db:
	cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DJUCE_DIR=$(JUCE_DIR)
	ln -sf build-debug/compile_commands.json compile_commands.json
	@echo ""
	@echo "compile_commands.json -> build-debug/compile_commands.json"
	@echo "Restart your nvim LSP (e.g. :LspRestart)."

clean:
	rm -rf build-debug build-release compile_commands.json

list: configure
	@cmake --build $(BUILD) --target help 2>/dev/null | grep -E "^\.\.\. $(PROJECT)" || cmake --build $(BUILD) --target help

help:
	@echo "Build targets:"
	@echo "  make            same as 'make all'"
	@echo "  make all        build VST3 + AU"
	@echo "  make vst3       build VST3 only"
	@echo "  make au         build AU only"
	@echo "  make standalone build Standalone .app"
	@echo ""
	@echo "Run / debug:"
	@echo "  make run        build standalone (CONFIG) and launch the .app"
	@echo "  make run-bin    build and exec the binary directly (stdout in terminal)"
	@echo "  make debug      shortcut for CONFIG=Debug make run"
	@echo "  make debug-bin  shortcut for CONFIG=Debug make run-bin (logs in terminal)"
	@echo ""
	@echo "Editor / LSP:"
	@echo "  make compile-db generate compile_commands.json for clangd"
	@echo ""
	@echo "Other:"
	@echo "  make configure  run cmake -S . -B \$$BUILD only"
	@echo "  make clean      remove all build dirs and compile_commands.json"
	@echo "  make list       list cmake targets"
	@echo ""
	@echo "Variables:"
	@echo "  CONFIG=Debug|Release   (default: Release; controls build dir name)"
	@echo "  JUCE_DIR=/path/to/JUCE (default: \$$HOME/Developer/JUCE)"
	@echo ""
	@echo "Examples:"
	@echo "  make debug                          # build Debug standalone and launch"
	@echo "  make CONFIG=Debug all               # debug VST3 + AU into build-debug/"
	@echo "  make JUCE_DIR=/opt/JUCE configure   # use a different JUCE checkout"
