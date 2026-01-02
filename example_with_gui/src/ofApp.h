#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxGlic.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);

private:
    // Debug logging
    bool debugMode;
    void log(const std::string& msg);
    void logDebug(const std::string& msg);
    void logError(const std::string& msg);

    // GUI setup helpers
    void setupGUI();
    void updateGUI();

    // Button callbacks
    void onLoadButton();
    void onReloadButton();
    void onEncodeButton();
    void onDecodeButton();
    void onSaveButton();
    void onRandomPreset();

    // Tab switching
    void switchToTab(int tab);

    // Codec helpers
    void readConfigFromGui();
    std::vector<glic::Color> ofImageToGlicColors(const ofImage& img);
    void glicColorsToOfImage(const std::vector<glic::Color>& colors, int width, int height, ofImage& img);

    // Images
    ofImage sourceImage;
    ofImage resultImage;
    ofImage segmentImage;
    ofImage predictionImage;
    ofImage* currentDisplay;

    // Image display
    int displayX, displayY;
    int displayW, displayH;
    int maxDisplaySize;

    // Codec
    glic::GlicCodec codec;

    // File paths
    std::string currentFilePath;
    std::string currentFolder;
    std::string currentFilename;

    // GUI
    bool guiVisible;
    bool separateChannels;
    int activeTab; // 0 = Global, 1-3 = Channels
    int displayMode; // 0=Image, 1=Segm, 2=Pred, 3=Result

    // Main GUI Panel
    ofxPanel mainPanel;

    // Global settings
    ofxLabel titleLabel;
    ofxIntSlider colorSpaceSlider;
    ofxLabel colorSpaceLabel;
    ofxIntSlider borderR, borderG, borderB;
    ofxToggle separateChannelsToggle;
    ofxToggle debugToggle;

    // Action buttons
    ofxButton loadButton;
    ofxButton reloadButton;
    ofxButton encodeButton;
    ofxButton decodeButton;
    ofxButton saveButton;
    ofxButton randomButton;

    // Display buttons
    ofxButton showImageBtn;
    ofxButton showSegmBtn;
    ofxButton showPredBtn;
    ofxButton showResultBtn;

    // Channel settings (in main panel)
    ofxLabel channelLabel;
    ofxIntSlider minBlockSlider;
    ofxIntSlider maxBlockSlider;
    ofxFloatSlider thresholdSlider;
    ofxIntSlider predictionSlider;
    ofxLabel predictionLabel;
    ofxIntSlider quantizationSlider;
    ofxToggle clampToggle;
    ofxIntSlider waveletSlider;
    ofxLabel waveletLabel;
    ofxIntSlider compressionSlider;
    ofxIntSlider scaleSlider;
    ofxToggle wptToggle;
    ofxIntSlider encodingSlider;
    ofxLabel encodingLabel;

    // Per-channel storage (for separate channels mode)
    struct ChannelSettings {
        int minBlock = 1;
        int maxBlock = 8;
        float threshold = 15.0f;
        int prediction = 9;
        int quantization = 110;
        bool clamp = false;
        int wavelet = 28;
        int compression = 0;
        int scale = 20;
        bool wpt = false;
        int encoding = 1;
    };
    ChannelSettings channelSettings[3];

    // Name lists
    std::vector<std::string> colorSpaceNames;
    std::vector<std::string> predictionNames;
    std::vector<std::string> waveletNames;
    std::vector<std::string> encodingNames;
};
