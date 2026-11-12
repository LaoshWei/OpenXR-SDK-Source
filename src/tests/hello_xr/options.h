// Copyright (c) 2017-2025 The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "pch.h"

inline XrFormFactor GetXrFormFactor(const std::string& formFactorStr) {
    if (EqualsIgnoreCase(formFactorStr, "Hmd")) {
        return XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    }
    if (EqualsIgnoreCase(formFactorStr, "Handheld")) {
        return XR_FORM_FACTOR_HANDHELD_DISPLAY;
    }
    throw std::invalid_argument(Fmt("Unknown form factor '%s'", formFactorStr.c_str()));
}

inline XrViewConfigurationType GetXrViewConfigurationType(const std::string& viewConfigurationStr) {
    if (EqualsIgnoreCase(viewConfigurationStr, "Mono")) {
        return XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO;
    }
    if (EqualsIgnoreCase(viewConfigurationStr, "Stereo")) {
        return XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    }
    throw std::invalid_argument(Fmt("Unknown view configuration '%s'", viewConfigurationStr.c_str()));
}

inline XrEnvironmentBlendMode GetXrEnvironmentBlendMode(const std::string& environmentBlendModeStr) {
    if (EqualsIgnoreCase(environmentBlendModeStr, "Opaque")) {
        return XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    }
    if (EqualsIgnoreCase(environmentBlendModeStr, "Additive")) {
        return XR_ENVIRONMENT_BLEND_MODE_ADDITIVE;
    }
    if (EqualsIgnoreCase(environmentBlendModeStr, "AlphaBlend")) {
        return XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND;
    }
    throw std::invalid_argument(Fmt("Unknown environment blend mode '%s'", environmentBlendModeStr.c_str()));
}

inline const char* GetXrEnvironmentBlendModeStr(XrEnvironmentBlendMode environmentBlendMode) {
    switch (environmentBlendMode) {
        case XR_ENVIRONMENT_BLEND_MODE_OPAQUE:
            return "Opaque";
        case XR_ENVIRONMENT_BLEND_MODE_ADDITIVE:
            return "Additive";
        case XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND:
            return "AlphaBlend";
        default:
            throw std::invalid_argument(Fmt("Unknown environment blend mode '%s'", to_string(environmentBlendMode)));
    }
}

struct Options {
    std::string GraphicsPlugin;

#ifdef NATIVE_LF_XR_DIAGNOSTIC
    static constexpr bool NativeLfDiagnostic = true;
#else
    static constexpr bool NativeLfDiagnostic = false;
#endif

    std::string NativeLfAtlasFormat{"auto"};
    std::string NativeLfTest{"normal"};
    std::string NativeLfAlpha{"baseline"};
    std::string ColorSwapchainFormat{"auto"};

    std::string FormFactor{"Hmd"};

    std::string ViewConfiguration{"Stereo"};

    std::string EnvironmentBlendMode{"Opaque"};

    std::string AppSpace{"Local"};

    struct {
        XrFormFactor FormFactor{XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY};

        XrViewConfigurationType ViewConfigType{XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};

        XrEnvironmentBlendMode EnvironmentBlendMode{XR_ENVIRONMENT_BLEND_MODE_OPAQUE};
    } Parsed;

    void ParseStrings() {
        if (NativeLfDiagnostic && NativeLfTest != "normal" && NativeLfTest != "zero" && NativeLfTest != "clear" &&
            NativeLfTest != "invalid" && NativeLfTest != "rotation" && NativeLfTest != "identity-pattern" &&
            NativeLfTest != "srgb" && NativeLfTest != "mixed-srgb") {
            throw std::invalid_argument("Unknown Native LF diagnostic test");
        }
        if (!EqualsIgnoreCase(ColorSwapchainFormat, "auto") && !EqualsIgnoreCase(ColorSwapchainFormat, "rgba")) {
            throw std::invalid_argument("Color swapchain format must be auto or rgba");
        }
        if (NativeLfDiagnostic && NativeLfAlpha != "baseline" && NativeLfAlpha != "opaque" &&
            NativeLfAlpha != "unpremultiplied-only" && NativeLfAlpha != "premultiplied" && NativeLfAlpha != "straight") {
            throw std::invalid_argument("Unknown Native LF alpha mode");
        }
        Parsed.FormFactor = GetXrFormFactor(FormFactor);
        Parsed.ViewConfigType = GetXrViewConfigurationType(ViewConfiguration);
        Parsed.EnvironmentBlendMode = GetXrEnvironmentBlendMode(EnvironmentBlendMode);

        if (NativeLfDiagnostic && NativeLfTest == "srgb") { NativeLfAtlasFormat = "rgba-srgb"; }
        if (NativeLfDiagnostic && !EqualsIgnoreCase(NativeLfAtlasFormat, "auto") &&
            !EqualsIgnoreCase(NativeLfAtlasFormat, "bgra") &&
            !EqualsIgnoreCase(NativeLfAtlasFormat, "rgba") &&
            !EqualsIgnoreCase(NativeLfAtlasFormat, "rgba-srgb") &&
            !EqualsIgnoreCase(NativeLfAtlasFormat, "bgra-srgb")) {
            throw std::invalid_argument("Native LF atlas format must be auto, bgra, rgba, bgra-srgb or rgba-srgb");
        }
        if (NativeLfDiagnostic && !EqualsIgnoreCase(GraphicsPlugin, "Vulkan") &&
            !EqualsIgnoreCase(GraphicsPlugin, "Vulkan2")) {
            throw std::invalid_argument("Native LF diagnostic requires the Vulkan graphics plugin");
        }
        if (NativeLfDiagnostic && Parsed.ViewConfigType != XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO) {
            throw std::invalid_argument("Native LF diagnostic requires PRIMARY_STEREO");
        }
        if (NativeLfDiagnostic && Parsed.EnvironmentBlendMode != XR_ENVIRONMENT_BLEND_MODE_OPAQUE) {
            throw std::invalid_argument("Native LF diagnostic requires opaque environment blend mode");
        }
    }

    std::array<float, 4> GetBackgroundClearColor() const {
        static const std::array<float, 4> SlateGrey{0.184313729f, 0.309803933f, 0.309803933f, 1.0f};
        static const std::array<float, 4> TransparentBlack{0.0f, 0.0f, 0.0f, 0.0f};
        static const std::array<float, 4> Black{0.0f, 0.0f, 0.0f, 1.0f};

        switch (Parsed.EnvironmentBlendMode) {
            case XR_ENVIRONMENT_BLEND_MODE_OPAQUE:
                return SlateGrey;
            case XR_ENVIRONMENT_BLEND_MODE_ADDITIVE:
                return Black;
            case XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND:
                return TransparentBlack;
            default:
                return SlateGrey;
        }
    }

    void SetEnvironmentBlendMode(XrEnvironmentBlendMode environmentBlendMode) {
        EnvironmentBlendMode = GetXrEnvironmentBlendModeStr(environmentBlendMode);
        Parsed.EnvironmentBlendMode = environmentBlendMode;
    }
};
