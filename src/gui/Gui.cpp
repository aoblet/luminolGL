
#include "gui/Gui.hpp"

  
namespace Gui{
	
	Gui::Gui(int _DPI, int _width, int _height): 
		DPI(_DPI), width(_width), height(_height)
	{

	}

	Gui::Gui(int DPI, int width, int height, float xwidth, float ywidth, 
    		unsigned char mbut, int mscrool, int logScrool){

		this->DPI = DPI;
		this->width = width;
		this->height = height;
	}

	void Gui::beginFrame(){

		imguiBeginFrame(mousex, mousey, mbut, mscroll);

	}

	void Gui::getCursorPos(GLFWwindow * window){

		glfwGetCursorPos(window, &mousex, &mousey);
	}

	void Gui::updateFrame(){
		mousex*=DPI;
        mousey*=DPI;
        mousey = height - mousey;
	}

	void Gui::updateMbut(bool leftButtonPress){
		mscroll = 0;
        if(leftButtonPress) 
        	mbut |= IMGUI_MBUT_LEFT;
        else mbut = '0';
	}

	void Gui::scrollArea(std::map<std::string, float*> & imguiParams, Light::LightHandler & lightHandler, Geometry::Spline3D & spline){

		imguiBeginScrollArea("aogl", width - xwidth - 10, height - ywidth - 10, xwidth, ywidth, &logScroll);
		
		sprintf(lineBuffer, "FPS %f", imguiParams["FPS"] );
		imguiLabel(lineBuffer);
		imguiSlider("Slider", imguiParams["Slider"], 0.0, 1.0, 0.001);
		imguiSlider("InstanceNumber", imguiParams["InstanceNumber"], 100, 100000, 1);
		imguiSlider("SliderMultiply", imguiParams["SliderMultiply"], 0.0, 1000.0, 0.1);

        imguiSlider("Shadow Bias", imguiParams["Shadow Bias"], 0, 0.001, 0.00000001);
        imguiSlider("Gamma", imguiParams["Gamma"], 1, 8, 0.01);
        imguiSlider("Sobel Intensity", imguiParams["SobelIntensity"], 0, 4, 0.01);
        imguiSlider("Blur Sample Count", imguiParams["BlurSampleCount"], 0, 32, 1);
        imguiSlider("Focus Near", imguiParams["FocusNear"], 0, 10, 0.01);
        imguiSlider("Focus Position", imguiParams["FocusPosition"], 0, 100, 0.01);
        imguiSlider("Focus Far", imguiParams["FocusFar"], 0, 100, 0.01);


		imguiSlider("Specular Power", &lightHandler._specularPower, 0, 100, 0.1);
        imguiSlider("Attenuation", &lightHandler._lightAttenuation, 0, 16, 0.1);
        imguiSlider("Intensity", &lightHandler._lightIntensity, 0, 10, 0.1);
        imguiSlider("Threshold", &lightHandler._lightAttenuationThreshold, 0, 0.5, 0.0001);
        for(size_t i = 0; i < lightHandler._spotLights.size(); ++i){
            imguiSlider("pos.x", &lightHandler._spotLights[i]._pos.x, -50, 50, 0.001);
            imguiSlider("pos.y", &lightHandler._spotLights[i]._pos.y, -50, 50, 0.001);
            imguiSlider("pos.z", &lightHandler._spotLights[i]._pos.z, -50, 50, 0.001);

            imguiSlider("dir.x", &lightHandler._spotLights[i]._dir.x, -1, 1, 0.001);
            imguiSlider("dir.y", &lightHandler._spotLights[i]._dir.y, -1, 1, 0.001);
            imguiSlider("dir.z", &lightHandler._spotLights[i]._dir.z, -1, 1, 0.001);

            imguiSlider("angle", &lightHandler._spotLights[i]._angle, 0, 180, 0.001);
            imguiSlider("falloff", &lightHandler._spotLights[i]._falloff, 0, 180, 0.001);
            imguiSlider("intensity", &lightHandler._spotLights[i]._intensity, 0, 10, 0.001);
            imguiSlider("attenuation", &lightHandler._spotLights[i]._attenuation, 0, 10, 0.001);
        }



        imguiLabel("Spline Handler");
        for(size_t i = 0; i < spline.size(); ++i){
            imguiLabel("point");
            imguiSlider("x", &spline[i].x, -100, 100, 0.1);
            imguiSlider("y", &spline[i].y, -100, 100, 0.1);
            imguiSlider("z", &spline[i].z, -100, 100, 0.1);
        }

        if(imguiButton("Add Spline")){
            spline.add(spline[spline.size()-1]);
        }

        // for (auto& x: imguiParams) {
		// 	std::cout << x.first << ": " << x.second << '\n';
		// }
	}

	void Gui::scrollAreaEnd(){
		imguiEndScrollArea();
        imguiEndFrame();
        imguiRenderGLDraw(width, height);
	}

}