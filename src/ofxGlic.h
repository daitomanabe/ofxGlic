#pragma once

#include "ofMain.h"
#include "ofxGlicConfig.h"
#include "glic/glic.hpp"
#include "glic/colorspaces.hpp"
#include "glic/effects.hpp"
#include "ofxGlicCodec.h"
#include "ofxGlicEffects.h"
#include "ofxGlicPresets.h"

// Main include file for ofxGlic addon
// Include this file to use all ofxGlic functionality

// Convenience typedefs for oF-style naming
using ofxGlicColorSpace = glic::ColorSpace;
using ofxGlicPrediction = glic::PredictionMethod;
using ofxGlicWavelet = glic::WaveletType;
using ofxGlicEncoding = glic::EncodingMethod;
using ofxGlicClamp = glic::ClampMethod;
using ofxGlicTransform = glic::TransformType;

// Helper functions for ofImage conversion
namespace ofxGlic {
    // Convert ofImage to GLIC color array
    inline std::vector<glic::Color> toGlicColors(const ofImage& img) {
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

    // Convert GLIC color array to ofImage
    inline void toOfImage(const std::vector<glic::Color>& colors, int width, int height, ofImage& img) {
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

    // Get color space name
    inline std::string getColorSpaceName(ofxGlicColorSpace cs) {
        return glic::colorSpaceName(cs);
    }

    // Get prediction method name
    inline std::string getPredictionName(ofxGlicPrediction pm) {
        return glic::predictionName(pm);
    }

    // Get wavelet name
    inline std::string getWaveletName(ofxGlicWavelet wt) {
        return glic::waveletName(wt);
    }

    // Get encoding method name
    inline std::string getEncodingName(ofxGlicEncoding em) {
        return glic::encodingName(em);
    }

    // Get all color space names
    inline std::vector<std::string> getColorSpaceNames() {
        return {"OHTA", "RGB", "CMY", "HSB", "XYZ", "YXY", "HCL", "LUV",
                "LAB", "HWB", "R-GGB-G", "YPbPr", "YCbCr", "YDbDr", "GS", "YUV"};
    }

    // Get all prediction method names
    inline std::vector<std::string> getPredictionNames() {
        return {"NONE", "CORNER", "H", "V", "DC", "DCMEDIAN", "MEDIAN", "AVG",
                "TRUEMOTION", "PAETH", "LDIAG", "HV", "JPEGLS", "DIFF", "REF", "ANGLE",
                "SAD", "BSAD", "RANDOM"};
    }

    // Get all wavelet names
    inline std::vector<std::string> getWaveletNames() {
        return {"NONE", "HAAR_ORTHOGONAL", "BIORTHOGONAL11", "BIORTHOGONAL13", "BIORTHOGONAL15",
                "BIORTHOGONAL22", "BIORTHOGONAL24", "BIORTHOGONAL26", "BIORTHOGONAL28",
                "BIORTHOGONAL31", "BIORTHOGONAL33", "BIORTHOGONAL35", "BIORTHOGONAL37", "BIORTHOGONAL39",
                "BIORTHOGONAL44", "BIORTHOGONAL55", "BIORTHOGONAL68",
                "COIFLET1", "COIFLET2", "COIFLET3", "COIFLET4", "COIFLET5",
                "SYMLET2", "SYMLET3", "SYMLET4", "SYMLET5", "SYMLET6", "SYMLET7", "SYMLET8", "SYMLET9", "SYMLET10",
                "DAUBECHIES2", "DAUBECHIES3", "DAUBECHIES4", "DAUBECHIES5", "DAUBECHIES6",
                "DAUBECHIES7", "DAUBECHIES8", "DAUBECHIES9", "DAUBECHIES10", "HAAR"};
    }

    // Get all encoding method names
    inline std::vector<std::string> getEncodingNames() {
        return {"RAW", "PACKED", "RLE"};
    }
}
