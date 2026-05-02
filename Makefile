PROJECT    := stinky_vst
XCODE_PROJ := $(PROJECT)/Builds/MacOSX/$(PROJECT).xcodeproj
SYMROOT    := $(abspath $(PROJECT)/Builds/MacOSX/build)
CONFIG     ?= Release

APP := $(SYMROOT)/$(CONFIG)/$(PROJECT).app
BIN := $(APP)/Contents/MacOS/$(PROJECT)
XCB := xcodebuild -project $(XCODE_PROJ) -configuration $(CONFIG) SYMROOT=$(SYMROOT)

.PHONY: all vst3 au standalone run run-bin debug compile-db clean list help

all: vst3 au

vst3:
	$(XCB) -scheme "$(PROJECT) - VST3" build

au:
	$(XCB) -scheme "$(PROJECT) - AU" build

standalone:
	$(XCB) -scheme "$(PROJECT) - Standalone Plugin" build

run: standalone
	@echo "Launching $(APP)"
	open $(APP)

run-bin: standalone
	@echo "Running $(BIN) (stdout/stderr in this terminal, Ctrl-C to quit)"
	$(BIN)

debug:
	$(MAKE) CONFIG=Debug run

# Generate compile_flags.txt at the repo root so clangd (nvim LSP) knows
# include paths and defines. Pulls flags from `xcodebuild -showBuildSettings`
# rather than intercepting compiler invocations (avoids macOS SIP blocking
# tools like `bear`). Run after adding/removing sources or upgrading JUCE.
compile-db:
	@scripts/gen-compile-flags.sh $(XCODE_PROJ) $(PROJECT)
	@echo "Restart your nvim LSP (e.g. :LspRestart) to pick it up."

clean:
	rm -rf $(SYMROOT)

list:
	xcodebuild -project $(XCODE_PROJ) -list

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
	@echo "  make run-bin    build standalone (CONFIG) and exec the binary directly"
	@echo "                  (stdout/stderr stays in your terminal)"
	@echo "  make debug      shortcut for CONFIG=Debug make run"
	@echo ""
	@echo "Editor / LSP:"
	@echo "  make compile-db generate compile_flags.txt for clangd"
	@echo ""
	@echo "Other:"
	@echo "  make clean      clean all build products"
	@echo "  make list       list xcode schemes/targets"
	@echo ""
	@echo "Variables:"
	@echo "  CONFIG=Debug|Release   (default: Release)"
	@echo ""
	@echo "Examples:"
	@echo "  make debug                    # build Debug standalone and launch"
	@echo "  make CONFIG=Debug standalone  # build Debug standalone only"
	@echo "  make run-bin                  # launch Release standalone with stdout"
