#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("ofxGlic Realtime Example");
    ofSetFrameRate(30);
    ofBackground(30);

    // Setup camera
    camera.setDesiredFrameRate(30);
    camera.initGrabber(640, 480);

    // Get preset names
    presetNames = ofxGlicPresets::instance().getPresetNames();

    // Setup GUI
    gui.setup("Realtime GLIC");
    gui.add(presetIndex.setup("Preset", 1, 0, presetNames.size() - 1));
    gui.add(quantization.setup("Quantization", 110, 0, 255));
    gui.add(enableEffects.setup("Enable Effects", true));
    gui.add(autoMode.setup("Auto Process", false));
    gui.add(skipFrames.setup("Frame Skip", 5, 1, 30));

    // Setup default effects
    effects.addScanline(30);
    effects.addChromatic(2, 0);

    std::cout << "ofxGlic Realtime Example" << std::endl;
    std::cout << "Press SPACE to capture and process a frame" << std::endl;
    std::cout << "Press A to toggle auto-processing" << std::endl;
    std::cout << "Press 1-0 to switch presets" << std::endl;
}

void ofApp::update() {
    camera.update();

    if (camera.isFrameNew()) {
        frameCounter++;

        if (autoMode && frameCounter >= skipFrames) {
            frameCounter = 0;
            processFrame();
        }
    }
}

void ofApp::processFrame() {
    if (isProcessing) return;

    isProcessing = true;
    uint64_t startTime = ofGetElapsedTimeMillis();

    // Capture current frame
    capturedFrame.setFromPixels(camera.getPixels());

    // Get current preset
    std::string presetName = presetNames[presetIndex];
    auto preset = ofxGlicPresets::instance().getPreset(presetName);

    // Apply preset config with custom quantization
    auto config = preset.codecConfig;
    for (int i = 0; i < 3; i++) {
        config.channels[i].quantizationValue = quantization;
    }
    codec.setConfig(config);

    // Process (encode to memory buffer, then decode - skipping file I/O for speed)
    auto buffer = codec.encodeToBuffer(capturedFrame);
    if (!buffer.empty()) {
        auto result = codec.decodeFromBuffer(buffer);
        if (result.success) {
            processedFrame = result.image;

            // Apply effects if enabled
            if (enableEffects) {
                // First apply preset effects
                for (const auto& e : preset.effects) {
                    ofxGlicEffects::applyEffect(processedFrame, e);
                }
                // Then apply custom effects
                effects.apply(processedFrame);
            }
        }
    }

    lastProcessTime = ofGetElapsedTimeMillis() - startTime;
    isProcessing = false;
}

void ofApp::draw() {
    ofSetColor(255);

    float camW = 640;
    float camH = 480;
    float margin = 20;

    // Camera feed
    ofDrawBitmapString("Camera Input", margin, 35);
    camera.draw(margin, 40, camW, camH);

    // Processed output
    ofDrawBitmapString("GLIC Output", margin + camW + margin, 35);
    if (processedFrame.isAllocated()) {
        processedFrame.draw(margin + camW + margin, 40, camW, camH);
    } else {
        ofSetColor(60);
        ofDrawRectangle(margin + camW + margin, 40, camW, camH);
        ofSetColor(255);
        ofDrawBitmapString("Press SPACE to process", margin + camW + margin + 200, 280);
    }

    // Info
    ofSetColor(255);
    std::string info = "Preset: " + presetNames[presetIndex];
    info += " | Process time: " + ofToString(lastProcessTime) + "ms";
    info += " | FPS: " + ofToString((int)ofGetFrameRate());
    if (autoMode) {
        info += " | AUTO MODE";
    }
    ofDrawBitmapString(info, margin, ofGetHeight() - 40);
    ofDrawBitmapString("SPACE: Process | A: Auto mode | 1-0: Presets | S: Save", margin, ofGetHeight() - 20);

    // Draw GUI
    gui.draw();
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        processFrame();
    } else if (key == 'a' || key == 'A') {
        autoMode = !autoMode;
        std::cout << "Auto mode: " << (autoMode ? "ON" : "OFF") << std::endl;
    } else if (key == 's' || key == 'S') {
        if (processedFrame.isAllocated()) {
            std::string filename = "capture_" + ofToString(ofGetUnixTime()) + ".png";
            processedFrame.save(ofToDataPath(filename));
            std::cout << "Saved: " << filename << std::endl;
        }
    } else if (key >= '1' && key <= '9') {
        int idx = key - '1';
        if (idx < presetNames.size()) {
            presetIndex = idx;
            std::cout << "Switched to preset: " << presetNames[idx] << std::endl;
        }
    } else if (key == '0') {
        if (presetNames.size() > 9) {
            presetIndex = 9;
            std::cout << "Switched to preset: " << presetNames[9] << std::endl;
        }
    }
}

void ofApp::exit() {
    camera.close();
}
