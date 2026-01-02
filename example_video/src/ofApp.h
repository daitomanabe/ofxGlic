#pragma once

#include "ofMain.h"
#include "ofxGlic.h"
#include "ofxGui.h"

struct ExportJob {
    std::string presetName;
    std::string outputFolder;
    int currentFrame = 0;
    int totalFrames = 0;
    bool completed = false;
};

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
    void dragEvent(ofDragInfo dragInfo) override;
    void exit() override;

private:
    void loadVideo(const std::string& path);
    void startExport();
    void processNextFrame();
    void nextPreset();
    std::string formatTime(float seconds);

    ofVideoPlayer video;
    ofxGlicCodec codec;
    ofxGlicEffects effects;

    ofImage currentFrame;
    ofImage processedFrame;

    bool videoLoaded = false;
    bool isExporting = false;
    bool isPaused = true;

    // Export settings
    std::vector<std::string> selectedPresets;
    std::vector<ExportJob> exportJobs;
    int currentJobIndex = 0;
    int exportedFrames = 0;
    int totalExportFrames = 0;
    float exportStartTime = 0;

    // Frame range
    int startFrame = 0;
    int endFrame = 0;
    int frameStep = 1;

    // GUI
    ofxPanel gui;
    ofxIntSlider startFrameSlider;
    ofxIntSlider endFrameSlider;
    ofxIntSlider frameStepSlider;
    ofxToggle presetSubtle;
    ofxToggle presetModerate;
    ofxToggle presetExtreme;
    ofxToggle presetVHS;
    ofxToggle presetDigital;
    ofxToggle presetCorrupted;
    ofxToggle presetWaves;
    ofxToggle presetBlocky;
    ofxToggle presetDreamy;
    ofxToggle presetRetro;

    std::vector<std::string> presetNames;
};
