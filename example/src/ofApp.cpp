#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("ofxGlic Example");
    ofBackground(30);

    // Configure codec with default settings
    auto& config = codec.getConfig();
    config.colorSpace = glic::ColorSpace::HWB;
    config.borderColorR = 128;
    config.borderColorG = 128;
    config.borderColorB = 128;

    // Channel 0 settings
    config.channels[0].minBlockSize = 2;
    config.channels[0].maxBlockSize = 256;
    config.channels[0].segmentationPrecision = 15.0f;
    config.channels[0].predictionMethod = glic::PredictionMethod::PAETH;
    config.channels[0].quantizationValue = 110;
    config.channels[0].waveletType = glic::WaveletType::SYMLET8;
    config.channels[0].transformScale = 20;
    config.channels[0].encodingMethod = glic::EncodingMethod::PACKED;

    // Copy channel 0 settings to channels 1 and 2
    config.channels[1] = config.channels[0];
    config.channels[2] = config.channels[0];

    std::cout << "ofxGlic Example" << std::endl;
    std::cout << "Drag and drop an image to encode it" << std::endl;
    std::cout << "Press 'e' to encode, 'd' to decode" << std::endl;
    std::cout << "Press 's' to save result" << std::endl;
}

void ofApp::update() {
}

void ofApp::draw() {
    ofSetColor(255);

    int x = 20;
    int y = 20;

    if (sourceImage.isAllocated()) {
        ofDrawBitmapString("Source Image:", x, y);
        y += 20;
        sourceImage.draw(x, y, 400, 400);
    }

    if (hasResult && resultImage.isAllocated()) {
        ofDrawBitmapString("Result Image:", x + 420, 20);
        resultImage.draw(x + 420, 40, 400, 400);
    }

    // Instructions
    ofDrawBitmapString("Drag & drop image | E: Encode | D: Decode | S: Save", 20, ofGetHeight() - 20);
}

void ofApp::keyPressed(int key) {
    if (key == 'e' || key == 'E') {
        if (sourceImage.isAllocated()) {
            std::cout << "Encoding..." << std::endl;
            auto result = codec.encode(sourceImage, ofToDataPath("output.glc"));
            if (result.success) {
                resultImage = result.image;
                hasResult = true;
                std::cout << "Encoded to output.glc" << std::endl;
            } else {
                std::cout << "Encoding failed: " << result.error << std::endl;
            }
        }
    }
    else if (key == 'd' || key == 'D') {
        auto result = codec.decode(ofToDataPath("output.glc"));
        if (result.success) {
            resultImage = result.image;
            hasResult = true;
            std::cout << "Decoded from output.glc" << std::endl;
        } else {
            std::cout << "Decoding failed: " << result.error << std::endl;
        }
    }
    else if (key == 's' || key == 'S') {
        if (hasResult && resultImage.isAllocated()) {
            resultImage.save(ofToDataPath("result.png"));
            std::cout << "Saved result to result.png" << std::endl;
        }
    }
    else if (key == 'l' || key == 'L') {
        ofFileDialogResult result = ofSystemLoadDialog("Load Image");
        if (result.bSuccess) {
            sourceImage.load(result.getPath());
            std::cout << "Loaded: " << result.getPath() << std::endl;
        }
    }
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
    if (!dragInfo.files.empty()) {
        sourceImage.load(dragInfo.files[0]);
        hasResult = false;
        std::cout << "Loaded: " << dragInfo.files[0] << std::endl;
    }
}
