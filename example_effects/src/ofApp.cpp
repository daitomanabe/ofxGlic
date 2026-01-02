#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("ofxGlic Effects Example");
    ofBackground(40);

    // Get preset names
    presetNames = ofxGlicPresets::instance().getPresetNames();

    // Setup GUI
    gui.setup("Effects");
    gui.add(presetIndex.setup("Preset", 1, 0, presetNames.size() - 1));

    gui.add(applyPixelate.setup("Pixelate", false));
    gui.add(pixelateSize.setup("  Size", 8, 2, 32));

    gui.add(applyScanline.setup("Scanline", false));
    gui.add(scanlineIntensity.setup("  Intensity", 50, 0, 100));

    gui.add(applyChromatic.setup("Chromatic", false));
    gui.add(chromaticOffsetX.setup("  Offset X", 2, 0, 20));
    gui.add(chromaticOffsetY.setup("  Offset Y", 0, 0, 20));

    gui.add(applyDither.setup("Dither", false));
    gui.add(ditherIntensity.setup("  Intensity", 50, 0, 100));

    gui.add(applyPosterize.setup("Posterize", false));
    gui.add(posterizeLevels.setup("  Levels", 4, 2, 16));

    gui.add(applyGlitchShift.setup("Glitch Shift", false));
    gui.add(glitchBlockSize.setup("  Block Size", 8, 4, 64));

    std::cout << "ofxGlic Effects Example" << std::endl;
    std::cout << "Drag and drop an image to load" << std::endl;
    std::cout << "Press SPACE to process with current preset" << std::endl;
    std::cout << "Press E to apply effects" << std::endl;
    std::cout << "Press S to save result" << std::endl;
    std::cout << "Press 1-0 to switch presets" << std::endl;
}

void ofApp::update() {
}

void ofApp::draw() {
    ofSetColor(255);

    float imgW = 380;
    float imgH = 380;
    float margin = 20;
    float y = 60;

    // Source image
    ofDrawBitmapString("Source", margin, y - 5);
    if (hasSource) {
        sourceImage.draw(margin, y, imgW, imgH);
    } else {
        ofSetColor(60);
        ofDrawRectangle(margin, y, imgW, imgH);
        ofSetColor(255);
        ofDrawBitmapString("Drag & drop image", margin + 120, y + 200);
    }

    // Processed image (after GLIC encode)
    ofDrawBitmapString("GLIC Processed", margin + imgW + margin, y - 5);
    if (hasProcessed) {
        processedImage.draw(margin + imgW + margin, y, imgW, imgH);
    } else {
        ofSetColor(60);
        ofDrawRectangle(margin + imgW + margin, y, imgW, imgH);
        ofSetColor(255);
        ofDrawBitmapString("Press SPACE to process", margin + imgW + margin + 100, y + 200);
    }

    // Effects applied
    ofDrawBitmapString("With Effects", margin + (imgW + margin) * 2, y - 5);
    if (effectsImage.isAllocated()) {
        effectsImage.draw(margin + (imgW + margin) * 2, y, imgW, imgH);
    } else {
        ofSetColor(60);
        ofDrawRectangle(margin + (imgW + margin) * 2, y, imgW, imgH);
        ofSetColor(255);
        ofDrawBitmapString("Press E to apply effects", margin + (imgW + margin) * 2 + 100, y + 200);
    }

    // Show current preset
    ofSetColor(255);
    if (presetIndex < presetNames.size()) {
        ofDrawBitmapString("Current Preset: " + presetNames[presetIndex], margin, ofGetHeight() - 40);
    }
    ofDrawBitmapString("SPACE: Process | E: Apply Effects | S: Save | 1-0: Presets", margin, ofGetHeight() - 20);

    // Draw GUI
    gui.draw();
}

void ofApp::processImage() {
    if (!hasSource) return;

    // Get current preset
    std::string presetName = presetNames[presetIndex];
    auto preset = ofxGlicPresets::instance().getPreset(presetName);

    // Apply preset config
    codec.setConfig(preset.codecConfig);

    // Encode and get result
    auto result = codec.encode(sourceImage, ofToDataPath("temp.glc"));
    if (result.success) {
        processedImage = result.image;
        hasProcessed = true;
        std::cout << "Processed with preset: " << presetName << std::endl;
    } else {
        std::cout << "Processing failed: " << result.error << std::endl;
    }
}

void ofApp::updateEffects() {
    if (!hasProcessed) return;

    // Clear and rebuild effects chain
    effects.clear();

    if (applyPixelate) {
        effects.addPixelate(pixelateSize);
    }
    if (applyScanline) {
        effects.addScanline(scanlineIntensity);
    }
    if (applyChromatic) {
        effects.addChromatic(chromaticOffsetX, chromaticOffsetY);
    }
    if (applyDither) {
        effects.addDither(ditherIntensity);
    }
    if (applyPosterize) {
        effects.addPosterize(posterizeLevels);
    }
    if (applyGlitchShift) {
        effects.addGlitchShift(glitchBlockSize);
    }

    // Also add effects from preset
    std::string presetName = presetNames[presetIndex];
    auto preset = ofxGlicPresets::instance().getPreset(presetName);
    for (const auto& e : preset.effects) {
        effects.addEffect(e);
    }

    // Apply effects
    effectsImage = effects.process(processedImage);
    std::cout << "Applied " << effects.getEffects().size() << " effects" << std::endl;
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        processImage();
    } else if (key == 'e' || key == 'E') {
        updateEffects();
    } else if (key == 's' || key == 'S') {
        if (effectsImage.isAllocated()) {
            effectsImage.save(ofToDataPath("result_effects.png"));
            std::cout << "Saved to result_effects.png" << std::endl;
        } else if (hasProcessed) {
            processedImage.save(ofToDataPath("result.png"));
            std::cout << "Saved to result.png" << std::endl;
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

void ofApp::dragEvent(ofDragInfo dragInfo) {
    if (!dragInfo.files.empty()) {
        sourceImage.load(dragInfo.files[0]);
        hasSource = true;
        hasProcessed = false;
        effectsImage.clear();
        std::cout << "Loaded: " << dragInfo.files[0] << std::endl;
    }
}
