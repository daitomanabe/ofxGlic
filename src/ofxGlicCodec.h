#pragma once

#include "ofMain.h"
#include "ofxGlicConfig.h"
#include "ofxGlicEffects.h"
#include <vector>
#include <string>

// Forward declaration
namespace glic {
    class GlicCodec;
    struct GlicResult;
}

// Result structure for ofxGlic operations
struct ofxGlicResult {
    ofImage image;
    bool success = false;
    std::string error;

    operator bool() const { return success; }
};

// Main ofxGlic codec class - wraps glic::GlicCodec with oF-friendly API
class ofxGlicCodec {
public:
    ofxGlicCodec();
    ~ofxGlicCodec();

    // Configuration
    void setConfig(const glic::CodecConfig& config);
    glic::CodecConfig& getConfig();
    const glic::CodecConfig& getConfig() const;

    // Set individual config values
    void setColorSpace(glic::ColorSpace cs);
    void setBorderColor(uint8_t r, uint8_t g, uint8_t b);
    void setBorderColor(const ofColor& c);

    // Channel configuration
    void setChannelConfig(int channel, const glic::ChannelConfig& config);
    glic::ChannelConfig& getChannelConfig(int channel);

    // Encoding - from ofImage
    ofxGlicResult encode(const ofImage& source, const std::string& outputPath);

    // Encoding - to memory buffer
    std::vector<uint8_t> encodeToBuffer(const ofImage& source);

    // Decoding - from file
    ofxGlicResult decode(const std::string& inputPath);

    // Decoding - from memory buffer
    ofxGlicResult decodeFromBuffer(const std::vector<uint8_t>& buffer);

    // Post-processing effects
    void setPostEffects(const std::vector<ofxGlicEffect>& effects);
    void addPostEffect(const ofxGlicEffect& effect);
    void clearPostEffects();
    ofxGlicEffects& getPostEffects() { return postEffects_; }
    const ofxGlicEffects& getPostEffects() const { return postEffects_; }

    // Apply effects to decoded result
    void applyEffects(ofImage& image);

    // Quick encode/decode (static methods)
    static bool encodeImage(const ofImage& source, const std::string& outputPath,
                            const glic::CodecConfig& config = glic::CodecConfig());
    static ofxGlicResult decodeImage(const std::string& inputPath);

    // Convert between ofImage and glic::Color array
    static std::vector<glic::Color> toGlicColors(const ofImage& img);
    static void toOfImage(const std::vector<glic::Color>& colors, int width, int height, ofImage& img);

private:
    std::unique_ptr<glic::GlicCodec> codec_;
    glic::CodecConfig config_;
    ofxGlicEffects postEffects_;
    bool applyPostEffects_ = true;
};
