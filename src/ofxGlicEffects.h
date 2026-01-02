#pragma once

#include "ofMain.h"
#include "glic/effects.hpp"
#include <vector>

// Convenience typedefs
using ofxGlicEffectType = glic::EffectType;

// Effect configuration wrapper for oF
struct ofxGlicEffect {
    ofxGlicEffectType type = ofxGlicEffectType::NONE;
    int intensity = 50;      // 0-100
    int blockSize = 8;       // For pixelate, glitch_shift
    int offsetX = 2;         // For chromatic aberration
    int offsetY = 0;         // For chromatic aberration
    int levels = 4;          // For posterize
    uint32_t seed = 12345;   // For randomized effects

    ofxGlicEffect() = default;
    ofxGlicEffect(ofxGlicEffectType t) : type(t) {}

    // Convert to glic::EffectConfig
    glic::EffectConfig toGlic() const {
        glic::EffectConfig cfg;
        cfg.type = type;
        cfg.intensity = intensity;
        cfg.blockSize = blockSize;
        cfg.offsetX = offsetX;
        cfg.offsetY = offsetY;
        cfg.levels = levels;
        cfg.seed = seed;
        return cfg;
    }

    // Factory methods for common effects
    static ofxGlicEffect pixelate(int blockSize = 8) {
        ofxGlicEffect e(ofxGlicEffectType::PIXELATE);
        e.blockSize = blockSize;
        return e;
    }

    static ofxGlicEffect scanline(int intensity = 50) {
        ofxGlicEffect e(ofxGlicEffectType::SCANLINE);
        e.intensity = intensity;
        return e;
    }

    static ofxGlicEffect chromatic(int offsetX = 2, int offsetY = 0) {
        ofxGlicEffect e(ofxGlicEffectType::CHROMATIC_ABERRATION);
        e.offsetX = offsetX;
        e.offsetY = offsetY;
        return e;
    }

    static ofxGlicEffect dither(int intensity = 50) {
        ofxGlicEffect e(ofxGlicEffectType::DITHER);
        e.intensity = intensity;
        return e;
    }

    static ofxGlicEffect posterize(int levels = 4) {
        ofxGlicEffect e(ofxGlicEffectType::POSTERIZE);
        e.levels = levels;
        return e;
    }

    static ofxGlicEffect glitchShift(int blockSize = 8, uint32_t seed = 12345) {
        ofxGlicEffect e(ofxGlicEffectType::GLITCH_SHIFT);
        e.blockSize = blockSize;
        e.seed = seed;
        return e;
    }
};

// Effects processor class
class ofxGlicEffects {
public:
    ofxGlicEffects() = default;

    // Add effects
    void addEffect(const ofxGlicEffect& effect);
    void addEffect(ofxGlicEffectType type);

    // Quick add methods
    void addPixelate(int blockSize = 8);
    void addScanline(int intensity = 50);
    void addChromatic(int offsetX = 2, int offsetY = 0);
    void addDither(int intensity = 50);
    void addPosterize(int levels = 4);
    void addGlitchShift(int blockSize = 8, uint32_t seed = 12345);

    // Clear all effects
    void clear();

    // Get/set effects list
    std::vector<ofxGlicEffect>& getEffects() { return effects_; }
    const std::vector<ofxGlicEffect>& getEffects() const { return effects_; }
    void setEffects(const std::vector<ofxGlicEffect>& effects) { effects_ = effects; }

    // Apply effects to image
    void apply(ofImage& image);
    void apply(ofPixels& pixels);

    // Apply effects and return new image
    ofImage process(const ofImage& source);

    // Static helper to apply single effect
    static void applyEffect(ofImage& image, const ofxGlicEffect& effect);
    static void applyEffect(ofPixels& pixels, const ofxGlicEffect& effect);

    // Get effect name
    static std::string getEffectName(ofxGlicEffectType type);

    // Get all effect names
    static std::vector<std::string> getEffectNames();

private:
    std::vector<ofxGlicEffect> effects_;
};
