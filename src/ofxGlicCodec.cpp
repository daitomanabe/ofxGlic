#include "ofxGlicCodec.h"
#include "glic/glic.hpp"

ofxGlicCodec::ofxGlicCodec() : codec_(std::make_unique<glic::GlicCodec>()), config_() {
    codec_->setConfig(config_);
}

ofxGlicCodec::~ofxGlicCodec() = default;

void ofxGlicCodec::setConfig(const glic::CodecConfig& config) {
    config_ = config;
    codec_->setConfig(config);
}

glic::CodecConfig& ofxGlicCodec::getConfig() {
    return config_;
}

const glic::CodecConfig& ofxGlicCodec::getConfig() const {
    return config_;
}

void ofxGlicCodec::setColorSpace(glic::ColorSpace cs) {
    config_.colorSpace = cs;
    codec_->setConfig(config_);
}

void ofxGlicCodec::setBorderColor(uint8_t r, uint8_t g, uint8_t b) {
    config_.borderColorR = r;
    config_.borderColorG = g;
    config_.borderColorB = b;
    codec_->setConfig(config_);
}

void ofxGlicCodec::setBorderColor(const ofColor& c) {
    setBorderColor(c.r, c.g, c.b);
}

void ofxGlicCodec::setChannelConfig(int channel, const glic::ChannelConfig& config) {
    if (channel >= 0 && channel < 3) {
        config_.channels[channel] = config;
        codec_->setConfig(config_);
    }
}

glic::ChannelConfig& ofxGlicCodec::getChannelConfig(int channel) {
    return config_.channels[std::min(2, std::max(0, channel))];
}

std::vector<glic::Color> ofxGlicCodec::toGlicColors(const ofImage& img) {
    std::vector<glic::Color> colors;
    int width = img.getWidth();
    int height = img.getHeight();
    colors.resize(width * height);

    const ofPixels& pixels = img.getPixels();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            ofColor c = pixels.getColor(x, y);
            colors[idx] = glic::makeColor(c.r, c.g, c.b, c.a);
        }
    }
    return colors;
}

void ofxGlicCodec::toOfImage(const std::vector<glic::Color>& colors, int width, int height, ofImage& img) {
    img.allocate(width, height, OF_IMAGE_COLOR_ALPHA);
    ofPixels& pixels = img.getPixels();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            glic::Color c = colors[idx];
            pixels.setColor(x, y, ofColor(glic::getR(c), glic::getG(c), glic::getB(c), glic::getA(c)));
        }
    }
    img.update();
}

ofxGlicResult ofxGlicCodec::encode(const ofImage& source, const std::string& outputPath) {
    ofxGlicResult result;

    auto colors = toGlicColors(source);
    int width = source.getWidth();
    int height = source.getHeight();

    auto glicResult = codec_->encode(colors.data(), width, height, outputPath);

    if (glicResult.success) {
        result.success = true;
        toOfImage(glicResult.pixels, glicResult.width, glicResult.height, result.image);
    } else {
        result.success = false;
        result.error = glicResult.error;
    }

    return result;
}

std::vector<uint8_t> ofxGlicCodec::encodeToBuffer(const ofImage& source) {
    auto colors = toGlicColors(source);
    int width = source.getWidth();
    int height = source.getHeight();

    return codec_->encodeToBuffer(colors.data(), width, height);
}

ofxGlicResult ofxGlicCodec::decode(const std::string& inputPath) {
    ofxGlicResult result;

    auto glicResult = codec_->decode(inputPath);

    if (glicResult.success) {
        result.success = true;
        toOfImage(glicResult.pixels, glicResult.width, glicResult.height, result.image);
    } else {
        result.success = false;
        result.error = glicResult.error;
    }

    return result;
}

ofxGlicResult ofxGlicCodec::decodeFromBuffer(const std::vector<uint8_t>& buffer) {
    ofxGlicResult result;

    auto glicResult = codec_->decodeFromBuffer(buffer);

    if (glicResult.success) {
        result.success = true;
        toOfImage(glicResult.pixels, glicResult.width, glicResult.height, result.image);
    } else {
        result.success = false;
        result.error = glicResult.error;
    }

    return result;
}

bool ofxGlicCodec::encodeImage(const ofImage& source, const std::string& outputPath,
                               const glic::CodecConfig& config) {
    ofxGlicCodec codec;
    codec.setConfig(config);
    auto result = codec.encode(source, outputPath);
    return result.success;
}

ofxGlicResult ofxGlicCodec::decodeImage(const std::string& inputPath) {
    ofxGlicCodec codec;
    return codec.decode(inputPath);
}

void ofxGlicCodec::setPostEffects(const std::vector<ofxGlicEffect>& effects) {
    postEffects_.setEffects(effects);
}

void ofxGlicCodec::addPostEffect(const ofxGlicEffect& effect) {
    postEffects_.addEffect(effect);
}

void ofxGlicCodec::clearPostEffects() {
    postEffects_.clear();
}

void ofxGlicCodec::applyEffects(ofImage& image) {
    postEffects_.apply(image);
}
