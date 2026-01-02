#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("ofxGlic Video Processor");
    ofSetFrameRate(60);
    ofBackground(40);

    presetNames = ofxGlicPresets::instance().getPresetNames();

    // Setup GUI
    gui.setup("Video Export");
    gui.add(startFrameSlider.setup("Start Frame", 0, 0, 100));
    gui.add(endFrameSlider.setup("End Frame", 100, 0, 100));
    gui.add(frameStepSlider.setup("Frame Step", 1, 1, 10));

    gui.add(presetSubtle.setup("Subtle", true));
    gui.add(presetModerate.setup("Moderate", false));
    gui.add(presetExtreme.setup("Extreme", false));
    gui.add(presetVHS.setup("VHS", false));
    gui.add(presetDigital.setup("Digital", false));
    gui.add(presetCorrupted.setup("Corrupted", false));
    gui.add(presetWaves.setup("Waves", false));
    gui.add(presetBlocky.setup("Blocky", false));
    gui.add(presetDreamy.setup("Dreamy", false));
    gui.add(presetRetro.setup("Retro", false));

    std::cout << "=== ofxGlic Video Processor ===" << std::endl;
    std::cout << "Drag and drop a video file to load" << std::endl;
    std::cout << "Select presets and press ENTER to export" << std::endl;
    std::cout << "Output: bin/data/output/<preset_name>/frame_XXXXX.png" << std::endl;
    std::cout << std::endl;
    std::cout << "Tip: Convert image sequence to video with ffmpeg:" << std::endl;
    std::cout << "  ffmpeg -framerate 30 -i frame_%05d.png -c:v libx264 output.mp4" << std::endl;
}

void ofApp::loadVideo(const std::string& path) {
    video.load(path);
    if (video.isLoaded()) {
        video.setLoopState(OF_LOOP_NONE);
        video.play();
        video.setPaused(true);
        video.setFrame(0);

        videoLoaded = true;
        isPaused = true;

        int totalFrames = video.getTotalNumFrames();
        startFrameSlider.setMax(totalFrames - 1);
        endFrameSlider.setMax(totalFrames - 1);
        startFrameSlider = 0;
        endFrameSlider = totalFrames - 1;

        std::cout << "Loaded video: " << path << std::endl;
        std::cout << "  Duration: " << formatTime(video.getDuration()) << std::endl;
        std::cout << "  Frames: " << totalFrames << std::endl;
        std::cout << "  Size: " << video.getWidth() << "x" << video.getHeight() << std::endl;
    } else {
        std::cout << "Failed to load video: " << path << std::endl;
    }
}

void ofApp::startExport() {
    if (!videoLoaded) {
        std::cout << "No video loaded!" << std::endl;
        return;
    }

    // Collect selected presets
    selectedPresets.clear();
    if (presetSubtle) selectedPresets.push_back("Subtle");
    if (presetModerate) selectedPresets.push_back("Moderate");
    if (presetExtreme) selectedPresets.push_back("Extreme");
    if (presetVHS) selectedPresets.push_back("VHS");
    if (presetDigital) selectedPresets.push_back("Digital");
    if (presetCorrupted) selectedPresets.push_back("Corrupted");
    if (presetWaves) selectedPresets.push_back("Waves");
    if (presetBlocky) selectedPresets.push_back("Blocky");
    if (presetDreamy) selectedPresets.push_back("Dreamy");
    if (presetRetro) selectedPresets.push_back("Retro");

    if (selectedPresets.empty()) {
        std::cout << "No presets selected!" << std::endl;
        return;
    }

    // Calculate frame range
    startFrame = startFrameSlider;
    endFrame = endFrameSlider;
    frameStep = frameStepSlider;

    int framesPerPreset = (endFrame - startFrame) / frameStep + 1;

    // Create export jobs
    exportJobs.clear();
    for (const auto& presetName : selectedPresets) {
        ExportJob job;
        job.presetName = presetName;
        job.outputFolder = ofToDataPath("output/" + presetName);
        job.currentFrame = startFrame;
        job.totalFrames = framesPerPreset;
        job.completed = false;

        // Create output folder
        ofDirectory dir(job.outputFolder);
        if (!dir.exists()) {
            dir.create(true);
        }

        exportJobs.push_back(job);
    }

    currentJobIndex = 0;
    exportedFrames = 0;
    totalExportFrames = framesPerPreset * selectedPresets.size();
    exportStartTime = ofGetElapsedTimef();
    isExporting = true;

    std::cout << std::endl;
    std::cout << "=== Starting Export ===" << std::endl;
    std::cout << "Presets: " << selectedPresets.size() << std::endl;
    std::cout << "Frames per preset: " << framesPerPreset << std::endl;
    std::cout << "Total frames: " << totalExportFrames << std::endl;
    std::cout << "Frame range: " << startFrame << " - " << endFrame << " (step: " << frameStep << ")" << std::endl;
    std::cout << std::endl;

    // Set video to start frame
    video.setFrame(startFrame);
    video.update();
}

void ofApp::processNextFrame() {
    if (!isExporting || currentJobIndex >= exportJobs.size()) {
        return;
    }

    auto& job = exportJobs[currentJobIndex];

    // Check if current job is complete
    if (job.currentFrame > endFrame) {
        job.completed = true;
        std::cout << "Completed preset: " << job.presetName << std::endl;
        nextPreset();
        return;
    }

    // Set video frame
    video.setFrame(job.currentFrame);
    video.update();

    // Capture frame
    ofPixels pixels;
    video.getPixels().cropTo(pixels, 0, 0, video.getWidth(), video.getHeight());
    currentFrame.setFromPixels(pixels);

    // Get preset and configure codec
    auto preset = ofxGlicPresets::instance().getPreset(job.presetName);
    codec.setConfig(preset.codecConfig);

    // Process frame
    auto buffer = codec.encodeToBuffer(currentFrame);
    if (!buffer.empty()) {
        auto result = codec.decodeFromBuffer(buffer);
        if (result.success) {
            processedFrame = result.image;

            // Apply preset effects
            for (const auto& e : preset.effects) {
                ofxGlicEffects::applyEffect(processedFrame, e);
            }

            // Save frame
            int frameNum = (job.currentFrame - startFrame) / frameStep;
            std::string filename = job.outputFolder + "/frame_" + ofToString(frameNum, 5, '0') + ".png";
            processedFrame.save(filename);

            exportedFrames++;
        }
    }

    // Move to next frame
    job.currentFrame += frameStep;

    // Progress update
    if (exportedFrames % 10 == 0) {
        float progress = (float)exportedFrames / totalExportFrames * 100.0f;
        float elapsed = ofGetElapsedTimef() - exportStartTime;
        float remaining = (elapsed / exportedFrames) * (totalExportFrames - exportedFrames);

        std::cout << "Progress: " << exportedFrames << "/" << totalExportFrames
                  << " (" << (int)progress << "%) "
                  << "ETA: " << formatTime(remaining) << std::endl;
    }
}

void ofApp::nextPreset() {
    currentJobIndex++;

    if (currentJobIndex >= exportJobs.size()) {
        // All done
        isExporting = false;
        float totalTime = ofGetElapsedTimef() - exportStartTime;

        std::cout << std::endl;
        std::cout << "=== Export Complete ===" << std::endl;
        std::cout << "Total frames: " << exportedFrames << std::endl;
        std::cout << "Total time: " << formatTime(totalTime) << std::endl;
        std::cout << "Average: " << (totalTime / exportedFrames) << " sec/frame" << std::endl;
        std::cout << std::endl;
        std::cout << "Output folders:" << std::endl;
        for (const auto& job : exportJobs) {
            std::cout << "  " << job.outputFolder << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Convert to video with:" << std::endl;
        std::cout << "  ffmpeg -framerate 30 -i output/<preset>/frame_%05d.png -c:v libx264 -pix_fmt yuv420p output.mp4" << std::endl;
    } else {
        std::cout << "Starting preset: " << exportJobs[currentJobIndex].presetName << std::endl;
    }
}

std::string ofApp::formatTime(float seconds) {
    int mins = (int)(seconds / 60);
    int secs = (int)seconds % 60;
    return ofToString(mins) + ":" + ofToString(secs, 2, '0');
}

void ofApp::update() {
    if (isExporting) {
        // Process multiple frames per update for speed
        for (int i = 0; i < 2 && isExporting; i++) {
            processNextFrame();
        }
    } else if (videoLoaded && !isPaused) {
        video.update();
        if (video.isFrameNew()) {
            ofPixels pixels;
            video.getPixels().cropTo(pixels, 0, 0, video.getWidth(), video.getHeight());
            currentFrame.setFromPixels(pixels);
        }
    }
}

void ofApp::draw() {
    ofSetColor(255);

    float margin = 20;
    float vidW = 480;
    float vidH = 360;

    // Video preview
    ofDrawBitmapString("Video Preview", margin, 35);
    if (videoLoaded) {
        if (currentFrame.isAllocated()) {
            currentFrame.draw(margin, 40, vidW, vidH);
        } else {
            video.draw(margin, 40, vidW, vidH);
        }

        // Frame info
        std::string info = "Frame: " + ofToString(video.getCurrentFrame()) + "/" + ofToString(video.getTotalNumFrames());
        info += " | Time: " + formatTime(video.getPosition() * video.getDuration());
        ofDrawBitmapString(info, margin, 40 + vidH + 20);
    } else {
        ofSetColor(60);
        ofDrawRectangle(margin, 40, vidW, vidH);
        ofSetColor(255);
        ofDrawBitmapString("Drag & drop video file", margin + 150, 220);
    }

    // Processed preview
    ofDrawBitmapString("Processed Preview", margin + vidW + margin, 35);
    if (processedFrame.isAllocated()) {
        processedFrame.draw(margin + vidW + margin, 40, vidW, vidH);
    } else {
        ofSetColor(60);
        ofDrawRectangle(margin + vidW + margin, 40, vidW, vidH);
        ofSetColor(255);
    }

    // Export progress
    if (isExporting) {
        ofSetColor(255, 255, 0);
        float progress = (float)exportedFrames / totalExportFrames;
        std::string status = "EXPORTING: " + exportJobs[currentJobIndex].presetName;
        status += " | " + ofToString((int)(progress * 100)) + "%";
        status += " | " + ofToString(exportedFrames) + "/" + ofToString(totalExportFrames);
        ofDrawBitmapString(status, margin, 40 + vidH + 40);

        // Progress bar
        ofSetColor(60);
        ofDrawRectangle(margin, 40 + vidH + 50, vidW * 2 + margin, 20);
        ofSetColor(0, 255, 0);
        ofDrawRectangle(margin, 40 + vidH + 50, (vidW * 2 + margin) * progress, 20);
    }

    // Instructions
    ofSetColor(255);
    ofDrawBitmapString("ENTER: Start Export | SPACE: Play/Pause | LEFT/RIGHT: Seek | ESC: Cancel", margin, ofGetHeight() - 20);

    // Draw GUI
    gui.draw();
}

void ofApp::keyPressed(int key) {
    if (key == OF_KEY_RETURN) {
        if (!isExporting) {
            startExport();
        }
    } else if (key == ' ') {
        if (videoLoaded && !isExporting) {
            isPaused = !isPaused;
            video.setPaused(isPaused);
        }
    } else if (key == OF_KEY_LEFT) {
        if (videoLoaded && !isExporting) {
            int frame = std::max(0, video.getCurrentFrame() - 30);
            video.setFrame(frame);
        }
    } else if (key == OF_KEY_RIGHT) {
        if (videoLoaded && !isExporting) {
            int frame = std::min(video.getTotalNumFrames() - 1, video.getCurrentFrame() + 30);
            video.setFrame(frame);
        }
    } else if (key == OF_KEY_ESC) {
        if (isExporting) {
            isExporting = false;
            std::cout << "Export cancelled!" << std::endl;
        }
    }
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
    if (!dragInfo.files.empty() && !isExporting) {
        loadVideo(dragInfo.files[0]);
    }
}

void ofApp::exit() {
    video.close();
}
