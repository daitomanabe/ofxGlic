#pragma once

#include "ofxGlicConfig.h"
#include "ofxGlicEffects.h"
#include <string>
#include <vector>
#include <map>

// Preset structure combining codec config and effects
struct ofxGlicPreset {
    std::string name;
    std::string description;
    glic::CodecConfig codecConfig;
    std::vector<ofxGlicEffect> effects;

    ofxGlicPreset() = default;
    ofxGlicPreset(const std::string& n, const std::string& desc = "")
        : name(n), description(desc) {}
};

// Preset manager with built-in presets
class ofxGlicPresets {
public:
    // Get singleton instance
    static ofxGlicPresets& instance();

    // Get all preset names
    std::vector<std::string> getPresetNames() const;

    // Get preset by name
    const ofxGlicPreset& getPreset(const std::string& name) const;
    bool hasPreset(const std::string& name) const;

    // Add custom preset
    void addPreset(const ofxGlicPreset& preset);

    // Built-in presets - static factory methods
    static ofxGlicPreset subtle();
    static ofxGlicPreset moderate();
    static ofxGlicPreset extreme();
    static ofxGlicPreset vhs();
    static ofxGlicPreset digital();
    static ofxGlicPreset corrupted();
    static ofxGlicPreset waves();
    static ofxGlicPreset blocky();
    static ofxGlicPreset dreamy();
    static ofxGlicPreset retro();

private:
    ofxGlicPresets();
    void initBuiltInPresets();

    std::map<std::string, ofxGlicPreset> presets_;
    ofxGlicPreset defaultPreset_;
};

// Convenience namespace for quick access
namespace ofxGlicPreset_ {
    inline ofxGlicPreset subtle() { return ofxGlicPresets::subtle(); }
    inline ofxGlicPreset moderate() { return ofxGlicPresets::moderate(); }
    inline ofxGlicPreset extreme() { return ofxGlicPresets::extreme(); }
    inline ofxGlicPreset vhs() { return ofxGlicPresets::vhs(); }
    inline ofxGlicPreset digital() { return ofxGlicPresets::digital(); }
    inline ofxGlicPreset corrupted() { return ofxGlicPresets::corrupted(); }
    inline ofxGlicPreset waves() { return ofxGlicPresets::waves(); }
    inline ofxGlicPreset blocky() { return ofxGlicPresets::blocky(); }
    inline ofxGlicPreset dreamy() { return ofxGlicPresets::dreamy(); }
    inline ofxGlicPreset retro() { return ofxGlicPresets::retro(); }
}
