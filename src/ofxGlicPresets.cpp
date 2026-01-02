#include "ofxGlicPresets.h"

ofxGlicPresets& ofxGlicPresets::instance() {
    static ofxGlicPresets instance;
    return instance;
}

ofxGlicPresets::ofxGlicPresets() {
    initBuiltInPresets();
}

void ofxGlicPresets::initBuiltInPresets() {
    addPreset(subtle());
    addPreset(moderate());
    addPreset(extreme());
    addPreset(vhs());
    addPreset(digital());
    addPreset(corrupted());
    addPreset(waves());
    addPreset(blocky());
    addPreset(dreamy());
    addPreset(retro());
}

std::vector<std::string> ofxGlicPresets::getPresetNames() const {
    std::vector<std::string> names;
    for (const auto& pair : presets_) {
        names.push_back(pair.first);
    }
    return names;
}

const ofxGlicPreset& ofxGlicPresets::getPreset(const std::string& name) const {
    auto it = presets_.find(name);
    if (it != presets_.end()) {
        return it->second;
    }
    return defaultPreset_;
}

bool ofxGlicPresets::hasPreset(const std::string& name) const {
    return presets_.find(name) != presets_.end();
}

void ofxGlicPresets::addPreset(const ofxGlicPreset& preset) {
    presets_[preset.name] = preset;
}

// Built-in presets

ofxGlicPreset ofxGlicPresets::subtle() {
    ofxGlicPreset p("Subtle", "Light glitch effect with minimal artifacts");

    auto& cfg = p.codecConfig;
    cfg.colorSpace = glic::ColorSpace::HWB;
    for (int i = 0; i < 3; i++) {
        cfg.channels[i].predictionMethod = glic::PredictionMethod::PAETH;
        cfg.channels[i].quantizationValue = 50;
        cfg.channels[i].waveletType = glic::WaveletType::SYMLET8;
        cfg.channels[i].transformScale = 10;
    }

    return p;
}

ofxGlicPreset ofxGlicPresets::moderate() {
    ofxGlicPreset p("Moderate", "Balanced glitch effect");

    auto& cfg = p.codecConfig;
    cfg.colorSpace = glic::ColorSpace::HWB;
    for (int i = 0; i < 3; i++) {
        cfg.channels[i].predictionMethod = glic::PredictionMethod::PAETH;
        cfg.channels[i].quantizationValue = 110;
        cfg.channels[i].waveletType = glic::WaveletType::SYMLET8;
        cfg.channels[i].transformScale = 20;
    }

    return p;
}

ofxGlicPreset ofxGlicPresets::extreme() {
    ofxGlicPreset p("Extreme", "Heavy glitch with maximum artifacts");

    auto& cfg = p.codecConfig;
    cfg.colorSpace = glic::ColorSpace::YUV;
    for (int i = 0; i < 3; i++) {
        cfg.channels[i].predictionMethod = glic::PredictionMethod::BSAD;
        cfg.channels[i].quantizationValue = 200;
        cfg.channels[i].waveletType = glic::WaveletType::HAAR;
        cfg.channels[i].transformScale = 50;
        cfg.channels[i].segmentationPrecision = 5.0f;
    }

    p.effects.push_back(ofxGlicEffect::glitchShift(16));

    return p;
}

ofxGlicPreset ofxGlicPresets::vhs() {
    ofxGlicPreset p("VHS", "VHS tape-like distortion");

    auto& cfg = p.codecConfig;
    cfg.colorSpace = glic::ColorSpace::YCbCr;
    for (int i = 0; i < 3; i++) {
        cfg.channels[i].predictionMethod = glic::PredictionMethod::H;
        cfg.channels[i].quantizationValue = 80;
        cfg.channels[i].waveletType = glic::WaveletType::HAAR;
    }

    p.effects.push_back(ofxGlicEffect::scanline(40));
    p.effects.push_back(ofxGlicEffect::chromatic(3, 0));

    return p;
}

ofxGlicPreset ofxGlicPresets::digital() {
    ofxGlicPreset p("Digital", "Digital compression artifacts");

    auto& cfg = p.codecConfig;
    cfg.colorSpace = glic::ColorSpace::YCbCr;
    for (int i = 0; i < 3; i++) {
        cfg.channels[i].predictionMethod = glic::PredictionMethod::DC;
        cfg.channels[i].quantizationValue = 150;
        cfg.channels[i].waveletType = glic::WaveletType::DAUBECHIES4;
        cfg.channels[i].minBlockSize = 8;
        cfg.channels[i].maxBlockSize = 64;
    }

    p.effects.push_back(ofxGlicEffect::pixelate(4));

    return p;
}

ofxGlicPreset ofxGlicPresets::corrupted() {
    ofxGlicPreset p("Corrupted", "Simulated file corruption");

    auto& cfg = p.codecConfig;
    cfg.colorSpace = glic::ColorSpace::RGB;
    for (int i = 0; i < 3; i++) {
        cfg.channels[i].predictionMethod = glic::PredictionMethod::RANDOM;
        cfg.channels[i].quantizationValue = 180;
        cfg.channels[i].waveletType = glic::WaveletType::BIORTHOGONAL22;
        cfg.channels[i].transformScale = 40;
    }

    p.effects.push_back(ofxGlicEffect::glitchShift(8, 54321));

    return p;
}

ofxGlicPreset ofxGlicPresets::waves() {
    ofxGlicPreset p("Waves", "Wave-like distortion patterns");

    auto& cfg = p.codecConfig;
    cfg.colorSpace = glic::ColorSpace::HSB;
    for (int i = 0; i < 3; i++) {
        cfg.channels[i].predictionMethod = glic::PredictionMethod::WAVE;
        cfg.channels[i].quantizationValue = 100;
        cfg.channels[i].waveletType = glic::WaveletType::SYMLET4;
    }

    return p;
}

ofxGlicPreset ofxGlicPresets::blocky() {
    ofxGlicPreset p("Blocky", "Block-based artifacts");

    auto& cfg = p.codecConfig;
    cfg.colorSpace = glic::ColorSpace::YUV;
    for (int i = 0; i < 3; i++) {
        cfg.channels[i].predictionMethod = glic::PredictionMethod::CHECKERBOARD;
        cfg.channels[i].quantizationValue = 120;
        cfg.channels[i].waveletType = glic::WaveletType::HAAR;
        cfg.channels[i].minBlockSize = 16;
        cfg.channels[i].maxBlockSize = 128;
    }

    p.effects.push_back(ofxGlicEffect::pixelate(8));

    return p;
}

ofxGlicPreset ofxGlicPresets::dreamy() {
    ofxGlicPreset p("Dreamy", "Soft, ethereal distortion");

    auto& cfg = p.codecConfig;
    cfg.colorSpace = glic::ColorSpace::LAB;
    for (int i = 0; i < 3; i++) {
        cfg.channels[i].predictionMethod = glic::PredictionMethod::GRADIENT;
        cfg.channels[i].quantizationValue = 60;
        cfg.channels[i].waveletType = glic::WaveletType::SYMLET10;
        cfg.channels[i].transformScale = 15;
    }

    p.effects.push_back(ofxGlicEffect::chromatic(2, 1));

    return p;
}

ofxGlicPreset ofxGlicPresets::retro() {
    ofxGlicPreset p("Retro", "Retro computing aesthetic");

    auto& cfg = p.codecConfig;
    cfg.colorSpace = glic::ColorSpace::RGB;
    for (int i = 0; i < 3; i++) {
        cfg.channels[i].predictionMethod = glic::PredictionMethod::NONE;
        cfg.channels[i].quantizationValue = 90;
        cfg.channels[i].waveletType = glic::WaveletType::HAAR;
    }

    p.effects.push_back(ofxGlicEffect::posterize(8));
    p.effects.push_back(ofxGlicEffect::scanline(30));
    p.effects.push_back(ofxGlicEffect::dither(40));

    return p;
}
