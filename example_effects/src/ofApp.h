#pragma once

#include "ofMain.h"
#include "ofxGlic.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
    void dragEvent(ofDragInfo dragInfo) override;

private:
    void processImage();
    void updateEffects();

    ofxGlicCodec codec;
    ofxGlicEffects effects;

    ofImage sourceImage;
    ofImage processedImage;
    ofImage effectsImage;

    bool hasSource = false;
    bool hasProcessed = false;

    // GUI
    ofxPanel gui;
    ofxIntSlider presetIndex;
    ofxToggle applyPixelate;
    ofxToggle applyScanline;
    ofxToggle applyChromatic;
    ofxToggle applyDither;
    ofxToggle applyPosterize;
    ofxToggle applyGlitchShift;

    ofxIntSlider pixelateSize;
    ofxIntSlider scanlineIntensity;
    ofxIntSlider chromaticOffsetX;
    ofxIntSlider chromaticOffsetY;
    ofxIntSlider ditherIntensity;
    ofxIntSlider posterizeLevels;
    ofxIntSlider glitchBlockSize;

    std::vector<std::string> presetNames;
    int currentPreset = 1;
};
