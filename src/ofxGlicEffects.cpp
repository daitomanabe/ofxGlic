#include "ofxGlicEffects.h"
#include "glic/effects.hpp"

void ofxGlicEffects::addEffect(const ofxGlicEffect& effect) {
    effects_.push_back(effect);
}

void ofxGlicEffects::addEffect(ofxGlicEffectType type) {
    effects_.push_back(ofxGlicEffect(type));
}

void ofxGlicEffects::addPixelate(int blockSize) {
    effects_.push_back(ofxGlicEffect::pixelate(blockSize));
}

void ofxGlicEffects::addScanline(int intensity) {
    effects_.push_back(ofxGlicEffect::scanline(intensity));
}

void ofxGlicEffects::addChromatic(int offsetX, int offsetY) {
    effects_.push_back(ofxGlicEffect::chromatic(offsetX, offsetY));
}

void ofxGlicEffects::addDither(int intensity) {
    effects_.push_back(ofxGlicEffect::dither(intensity));
}

void ofxGlicEffects::addPosterize(int levels) {
    effects_.push_back(ofxGlicEffect::posterize(levels));
}

void ofxGlicEffects::addGlitchShift(int blockSize, uint32_t seed) {
    effects_.push_back(ofxGlicEffect::glitchShift(blockSize, seed));
}

void ofxGlicEffects::clear() {
    effects_.clear();
}

void ofxGlicEffects::apply(ofImage& image) {
    apply(image.getPixels());
    image.update();
}

void ofxGlicEffects::apply(ofPixels& pixels) {
    int width = pixels.getWidth();
    int height = pixels.getHeight();

    // Convert to glic::Color array
    std::vector<glic::Color> colors(width * height);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            ofColor c = pixels.getColor(x, y);
            colors[idx] = glic::makeColor(c.r, c.g, c.b, c.a);
        }
    }

    // Apply each effect
    for (const auto& effect : effects_) {
        glic::applyEffect(colors, width, height, effect.toGlic());
    }

    // Convert back to ofPixels
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            glic::Color c = colors[idx];
            pixels.setColor(x, y, ofColor(glic::getR(c), glic::getG(c), glic::getB(c), glic::getA(c)));
        }
    }
}

ofImage ofxGlicEffects::process(const ofImage& source) {
    ofImage result;
    result.clone(source);
    apply(result);
    return result;
}

void ofxGlicEffects::applyEffect(ofImage& image, const ofxGlicEffect& effect) {
    applyEffect(image.getPixels(), effect);
    image.update();
}

void ofxGlicEffects::applyEffect(ofPixels& pixels, const ofxGlicEffect& effect) {
    int width = pixels.getWidth();
    int height = pixels.getHeight();

    // Convert to glic::Color array
    std::vector<glic::Color> colors(width * height);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            ofColor c = pixels.getColor(x, y);
            colors[idx] = glic::makeColor(c.r, c.g, c.b, c.a);
        }
    }

    // Apply effect
    glic::applyEffect(colors, width, height, effect.toGlic());

    // Convert back to ofPixels
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            glic::Color c = colors[idx];
            pixels.setColor(x, y, ofColor(glic::getR(c), glic::getG(c), glic::getB(c), glic::getA(c)));
        }
    }
}

std::string ofxGlicEffects::getEffectName(ofxGlicEffectType type) {
    return glic::effectName(type);
}

std::vector<std::string> ofxGlicEffects::getEffectNames() {
    return {"None", "Pixelate", "Scanline", "Chromatic Aberration", "Dither", "Posterize", "Glitch Shift"};
}
