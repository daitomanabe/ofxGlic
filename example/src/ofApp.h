#pragma once

#include "ofMain.h"
#include "ofxGlic.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void dragEvent(ofDragInfo dragInfo);

private:
    ofImage sourceImage;
    ofImage resultImage;
    ofxGlicCodec codec;
    bool hasResult = false;
};
