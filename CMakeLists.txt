cmake_minimum_required(VERSION 3.22)

project(stinky_vst VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Path to your local JUCE checkout. Override on the cmake command line with
# -DJUCE_DIR=/path/to/JUCE if it lives somewhere else.
if(NOT DEFINED JUCE_DIR)
    set(JUCE_DIR "$ENV{HOME}/Developer/JUCE" CACHE PATH "Path to JUCE checkout")
endif()

if(NOT EXISTS "${JUCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "JUCE not found at ${JUCE_DIR}.\n"
        "Either clone JUCE there, or pass -DJUCE_DIR=/path/to/JUCE to cmake.")
endif()

add_subdirectory("${JUCE_DIR}" JUCE EXCLUDE_FROM_ALL)

juce_add_plugin(stinky_vst
    VERSION                     1.0.0
    COMPANY_NAME                "yourcompany"
    BUNDLE_ID                   com.yourcompany.stinky_vst
    PLUGIN_MANUFACTURER_CODE    Manu
    PLUGIN_CODE                 Mdfp
    FORMATS                     VST3 AU Standalone
    PRODUCT_NAME                "stinky_vst"
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD     TRUE
)

# Generates JuceHeader.h so existing #include <JuceHeader.h> keeps working.
juce_generate_juce_header(stinky_vst)

target_sources(stinky_vst
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Square.cpp
        Source/SineWave.cpp
)

target_compile_definitions(stinky_vst
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(stinky_vst
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
