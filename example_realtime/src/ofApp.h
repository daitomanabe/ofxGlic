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
    void exit() override;

private:
    void processFrame();

    ofVideoGrabber camera;
    ofxGlicCodec codec;
    ofxGlicEffects effects;

    ofImage capturedFrame;
    ofImage processedFrame;

    bool isProcessing = false;
    bool autoProcess = false;
    int frameSkip = 5;
    int frameCounter = 0;

    // GUI
    ofxPanel gui;
    ofxIntSlider presetIndex;
    ofxIntSlider quantization;
    ofxToggle enableEffects;
    ofxToggle autoMode;
    ofxIntSlider skipFrames;

    std::vector<std::string> presetNames;
    uint64_t lastProcessTime = 0;
};
