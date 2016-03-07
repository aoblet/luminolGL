#pragma once

#include <string>

namespace Graphics{
    /**
     * UBO binding keys between host and device (CPU & GPU)
     */
    namespace UBO_keys{
        // Base
        const std::string COLOR_BUFFER     = "ColorBuffer";
        const std::string NORMAL_BUFFER    = "NormalBuffer";
        const std::string DEPTH_BUFFER     = "DepthBuffer";
        const std::string DIFFUSE          = "Diffuse";
        const std::string SPECULAR         = "Specular";
        const std::string NORMAL_MAP       = "NormalMap";
        const std::string CAMERA_POSITION  = "CamPos";
        const std::string NORMAL_MAP_ACTIVE = "IsNormalMapActive";

        const std::string MVP              = "MVP";
        const std::string MV               = "MV";
        const std::string MV_INVERSE       = "MVInverse";
        const std::string TIME             = "Time";
        const std::string SLIDER           = "Slider";
        const std::string SLIDER_MULT      = "SliderMult";
        const std::string SPECULAR_POWER   = "SpecularPower";
        const std::string INSTANCE_NUMBER  = "InstanceNumber";

        // Shadow
        const std::string SHADOW_MVP       = "ShadowMVP";
        const std::string SHADOW_POISSON_SAMPLE_COUNT  = "SampleCountPoisson";
        const std::string SHADOW_POISSON_SPREAD  = "SpreadPoisson";
        const std::string SHADOW_MV        = "ShadowMV";
        const std::string SHADOW_BIAS      = "ShadowBias";

        const std::string WORLD_TO_LIGHT_SCREEN = "WorldToLightScreen";
        const std::string SCREEN_TO_VIEW  = "ScreenToView";

        // Camera Motion Blur
        const std::string PREVIOUS_MVP  = "PreviousMVP";


        // FX
        const std::string FOCUS            = "Focus";
        const std::string SHADOW_BUFFER    = "ShadowBuffer";
        const std::string GAMMA            = "Gamma";
        const std::string SOBEL_INTENSITY  = "SobelIntensity";
        const std::string BLUR_SAMPLE_COUNT= "SampleCount";
        const std::string BLUR_DIRECTION   = "BlurDirection";

        const std::string DOF_COLOR        = "Color";
        const std::string DOF_COC          = "CoC";
        const std::string DOF_BLUR         = "Blur";

        const std::string MOTION_BLUR_COLOR        = "LastPass";
        const std::string MOTION_BLUR_DEPTH        = "Depth";
        const std::string MOTION_BLUR_SAMPLE_COUNT = "SampleCount";

        // DEBUG
        const std::string DEBUG_COLOR        = "debugColor";


        // UBO STRUCTS BINDING POINTS
        const std::string STRUCT_BINDING_POINT_CAMERA = "Camera";
        const std::string STRUCT_BINDING_POINT_LIGHT = "Light";

    }
}