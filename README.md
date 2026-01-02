# ofxGlic

openFrameworks addon for [GLIC (GLitch Image Codec)](https://github.com/daitomanabe/glic-cpp) - a creative image codec designed for generating glitch art effects.

## Overview

ofxGlic wraps the GLIC C++ library to provide an openFrameworks-friendly API for encoding and decoding images with intentional glitch artifacts. GLIC uses various prediction algorithms, wavelet transforms, and quantization techniques to create unique visual effects.

## Features

- 16 color spaces (RGB, HSB, LAB, YUV, etc.)
- 24 prediction algorithms (PAETH, SPIRAL, WAVE, RADIAL, etc.)
- Multiple wavelet families (Haar, Daubechies, Symlet, Coiflet)
- 6 encoding methods (raw, packed, rle, delta, xor, zigzag)
- **6 post-processing effects** (pixelate, scanline, chromatic aberration, dither, posterize, glitch shift)
- **10 built-in presets** (Subtle, Moderate, Extreme, VHS, Digital, etc.)
- Per-channel configuration for fine-grained control
- Simple ofImage integration

## Installation

### Clone with submodules

```bash
cd openFrameworks/addons
git clone --recursive https://github.com/daitomanabe/ofxGlic.git
```

### Or add submodule manually

```bash
cd openFrameworks/addons
git clone https://github.com/daitomanabe/ofxGlic.git
cd ofxGlic
git submodule update --init
```

## Usage

### Basic Example

```cpp
#include "ofxGlic.h"

ofxGlicCodec codec;
ofImage sourceImage;
ofImage resultImage;

void ofApp::setup() {
    sourceImage.load("input.png");

    // Configure codec
    auto& config = codec.getConfig();
    config.colorSpace = glic::ColorSpace::HWB;
    config.channels[0].predictionMethod = glic::PredictionMethod::PAETH;
    config.channels[0].quantizationValue = 110;
    config.channels[0].waveletType = glic::WaveletType::SYMLET8;

    // Copy settings to all channels
    config.channels[1] = config.channels[0];
    config.channels[2] = config.channels[0];
}

void ofApp::encode() {
    auto result = codec.encode(sourceImage, ofToDataPath("output.glc"));
    if (result.success) {
        resultImage = result.image;
    }
}

void ofApp::decode() {
    auto result = codec.decode(ofToDataPath("output.glc"));
    if (result.success) {
        resultImage = result.image;
    }
}
```

### Using Presets

```cpp
#include "ofxGlic.h"

void ofApp::setup() {
    // Get a preset
    auto preset = ofxGlicPresets::vhs();

    // Apply preset config to codec
    codec.setConfig(preset.codecConfig);

    // Preset includes recommended effects
    for (const auto& effect : preset.effects) {
        codec.addPostEffect(effect);
    }
}

// Available presets:
// ofxGlicPresets::subtle()    - Light glitch effect
// ofxGlicPresets::moderate()  - Balanced glitch
// ofxGlicPresets::extreme()   - Heavy artifacts
// ofxGlicPresets::vhs()       - VHS tape-like
// ofxGlicPresets::digital()   - Digital compression
// ofxGlicPresets::corrupted() - File corruption
// ofxGlicPresets::waves()     - Wave distortion
// ofxGlicPresets::blocky()    - Block artifacts
// ofxGlicPresets::dreamy()    - Soft, ethereal
// ofxGlicPresets::retro()     - Retro computing
```

### Using Effects

```cpp
#include "ofxGlic.h"

ofxGlicEffects effects;
ofImage processedImage;

void ofApp::setup() {
    // Add effects chain
    effects.addPixelate(8);
    effects.addScanline(50);
    effects.addChromatic(3, 0);
    effects.addPosterize(4);
    effects.addGlitchShift(16);
}

void ofApp::applyEffects() {
    // Apply effects to an image
    effects.apply(processedImage);

    // Or create a new processed image
    ofImage result = effects.process(sourceImage);
}

// Effect factory methods:
// ofxGlicEffect::pixelate(blockSize)
// ofxGlicEffect::scanline(intensity)
// ofxGlicEffect::chromatic(offsetX, offsetY)
// ofxGlicEffect::dither(intensity)
// ofxGlicEffect::posterize(levels)
// ofxGlicEffect::glitchShift(blockSize, seed)
```

### Quick Encode/Decode

```cpp
// Static methods for one-off operations
ofxGlicCodec::encodeImage(sourceImage, "output.glc");
auto result = ofxGlicCodec::decodeImage("output.glc");
```

## Configuration Options

### Color Spaces

| Name | Description |
|------|-------------|
| RGB | Standard RGB |
| HSB | Hue, Saturation, Brightness |
| HWB | Hue, Whiteness, Blackness |
| LAB | CIE L*a*b* |
| YUV | Analog video |
| OHTA | Optimal color transform |
| ... | And 10 more |

### Prediction Methods

| Name | Glitch Characteristic |
|------|----------------------|
| PAETH | PNG-style prediction |
| SPIRAL | Spiral artifacts from center |
| WAVE | Wave-like distortions |
| RADIAL | Radial artifacts |
| CHECKERBOARD | Checkerboard pattern |
| NOISE | Noise-like texture |
| BSAD | Maximum glitch (worst prediction) |

### Post-Processing Effects

| Effect | Description | Parameters |
|--------|-------------|------------|
| Pixelate | Mosaic effect | blockSize |
| Scanline | CRT monitor lines | intensity (0-100) |
| Chromatic | RGB channel offset | offsetX, offsetY |
| Dither | Bayer dithering | intensity (0-100) |
| Posterize | Color reduction | levels (2-16) |
| Glitch Shift | Random row shifting | blockSize, seed |

### Channel Configuration

```cpp
glic::ChannelConfig channelConfig;
channelConfig.minBlockSize = 2;           // Min quad-tree block size
channelConfig.maxBlockSize = 256;         // Max quad-tree block size
channelConfig.segmentationPrecision = 15; // Segmentation threshold
channelConfig.predictionMethod = glic::PredictionMethod::PAETH;
channelConfig.quantizationValue = 110;    // 0-255, higher = more glitch
channelConfig.waveletType = glic::WaveletType::SYMLET8;
channelConfig.transformScale = 20;
channelConfig.encodingMethod = glic::EncodingMethod::PACKED;
```

## Examples

This addon includes six example projects:

| Example | Description |
|---------|-------------|
| **example/** | Basic encode/decode with drag-and-drop |
| **example_with_gui/** | Interactive GUI for adjusting parameters |
| **example_effects/** | Post-processing effects demo with presets |
| **example_realtime/** | Live camera input with real-time glitching |
| **example_batch/** | Batch process multiple images from folder |
| **example_video/** | Process video with multiple presets, export as image sequence |

### Building Examples

```bash
cd example
make
make run

# Or use Xcode project
open example.xcodeproj
```

## API Reference

### ofxGlicCodec

Main codec class for encoding/decoding.

```cpp
class ofxGlicCodec {
    // Configuration
    void setConfig(const glic::CodecConfig& config);
    glic::CodecConfig& getConfig();

    // Encoding
    ofxGlicResult encode(const ofImage& source, const std::string& outputPath);
    std::vector<uint8_t> encodeToBuffer(const ofImage& source);

    // Decoding
    ofxGlicResult decode(const std::string& inputPath);
    ofxGlicResult decodeFromBuffer(const std::vector<uint8_t>& buffer);

    // Post-effects
    void addPostEffect(const ofxGlicEffect& effect);
    void clearPostEffects();
};
```

### ofxGlicEffects

Effects processor for post-processing.

```cpp
class ofxGlicEffects {
    void addPixelate(int blockSize = 8);
    void addScanline(int intensity = 50);
    void addChromatic(int offsetX = 2, int offsetY = 0);
    void addDither(int intensity = 50);
    void addPosterize(int levels = 4);
    void addGlitchShift(int blockSize = 8, uint32_t seed = 12345);

    void apply(ofImage& image);
    ofImage process(const ofImage& source);
    void clear();
};
```

### ofxGlicPresets

Preset manager with built-in configurations.

```cpp
class ofxGlicPresets {
    static ofxGlicPresets& instance();

    std::vector<std::string> getPresetNames() const;
    const ofxGlicPreset& getPreset(const std::string& name) const;

    // Built-in presets
    static ofxGlicPreset subtle();
    static ofxGlicPreset moderate();
    static ofxGlicPreset extreme();
    static ofxGlicPreset vhs();
    static ofxGlicPreset digital();
    static ofxGlicPreset corrupted();
    static ofxGlicPreset waves();
    static ofxGlicPreset blocky();
    static ofxGlicPreset dreamy();
    static ofxGlicPreset retro();
};
```

## Dependencies

- openFrameworks 0.11.0+
- C++17 compiler
- ofxGui (for examples with GUI)

## License

MIT License

## Credits

- [GLIC C++](https://github.com/daitomanabe/glic-cpp) - Core codec library
- Original GLIC concept by Daito Manabe
