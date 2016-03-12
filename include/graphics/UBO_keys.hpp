#pragma once

#include <string>

namespace Graphics{
    /**
     * UBO binding keys between host and device (CPU & GPU)
     */
    namespace UBO_keys{
        // Base
        const std::string COLOR_BUFFER                  = "ColorBuffer";
        const std::string NORMAL_BUFFER                 = "NormalBuffer";
        const std::string DEPTH_BUFFER                  = "DepthBuffer";
        const std::string DIFFUSE                       = "Diffuse";
        const std::string SPECULAR                      = "Specular";
        const std::string NORMAL_MAP                    = "NormalMap";
        const std::string CAMERA_POSITION               = "CamPos";
        const std::string NORMAL_MAP_ACTIVE             = "IsNormalMapActive";

        const std::string MVP                           = "MVP";
        const std::string MV                            = "MV";
        const std::string MV_NORMAL                     = "MVNormal";
        const std::string MV_INVERSE                    = "MVInverse";
        const std::string TIME                          = "Time";
        const std::string SPECULAR_POWER                = "SpecularPower";

        // Shadow
        const std::string SHADOW_MVP                    = "ShadowMVP";
        const std::string SHADOW_MV                     = "ShadowMV";
        const std::string SHADOW_BIAS                   = "ShadowBias";
        const std::string SHADOW_POISSON_SAMPLE_COUNT   = "SampleCountPoisson";
        const std::string SHADOW_POISSON_SPREAD         = "SpreadPoisson";
        const std::string WORLD_TO_LIGHT_SCREEN         = "WorldToLightScreen";
        const std::string SCREEN_TO_VIEW                = "ScreenToView";

        // Ambient
        const std::string AMBIENT_INTENSITY  = "Ambient";
        const std::string BEAUTY_BUFFER  = "Beauty";
        const std::string SSAO_BUFFER  = "SSAO";

        // Camera Motion Blur
        const std::string PREVIOUS_MVP                  = "PreviousMVP";


        // FX
        const std::string FOCUS                         = "Focus";
        const std::string SHADOW_BUFFER                 = "ShadowBuffer";
        const std::string GAMMA                         = "Gamma";
        const std::string SOBEL_INTENSITY               = "SobelIntensity";
        const std::string BLUR_SAMPLE_COUNT             = "SampleCount";
        const std::string BLUR_DIRECTION                = "BlurDirection";

        const std::string DOF_COLOR                     = "Color";
        const std::string DOF_COC                       = "CoC";
        const std::string DOF_BLUR                      = "Blur";

        const std::string MOTION_BLUR_COLOR             = "LastPass";
        const std::string MOTION_BLUR_DEPTH             = "Depth";
        const std::string MOTION_BLUR_SAMPLE_COUNT      = "SampleCount";

        const std::string SSAO_POSITION_DEPTH = "PositionDepth";
        const std::string SSAO_DEPTH = "Depth";
        const std::string SSAO_NORMAL = "Normal";
        const std::string SSAO_NOISE = "Noise";
        const std::string SSAO_SCREEN_DIM = "ScreenDim";
        const std::string SSAO_SAMPLES = "Samples";
        const std::string SSAO_PROJECTION = "Projection";
        const std::string SSAO_OCCLUSION_INTENSITY = "OcclusionIntensity";
        const std::string SSAO_OCCLUSION_RADIUS = "OcclusionRadius";

        // DEBUG
        const std::string DEBUG_COLOR                   = "debugColor";


        // UBO STRUCTS BINDING POINTS
        const std::string STRUCT_BINDING_POINT_CAMERA   = "Camera";
        const std::string STRUCT_BINDING_POINT_LIGHT    = "Light";

        // Skybox
        const std::string SKYBOX_CUBE_MAP               = "Skybox";
        const std::string SKYBOX_BEAUTY                 = "Beauty";
        const std::string SKYBOX_DEPTH_BUFFER           = "Depth";
        const std::string SKYBOX_SCREEN_TO_WORLD_NO_TRANSLATE = "ScreenToWorldNoTranslate";

        // Water
        const std::string WATER_Y_POS                   = "WaterYPos";
        const std::string WATER_NOISE_AMPLITUDE         = "NoiseAmplitude";
        const std::string WATER_SPECULAR_AMPLITUDE      = "SpecularAmplitude";
        const std::string WATER_IS_REFLECTION           = "IsReflection";
        const std::string WATER_LIGHT_RAY               = "LightRay";
        const std::string WATER_REFRACTION_TEXTURE      = "RefractionTexture";
        const std::string WATER_FRESNEL_AMPLITUDE       = "FresnelAmplitude";
        const std::string WATER_FRESNEL_BIAS            = "FresnelBias";


    }
}