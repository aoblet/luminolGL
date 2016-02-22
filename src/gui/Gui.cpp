
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




	void Gui::scrollArea(
		std::map<std::string, 
		float*> & imguiParams, 
		Light::LightHandler & lightHandler, 
		Geometry::Spline3D & spline,
		View::CameraController & cameraController,
		GUI::UserInput & userInput,
		std::map<std::string, Graphics::ShaderProgram*> & imguiShaders,
    	std::map<std::string,std::string> & imguiUniforms
	)
	{

		int isNormalMapActive = 1;
		

		imguiBeginScrollArea("aogl", width - xwidth - 10, height - ywidth - 10, xwidth, ywidth, &logScroll);
		
		sprintf(lineBuffer, "FPS %f", imguiParams["FPS"] );
		imguiLabel(lineBuffer);
		imguiSlider("Slider", imguiParams["Slider"], 0.0, 1.0, 0.001);
		imguiSlider("InstanceNumber", imguiParams["InstanceNumber"], 100, 100000, 1);
		imguiSlider("SliderMultiply", imguiParams["SliderMultiply"], 0.0, 1000.0, 0.1);

        if(imguiButton("Camera Spectator"))
            cameraController.setSpectator(!cameraController.isSpectator());
        imguiSlider("Camera splines velocity", &(cameraController.velocitySplines()), 0.0, 1.0, 0.001);
        imguiSlider("Camera angles velocity", &userInput.getVelocityRotate(), 0.0, 0.2, 0.001);

        imguiSeparatorLine();
        if(imguiButton("IsNormalMapActive"))
            // imguiShaders["mainShader"]->updateUniform(imguiUniforms["UNIFORM_NAME_NORMAL_MAP_ACTIVE"], (imguiParams["isNormalMapActive"] = imguiParams["isNormalMapActive"] ? 0.f : 1.f));
            imguiShaders["mainShader"]->updateUniform(imguiUniforms["UNIFORM_NAME_NORMAL_MAP_ACTIVE"], (isNormalMapActive = isNormalMapActive ? 0 : 1));

		imguiLabel("Post-FX parameters");
        imguiSlider("Shadow Bias", imguiParams["Shadow Bias"], 0, 0.001, 0.00000001);
        imguiSlider("Gamma", imguiParams["Gamma"], 1, 8, 0.01);
        imguiSlider("Sobel Intensity", imguiParams["SobelIntensity"], 0, 4, 0.01);
        imguiSlider("Blur Sample Count", imguiParams["BlurSampleCount"], 0, 32, 1);
        imguiSlider("Focus Near", imguiParams["FocusNear"], 0, 10, 0.01);
        imguiSlider("Focus Position", imguiParams["FocusPosition"], 0, 100, 0.01);
        imguiSlider("Focus Far", imguiParams["FocusFar"], 0, 100, 0.01);

		sprintf(lineBuffer, "General Lights Parameters");
		imguiLabel(lineBuffer);
		imguiSlider("Specular Power", &lightHandler._specularPower, 0, 100, 0.1);
		imguiSlider("Attenuation", &lightHandler._lightAttenuation, 0, 16, 0.1);
        imguiSlider("Intensity", &lightHandler._lightIntensity, 0, 10, 0.1);
        imguiSlider("Threshold", &lightHandler._lightAttenuationThreshold, 0, 0.5, 0.0001);

		sprintf(lineBuffer, "Directional Light [0]");
		imguiLabel(lineBuffer); 
		imguiSlider("DL: dir.x", &lightHandler._directionnalLights[0]._pos.x, -150, 300, 0.001);
        imguiSlider("DL: dir.y", &lightHandler._directionnalLights[0]._pos.y, -150, 300, 0.001);
        imguiSlider("DL: dir.z", &lightHandler._directionnalLights[0]._pos.z, -150, 300, 0.001);
        imguiSlider("DL: col.R", &lightHandler._directionnalLights[0]._color.r, 0, 1, 0.01);
        imguiSlider("DL: col.G", &lightHandler._directionnalLights[0]._color.g, 0, 1, 0.01);
        imguiSlider("DL: col.B", &lightHandler._directionnalLights[0]._color.b, 0, 1, 0.01);
        imguiSlider("DL: intensity", &lightHandler._directionnalLights[0]._intensity, 0, 1, 0.001);
        imguiSlider("DL: attenuation", &lightHandler._directionnalLights[0]._attenuation, 0.01, 3, 0.001);

		sprintf(lineBuffer, "Point Light [0]");
		imguiLabel(lineBuffer);
		imguiSlider("PL: pos.x", &lightHandler._pointLights[0]._pos.x, -150, 300, 0.00001);
        imguiSlider("PL: pos.y", &lightHandler._pointLights[0]._pos.y, -5, 10, 0.00001);
        imguiSlider("PL: pos.z", &lightHandler._pointLights[0]._pos.z, -150, 300, 0.00001);
        imguiSlider("PL: col.R", &lightHandler._pointLights[0]._color.r, 0, 1, 0.01);
        imguiSlider("PL: col.G", &lightHandler._pointLights[0]._color.g, 0, 1, 0.01);
        imguiSlider("PL: col.B", &lightHandler._pointLights[0]._color.b, 0, 1, 0.01);
        imguiSlider("PL: intensity", &lightHandler._pointLights[0]._intensity, 0, 1, 0.001);
        imguiSlider("PL: attenuation", &lightHandler._pointLights[0]._attenuation, 0.01, 3, 0.001);
        
        for(size_t i = 0; i < lightHandler._spotLights.size(); ++i){
        	sprintf(lineBuffer, "Spot Light  %d", i );
        	imguiLabel(lineBuffer);
            imguiSlider("pos.x", &lightHandler._spotLights[i]._pos.x, -150, 300, 0.001);
            imguiSlider("pos.y", &lightHandler._spotLights[i]._pos.y, -150, 300, 0.001);
            imguiSlider("pos.z", &lightHandler._spotLights[i]._pos.z, -150, 300, 0.001);

            imguiSlider("dir.x", &lightHandler._spotLights[i]._dir.x, -1, 1, 0.001);
            imguiSlider("dir.y", &lightHandler._spotLights[i]._dir.y, -1, 1, 0.001);
            imguiSlider("dir.z", &lightHandler._spotLights[i]._dir.z, -1, 1, 0.001);

            imguiSlider("angle", &lightHandler._spotLights[i]._angle, 0, 180, 0.001);
            imguiSlider("falloff", &lightHandler._spotLights[i]._falloff, 0, 180, 0.001);
            imguiSlider("intensity", &lightHandler._spotLights[i]._intensity, 0, 1, 0.001);
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