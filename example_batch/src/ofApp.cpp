#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("ofxGlic Batch Processor");
    ofBackground(40);

    inputFolder = ofToDataPath("input");
    outputFolder = ofToDataPath("output");

    // Get preset names
    presetNames = ofxGlicPresets::instance().getPresetNames();

    // Setup GUI
    gui.setup("Batch Processor");
    gui.add(presetIndex.setup("Preset", 1, 0, presetNames.size() - 1));
    gui.add(applyEffects.setup("Apply Effects", true));
    gui.add(statusLabel.setup("Status", "Ready"));
    gui.add(progressLabel.setup("Progress", "0/0"));

    // Setup default effects
    effects.addScanline(30);

    // Scan input folder
    scanInputFolder();

    std::cout << "ofxGlic Batch Processor" << std::endl;
    std::cout << "Place images in: " << inputFolder << std::endl;
    std::cout << "Output will be saved to: " << outputFolder << std::endl;
    std::cout << "Press ENTER to process all images" << std::endl;
    std::cout << "Press SPACE to process one at a time" << std::endl;
    std::cout << "Press R to rescan input folder" << std::endl;
}

void ofApp::scanInputFolder() {
    jobs.clear();
    currentJobIndex = -1;

    ofDirectory dir(inputFolder);
    dir.allowExt("png");
    dir.allowExt("jpg");
    dir.allowExt("jpeg");
    dir.allowExt("bmp");
    dir.allowExt("tif");
    dir.allowExt("tiff");
    dir.listDir();

    for (size_t i = 0; i < dir.size(); i++) {
        BatchJob job;
        job.inputPath = dir.getPath(i);
        job.filename = dir.getName(i);

        // Create output filename
        std::string baseName = job.filename.substr(0, job.filename.find_last_of('.'));
        job.outputPath = outputFolder + "/" + baseName + "_glitched.png";

        jobs.push_back(job);
    }

    std::cout << "Found " << jobs.size() << " images in input folder" << std::endl;
    progressLabel = "0/" + ofToString(jobs.size());
}

void ofApp::processNext() {
    if (jobs.empty()) {
        statusLabel = "No images to process";
        return;
    }

    // Find next unprocessed job
    currentJobIndex = -1;
    for (size_t i = 0; i < jobs.size(); i++) {
        if (!jobs[i].processed) {
            currentJobIndex = i;
            break;
        }
    }

    if (currentJobIndex < 0) {
        statusLabel = "All images processed!";
        isProcessing = false;
        batchMode = false;
        return;
    }

    isProcessing = true;
    auto& job = jobs[currentJobIndex];
    statusLabel = "Processing: " + job.filename;

    // Load image
    ofImage img;
    if (!img.load(job.inputPath)) {
        job.processed = true;
        job.success = false;
        job.error = "Failed to load image";
        std::cout << "Failed to load: " << job.inputPath << std::endl;

        if (batchMode) {
            processNext();
        }
        return;
    }

    currentPreview = img;

    // Get current preset
    std::string presetName = presetNames[presetIndex];
    auto preset = ofxGlicPresets::instance().getPreset(presetName);

    // Apply preset config
    codec.setConfig(preset.codecConfig);

    // Process
    auto buffer = codec.encodeToBuffer(img);
    if (!buffer.empty()) {
        auto result = codec.decodeFromBuffer(buffer);
        if (result.success) {
            currentResult = result.image;

            // Apply effects if enabled
            if (applyEffects) {
                for (const auto& e : preset.effects) {
                    ofxGlicEffects::applyEffect(currentResult, e);
                }
                effects.apply(currentResult);
            }

            // Save result
            currentResult.save(job.outputPath);

            job.processed = true;
            job.success = true;
            std::cout << "Processed: " << job.filename << " -> " << job.outputPath << std::endl;
        } else {
            job.processed = true;
            job.success = false;
            job.error = result.error;
            std::cout << "Processing failed: " << job.filename << " - " << result.error << std::endl;
        }
    } else {
        job.processed = true;
        job.success = false;
        job.error = "Encoding failed";
    }

    // Update progress
    int processed = 0;
    for (const auto& j : jobs) {
        if (j.processed) processed++;
    }
    progressLabel = ofToString(processed) + "/" + ofToString(jobs.size());

    isProcessing = false;

    if (batchMode) {
        processNext();
    }
}

void ofApp::processAll() {
    batchMode = true;
    processNext();
}

void ofApp::update() {
}

void ofApp::draw() {
    ofSetColor(255);

    float imgW = 400;
    float imgH = 300;
    float margin = 20;

    // Input preview
    ofDrawBitmapString("Input", margin, 55);
    if (currentPreview.isAllocated()) {
        currentPreview.draw(margin, 60, imgW, imgH);
    } else {
        ofSetColor(60);
        ofDrawRectangle(margin, 60, imgW, imgH);
        ofSetColor(255);
    }

    // Output preview
    ofDrawBitmapString("Output", margin + imgW + margin, 55);
    if (currentResult.isAllocated()) {
        currentResult.draw(margin + imgW + margin, 60, imgW, imgH);
    } else {
        ofSetColor(60);
        ofDrawRectangle(margin + imgW + margin, 60, imgW, imgH);
        ofSetColor(255);
    }

    // Job list
    float listY = 380;
    ofDrawBitmapString("Queue (" + ofToString(jobs.size()) + " files):", margin, listY);
    listY += 20;

    int maxDisplay = 10;
    int startIdx = std::max(0, currentJobIndex - 2);
    for (int i = startIdx; i < std::min((int)jobs.size(), startIdx + maxDisplay); i++) {
        const auto& job = jobs[i];
        std::string status = job.processed ? (job.success ? "[OK]" : "[FAIL]") : "[  ]";
        std::string line = status + " " + job.filename;

        if (i == currentJobIndex) {
            ofSetColor(255, 255, 0);
            line = "> " + line;
        } else if (job.processed && job.success) {
            ofSetColor(100, 255, 100);
        } else if (job.processed && !job.success) {
            ofSetColor(255, 100, 100);
        } else {
            ofSetColor(180);
        }

        ofDrawBitmapString(line, margin, listY);
        listY += 15;
    }

    // Instructions
    ofSetColor(255);
    ofDrawBitmapString("ENTER: Process all | SPACE: Process one | R: Rescan | 1-0: Presets", margin, ofGetHeight() - 20);

    // Draw GUI
    gui.draw();
}

void ofApp::keyPressed(int key) {
    if (key == OF_KEY_RETURN) {
        processAll();
    } else if (key == ' ') {
        batchMode = false;
        processNext();
    } else if (key == 'r' || key == 'R') {
        scanInputFolder();
        currentPreview.clear();
        currentResult.clear();
        statusLabel = "Rescanned";
    } else if (key >= '1' && key <= '9') {
        int idx = key - '1';
        if (idx < presetNames.size()) {
            presetIndex = idx;
            std::cout << "Switched to preset: " << presetNames[idx] << std::endl;
        }
    } else if (key == '0') {
        if (presetNames.size() > 9) {
            presetIndex = 9;
        }
    }
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
    // Copy dropped files to input folder
    for (const auto& file : dragInfo.files) {
        ofFile f(file);
        if (f.exists()) {
            std::string ext = f.getExtension();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp" || ext == "tif" || ext == "tiff") {
                std::string dest = inputFolder + "/" + f.getFileName();
                f.copyTo(dest);
                std::cout << "Copied: " << f.getFileName() << " to input folder" << std::endl;
            }
        }
    }

    // Rescan
    scanInputFolder();
}
