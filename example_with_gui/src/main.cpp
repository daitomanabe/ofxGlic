#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main() {
    ofGLWindowSettings settings;
    settings.setSize(1024, 820);
    settings.windowMode = OF_WINDOW;
    settings.title = "GLIC - GLitch Image Codec";

    auto window = ofCreateWindow(settings);
    ofRunApp(window, make_shared<ofApp>());
    ofRunMainLoop();
}
