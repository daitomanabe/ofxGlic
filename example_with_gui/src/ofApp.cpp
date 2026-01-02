#include "ofApp.h"
#include <filesystem>

//--------------------------------------------------------------
// Debug Logging
//--------------------------------------------------------------
void ofApp::log(const std::string& msg) {
    ofLogNotice("GLIC") << msg;
}

void ofApp::logDebug(const std::string& msg) {
    if (debugMode) {
        ofLogNotice("GLIC-DEBUG") << msg;
    }
}

void ofApp::logError(const std::string& msg) {
    ofLogError("GLIC") << msg;
}

//--------------------------------------------------------------
// Setup
//--------------------------------------------------------------
void ofApp::setup() {
    ofSetVerticalSync(true);
    ofBackground(40);
    ofSetWindowTitle("GLIC - GLitch Image Codec");

    // Initialize
    debugMode = false;
    guiVisible = true;
    separateChannels = false;
    activeTab = 0;
    displayMode = 0;
    currentDisplay = nullptr;
    maxDisplaySize = 780;
    displayX = 230;
    displayY = 10;

    // Initialize name lists
    colorSpaceNames = {"OHTA", "RGB", "CMY", "HSB", "XYZ", "YXY", "HCL", "LUV",
                       "LAB", "HWB", "R-GGB-G", "YPbPr", "YCbCr", "YDbDr", "GS", "YUV"};

    predictionNames = {"NONE", "CORNER", "H", "V", "DC", "DCMEDIAN", "MEDIAN", "AVG",
                       "TRUEMOTION", "PAETH", "LDIAG", "HV", "JPEGLS", "DIFF", "REF", "ANGLE",
                       "SAD", "BSAD", "RANDOM"};

    waveletNames = {"NONE", "HAAR_ORTH", "BIO11", "BIO13", "BIO15", "BIO22", "BIO24", "BIO26",
                    "BIO28", "BIO31", "BIO33", "BIO35", "BIO37", "BIO39", "BIO44", "BIO55",
                    "BIO68", "COIF1", "COIF2", "COIF3", "COIF4", "COIF5",
                    "SYM2", "SYM3", "SYM4", "SYM5", "SYM6", "SYM7", "SYM8", "SYM9", "SYM10",
                    "DB2", "DB3", "DB4", "DB5", "DB6", "DB7", "DB8", "DB9", "DB10", "HAAR"};

    encodingNames = {"RAW", "PACKED", "RLE"};

    // Initialize channel settings
    for (int i = 0; i < 3; i++) {
        channelSettings[i] = ChannelSettings();
    }

    setupGUI();
    log("GLIC initialized. Drag & drop image or press L to load.");
}

//--------------------------------------------------------------
// GUI Setup
//--------------------------------------------------------------
void ofApp::setupGUI() {
    mainPanel.setup("GLIC Controls", "settings.xml", 10, 10);
    mainPanel.setDefaultWidth(210);

    // Title
    mainPanel.add(titleLabel.setup("", "GLIC v2.0"));

    // Debug toggle
    mainPanel.add(debugToggle.setup("Debug Mode", false));

    // Color space
    mainPanel.add(colorSpaceSlider.setup("ColorSpace", 9, 0, 15));
    mainPanel.add(colorSpaceLabel.setup("", "HWB"));

    // Border color
    mainPanel.add(borderR.setup("Border R", 128, 0, 255));
    mainPanel.add(borderG.setup("Border G", 128, 0, 255));
    mainPanel.add(borderB.setup("Border B", 128, 0, 255));

    // Separate channels
    mainPanel.add(separateChannelsToggle.setup("Separate Channels", false));

    // Action buttons
    mainPanel.add(loadButton.setup("Load Image [L]"));
    mainPanel.add(reloadButton.setup("Reload [R]"));
    mainPanel.add(encodeButton.setup("Encode [E]"));
    mainPanel.add(decodeButton.setup("Decode [D]"));
    mainPanel.add(saveButton.setup("Save Result [S]"));
    mainPanel.add(randomButton.setup("Random Preset"));

    // Display buttons
    mainPanel.add(showImageBtn.setup("View: Source [1]"));
    mainPanel.add(showSegmBtn.setup("View: Segment [2]"));
    mainPanel.add(showPredBtn.setup("View: Predict [3]"));
    mainPanel.add(showResultBtn.setup("View: Result [4]"));

    // Channel settings header
    mainPanel.add(channelLabel.setup("", "--- Channel Settings ---"));

    // Segmentation
    mainPanel.add(minBlockSlider.setup("Min Block 2^", 1, 1, 9));
    mainPanel.add(maxBlockSlider.setup("Max Block 2^", 8, 1, 9));
    mainPanel.add(thresholdSlider.setup("Threshold", 15.0, 5.0, 250.0));

    // Prediction
    mainPanel.add(predictionSlider.setup("Prediction", 9, 0, 18));
    mainPanel.add(predictionLabel.setup("", "PAETH"));

    // Quantization
    mainPanel.add(quantizationSlider.setup("Quantization", 110, 0, 255));
    mainPanel.add(clampToggle.setup("Clamp MOD256", false));

    // Wavelet
    mainPanel.add(waveletSlider.setup("Wavelet", 28, 0, 40));
    mainPanel.add(waveletLabel.setup("", "SYM8"));

    // Transform
    mainPanel.add(compressionSlider.setup("Compression", 0, 0, 255));
    mainPanel.add(scaleSlider.setup("Scale 2^", 20, 2, 24));
    mainPanel.add(wptToggle.setup("WPT Transform", false));

    // Encoding
    mainPanel.add(encodingSlider.setup("Encoding", 1, 0, 2));
    mainPanel.add(encodingLabel.setup("", "PACKED"));

    // Button listeners
    loadButton.addListener(this, &ofApp::onLoadButton);
    reloadButton.addListener(this, &ofApp::onReloadButton);
    encodeButton.addListener(this, &ofApp::onEncodeButton);
    decodeButton.addListener(this, &ofApp::onDecodeButton);
    saveButton.addListener(this, &ofApp::onSaveButton);
    randomButton.addListener(this, &ofApp::onRandomPreset);
}

//--------------------------------------------------------------
// Update
//--------------------------------------------------------------
void ofApp::update() {
    debugMode = debugToggle;
    separateChannels = separateChannelsToggle;

    // Sync debug mode with GLIC codec
    glic::setDebugMode(debugMode);

    // Update labels based on slider values
    int csIdx = colorSpaceSlider;
    if (csIdx >= 0 && csIdx < (int)colorSpaceNames.size()) {
        colorSpaceLabel = colorSpaceNames[csIdx];
    }

    int predIdx = predictionSlider;
    if (predIdx >= 0 && predIdx < (int)predictionNames.size()) {
        predictionLabel = predictionNames[predIdx];
    }

    int wavIdx = waveletSlider;
    if (wavIdx >= 0 && wavIdx < (int)waveletNames.size()) {
        waveletLabel = waveletNames[wavIdx];
    }

    int encIdx = encodingSlider;
    if (encIdx >= 0 && encIdx < (int)encodingNames.size()) {
        encodingLabel = encodingNames[encIdx];
    }
}

//--------------------------------------------------------------
// Draw
//--------------------------------------------------------------
void ofApp::draw() {
    // Draw image
    if (currentDisplay && currentDisplay->isAllocated()) {
        ofSetColor(255);

        float imgW = currentDisplay->getWidth();
        float imgH = currentDisplay->getHeight();

        if (imgW > 0 && imgH > 0) {
            float imgRatio = imgW / imgH;

            if (imgRatio > 1.0f) {
                displayW = maxDisplaySize;
                displayH = (int)(maxDisplaySize / imgRatio);
            } else {
                displayH = maxDisplaySize;
                displayW = (int)(maxDisplaySize * imgRatio);
            }

            int offsetX = (maxDisplaySize - displayW) / 2;
            int offsetY = (maxDisplaySize - displayH) / 2;

            currentDisplay->draw(displayX + offsetX, displayY + offsetY, displayW, displayH);

            // Draw border around image
            ofNoFill();
            ofSetColor(100);
            ofDrawRectangle(displayX + offsetX - 1, displayY + offsetY - 1, displayW + 2, displayH + 2);
            ofFill();
        }
    } else {
        // Draw placeholder
        ofSetColor(80);
        ofDrawRectangle(displayX, displayY, maxDisplaySize, maxDisplaySize);
        ofSetColor(150);
        ofDrawBitmapString("Drag & Drop Image Here\nor Press L to Load", displayX + 280, displayY + 390);
    }

    // Draw GUI
    if (guiVisible) {
        mainPanel.draw();
    }

    // Draw info bar at bottom
    ofSetColor(255);
    std::string info = "GLIC";
    if (!currentFilename.empty()) {
        info += " | " + currentFilename;
    }
    if (currentDisplay && currentDisplay->isAllocated()) {
        info += " | " + std::to_string((int)currentDisplay->getWidth()) + "x" + std::to_string((int)currentDisplay->getHeight());
    }
    info += " | View: ";
    switch (displayMode) {
        case 0: info += "Source"; break;
        case 1: info += "Segmentation"; break;
        case 2: info += "Prediction"; break;
        case 3: info += "Result"; break;
    }
    if (debugMode) {
        info += " | DEBUG ON";
    }
    ofDrawBitmapString(info, displayX, ofGetHeight() - 10);

    // Color space display
    int csIdx = colorSpaceSlider;
    if (csIdx >= 0 && csIdx < (int)colorSpaceNames.size()) {
        ofDrawBitmapString("ColorSpace: " + colorSpaceNames[csIdx], displayX, ofGetHeight() - 25);
    }
}

//--------------------------------------------------------------
// Exit
//--------------------------------------------------------------
void ofApp::exit() {
    loadButton.removeListener(this, &ofApp::onLoadButton);
    reloadButton.removeListener(this, &ofApp::onReloadButton);
    encodeButton.removeListener(this, &ofApp::onEncodeButton);
    decodeButton.removeListener(this, &ofApp::onDecodeButton);
    saveButton.removeListener(this, &ofApp::onSaveButton);
    randomButton.removeListener(this, &ofApp::onRandomPreset);
}

//--------------------------------------------------------------
// Key Events
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    switch (key) {
        case 'l': case 'L':
            onLoadButton();
            break;
        case 'r': case 'R':
            onReloadButton();
            break;
        case 'e': case 'E':
            onEncodeButton();
            break;
        case 'd': case 'D':
            onDecodeButton();
            break;
        case 's': case 'S':
            onSaveButton();
            break;
        case 'h': case 'H':
            guiVisible = !guiVisible;
            break;
        case '1':
            displayMode = 0;
            if (sourceImage.isAllocated()) currentDisplay = &sourceImage;
            break;
        case '2':
            displayMode = 1;
            if (segmentImage.isAllocated()) currentDisplay = &segmentImage;
            break;
        case '3':
            displayMode = 2;
            if (predictionImage.isAllocated()) currentDisplay = &predictionImage;
            break;
        case '4':
            displayMode = 3;
            if (resultImage.isAllocated()) currentDisplay = &resultImage;
            break;
    }
}

void ofApp::keyReleased(int key) {}
void ofApp::mouseMoved(int x, int y) {}
void ofApp::mouseDragged(int x, int y, int button) {}
void ofApp::mousePressed(int x, int y, int button) {}
void ofApp::mouseReleased(int x, int y, int button) {}
void ofApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
// Drag & Drop
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
    if (!dragInfo.files.empty()) {
        std::string path = dragInfo.files[0];
        std::string ext = ofFilePath::getFileExt(path);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        log("Drag & drop: " + path);

        if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp" || ext == "gif" || ext == "tiff") {
            currentFilePath = path;
            currentFolder = ofFilePath::getEnclosingDirectory(path);
            currentFilename = ofFilePath::getFileName(path);
            sourceImage.load(path);
            currentDisplay = &sourceImage;
            displayMode = 0;
            log("Loaded image: " + std::to_string((int)sourceImage.getWidth()) + "x" + std::to_string((int)sourceImage.getHeight()));
        } else if (ext == "glic" || ext == "glc") {
            currentFilePath = path;
            currentFolder = ofFilePath::getEnclosingDirectory(path);
            currentFilename = ofFilePath::getFileName(path);
            onDecodeButton();
        }
    }
}

//--------------------------------------------------------------
// Button Callbacks
//--------------------------------------------------------------
void ofApp::onLoadButton() {
    log("Opening file dialog...");
    ofFileDialogResult result = ofSystemLoadDialog("Select an image", false);
    if (result.bSuccess) {
        currentFilePath = result.getPath();
        currentFolder = ofFilePath::getEnclosingDirectory(result.getPath());
        currentFilename = result.getName();

        std::string ext = ofFilePath::getFileExt(result.getPath());
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == "glic" || ext == "glc") {
            onDecodeButton();
        } else {
            sourceImage.load(result.getPath());
            currentDisplay = &sourceImage;
            displayMode = 0;
            log("Loaded: " + currentFilename + " (" + std::to_string((int)sourceImage.getWidth()) + "x" + std::to_string((int)sourceImage.getHeight()) + ")");
        }
    }
}

void ofApp::onReloadButton() {
    if (!currentFilePath.empty()) {
        std::string ext = ofFilePath::getFileExt(currentFilePath);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext != "glic" && ext != "glc") {
            sourceImage.load(currentFilePath);
            currentDisplay = &sourceImage;
            displayMode = 0;
            log("Reloaded: " + currentFilename);
        }
    }
}

void ofApp::onEncodeButton() {
    try {
        logDebug("=== ENCODE START ===");

        if (!sourceImage.isAllocated()) {
            logError("No source image loaded. Please load an image first.");
            return;
        }

        // Check output path
        if (currentFolder.empty() || currentFilename.empty()) {
            currentFolder = ofToDataPath("", true);
            currentFilename = "untitled.png";
            logDebug("Using default output folder: " + currentFolder);
        }

        int width = sourceImage.getWidth();
        int height = sourceImage.getHeight();

        log("Encoding image: " + std::to_string(width) + "x" + std::to_string(height));

        if (width <= 0 || height <= 0) {
            logError("Invalid image dimensions");
            return;
        }

        // Read config from GUI
        logDebug("Reading config from GUI...");
        readConfigFromGui();

        // Convert to GLIC colors
        logDebug("Converting image to GLIC colors...");
        auto colors = ofImageToGlicColors(sourceImage);
        logDebug("Converted " + std::to_string(colors.size()) + " pixels");

        if (colors.empty()) {
            logError("Failed to convert image to colors");
            return;
        }

        // Output path
        std::string outputPath = currentFolder + "/" + ofFilePath::getBaseName(currentFilename) + ".glic";
        log("Output: " + outputPath);

        // Encode
        logDebug("Calling codec.encode()...");
        auto result = codec.encode(colors.data(), width, height, outputPath);

        if (result.success) {
            log("Encode SUCCESS");

            // Decode to show result
            logDebug("Decoding result for preview...");
            auto decResult = codec.decode(outputPath);
            if (decResult.success) {
                glicColorsToOfImage(decResult.pixels, decResult.width, decResult.height, resultImage);
                currentDisplay = &resultImage;
                displayMode = 3;
                log("Done! Result size: " + std::to_string(decResult.width) + "x" + std::to_string(decResult.height));
            } else {
                logError("Preview decode failed: " + decResult.error);
            }
        } else {
            logError("Encode FAILED: " + result.error);
        }

        logDebug("=== ENCODE END ===");
    } catch (const std::exception& e) {
        logError("Exception during encode: " + std::string(e.what()));
    } catch (...) {
        logError("Unknown exception during encode");
    }
}

void ofApp::onDecodeButton() {
    try {
        logDebug("=== DECODE START ===");

        std::string glicPath;

        if (!currentFilePath.empty()) {
            std::string ext = ofFilePath::getFileExt(currentFilePath);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            if (ext == "glic" || ext == "glc") {
                glicPath = currentFilePath;
            } else {
                glicPath = currentFolder + "/" + ofFilePath::getBaseName(currentFilename) + ".glic";
            }
        }

        if (glicPath.empty() || !ofFile::doesFileExist(glicPath)) {
            ofFileDialogResult result = ofSystemLoadDialog("Select a GLIC file", false);
            if (result.bSuccess) {
                glicPath = result.getPath();
            } else {
                return;
            }
        }

        log("Decoding: " + glicPath);

        auto result = codec.decode(glicPath);

        if (result.success) {
            glicColorsToOfImage(result.pixels, result.width, result.height, resultImage);
            currentDisplay = &resultImage;
            displayMode = 3;
            log("Decode SUCCESS: " + std::to_string(result.width) + "x" + std::to_string(result.height));
        } else {
            logError("Decode FAILED: " + result.error);
        }

        logDebug("=== DECODE END ===");
    } catch (const std::exception& e) {
        logError("Exception during decode: " + std::string(e.what()));
    } catch (...) {
        logError("Unknown exception during decode");
    }
}

void ofApp::onSaveButton() {
    if (!resultImage.isAllocated()) {
        logError("No result image to save");
        return;
    }

    ofFileDialogResult result = ofSystemSaveDialog("result.png", "Save result image");
    if (result.bSuccess) {
        resultImage.save(result.getPath());
        log("Saved: " + result.getPath());
    }
}

void ofApp::onRandomPreset() {
    log("Generating random preset...");

    colorSpaceSlider = ofRandom(0, 16);
    borderR = ofRandom(0, 256);
    borderG = ofRandom(0, 256);
    borderB = ofRandom(0, 256);

    minBlockSlider = ofRandom(1, 5);
    maxBlockSlider = ofRandom(5, 10);
    thresholdSlider = ofRandom(5, 100);
    predictionSlider = ofRandom(0, 19);
    quantizationSlider = ofRandom(0, 200);
    clampToggle = ofRandom(0, 2) > 1;
    waveletSlider = ofRandom(0, 41);
    compressionSlider = ofRandom(0, 100);
    scaleSlider = ofRandom(10, 24);
    wptToggle = ofRandom(0, 2) > 1;
    encodingSlider = ofRandom(0, 3);

    log("Random preset applied");
}

//--------------------------------------------------------------
// Config & Conversion
//--------------------------------------------------------------
void ofApp::readConfigFromGui() {
    glic::CodecConfig config;

    // Global settings
    config.colorSpace = static_cast<glic::ColorSpace>(static_cast<int>(colorSpaceSlider));
    config.borderColorR = borderR;
    config.borderColorG = borderG;
    config.borderColorB = borderB;

    logDebug("Config: ColorSpace=" + colorSpaceNames[colorSpaceSlider] +
             ", Border=(" + std::to_string((int)borderR) + "," +
             std::to_string((int)borderG) + "," + std::to_string((int)borderB) + ")");

    // Channel settings
    for (int p = 0; p < 3; p++) {
        config.channels[p].minBlockSize = 1 << static_cast<int>(minBlockSlider);
        config.channels[p].maxBlockSize = 1 << static_cast<int>(maxBlockSlider);
        config.channels[p].segmentationPrecision = thresholdSlider;

        // Prediction method
        int predIdx = predictionSlider;
        if (predIdx >= 0 && predIdx < 16) {
            config.channels[p].predictionMethod = static_cast<glic::PredictionMethod>(predIdx);
        } else if (predIdx == 16) {
            config.channels[p].predictionMethod = glic::PredictionMethod::SAD;
        } else if (predIdx == 17) {
            config.channels[p].predictionMethod = glic::PredictionMethod::BSAD;
        } else if (predIdx == 18) {
            config.channels[p].predictionMethod = glic::PredictionMethod::RANDOM;
        }

        config.channels[p].quantizationValue = quantizationSlider;
        config.channels[p].clampMethod = clampToggle ? glic::ClampMethod::MOD256 : glic::ClampMethod::NONE;

        config.channels[p].waveletType = static_cast<glic::WaveletType>(static_cast<int>(waveletSlider));
        config.channels[p].transformCompress = compressionSlider;
        config.channels[p].transformScale = 1 << static_cast<int>(scaleSlider);
        config.channels[p].transformType = wptToggle ? glic::TransformType::WPT : glic::TransformType::FWT;

        config.channels[p].encodingMethod = static_cast<glic::EncodingMethod>(static_cast<int>(encodingSlider));

        if (p == 0) {
            logDebug("Channel " + std::to_string(p) + ": Block=" +
                     std::to_string(config.channels[p].minBlockSize) + "-" +
                     std::to_string(config.channels[p].maxBlockSize) +
                     ", Threshold=" + std::to_string(config.channels[p].segmentationPrecision) +
                     ", Pred=" + predictionNames[predIdx] +
                     ", Quant=" + std::to_string(config.channels[p].quantizationValue) +
                     ", Wavelet=" + waveletNames[waveletSlider] +
                     ", Scale=" + std::to_string(config.channels[p].transformScale));
        }
    }

    codec.setConfig(config);
    logDebug("Config applied to codec");
}

std::vector<glic::Color> ofApp::ofImageToGlicColors(const ofImage& img) {
    std::vector<glic::Color> colors;
    int width = img.getWidth();
    int height = img.getHeight();

    logDebug("Converting ofImage " + std::to_string(width) + "x" + std::to_string(height) + " to GLIC colors");

    colors.resize(width * height);
    const ofPixels& pixels = img.getPixels();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            ofColor c = pixels.getColor(x, y);
            colors[idx] = glic::makeColor(c.r, c.g, c.b, c.a);
        }
    }

    logDebug("Conversion complete: " + std::to_string(colors.size()) + " colors");
    return colors;
}

void ofApp::glicColorsToOfImage(const std::vector<glic::Color>& colors, int width, int height, ofImage& img) {
    logDebug("Converting " + std::to_string(colors.size()) + " GLIC colors to ofImage " +
             std::to_string(width) + "x" + std::to_string(height));

    img.allocate(width, height, OF_IMAGE_COLOR_ALPHA);
    ofPixels& pixels = img.getPixels();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            if (idx < (int)colors.size()) {
                glic::Color c = colors[idx];
                pixels.setColor(x, y, ofColor(glic::getR(c), glic::getG(c), glic::getB(c), glic::getA(c)));
            }
        }
    }

    img.update();
    logDebug("Image conversion complete");
}

void ofApp::switchToTab(int tab) {
    activeTab = tab;
}

void ofApp::updateGUI() {
    // Reserved for future use
}
