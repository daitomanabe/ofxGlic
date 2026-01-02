#pragma once

#include "ofMain.h"
#include "ofxGlic.h"
#include "ofxGui.h"

struct BatchJob {
    std::string inputPath;
    std::string outputPath;
    std::string filename;
    bool processed = false;
    bool success = false;
    std::string error;
};

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
    void dragEvent(ofDragInfo dragInfo) override;

private:
    void scanInputFolder();
    void processNext();
    void processAll();
    void applyPresetToAll(const std::string& presetName);

    ofxGlicCodec codec;
    ofxGlicEffects effects;

    std::vector<BatchJob> jobs;
    int currentJobIndex = -1;
    bool isProcessing = false;
    bool batchMode = false;

    ofImage currentPreview;
    ofImage currentResult;

    std::string inputFolder;
    std::string outputFolder;

    // GUI
    ofxPanel gui;
    ofxIntSlider presetIndex;
    ofxToggle applyEffects;
    ofxLabel statusLabel;
    ofxLabel progressLabel;

    std::vector<std::string> presetNames;
};
