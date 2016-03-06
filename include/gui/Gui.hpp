#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "stb/stb_image.h"
#include "imgui/imgui.h"
#include "imgui/imguiRenderGL3.h"
#include "lights/Light.hpp"
#include "geometry/Spline3D.h"
#include "view/CameraController.hpp"
#include "gui/UserInput.hpp"
#include "graphics/ShaderProgram.hpp"

namespace Gui{

	/**
    * Gui class: Handler for imgui.
    * If you want to add a parameter that is not in the LightHandler or in the Splide3D Handler you need to
    * add the float variable in the map<std::string, float*> & imguiParams in the main loop of the main.
    * Look for imguiParams
    * This class allow us to draw a block, get the cursor position and any associated event.
    * According to the event the class will update the frame and the variables.
    */

	struct Param{
		const char * _name;
		float * _var;
		float _begin;
		float _end;
		float _step;

		Param(const char * name, float * var, float begin, float end, float step):
				_name(name), _var(var), _begin(begin), _end(end), _step(step)
		{

		}
	};

	class Gui {

	private:
		int _dpi;
		int _screenWidth;
		int _screenHeight;

		int _windowWidth = 400;
		int _windowHeight = 550;

		const char *_name;

		double _mouseX, _mouseY;

		unsigned char _mbut = '0';
		int _mscroll = 0;
		int _logScroll = 0;
		char _lineBuffer[512];

	public:

		Gui(int DPI, int screenWidth, int screenHeight, int windowWidth = 100, int windowHeight = 100, const char * name = "");

		void beginFrame();
		void getCursorPos(GLFWwindow * window);
		void updateFrame();
		void updateMbut(bool leftButtonPress);
		void beginScrollArea();

		void init(GLFWwindow * window);;

		void addSlider(const char * name, float * var, float begin, float end, float step);
		void addSlider(Param & p);
		void addSlider(std::vector<Param> & params);
		void addSliderInt(const char* text, int* val, float vmin, float vmax, float vinc, bool enabled = true);

		void addLabel(const char * label);
		void addLabel(const char * label, float * var);
		void addLabel(const char * label, int var);

		bool addButton(const char * name);
		bool addButton(const char * name, bool & display);

		void addSliderDirectionalLights(Light::LightHandler & lightHandler, float posBegin=-300, float posEnd=300, float posStep=0.001, float attBegin=0.01, float attEnd=4.0, float attStep=0.001);
		void addSliderSpotLights(Light::LightHandler & lightHandler, float posBegin=-300, float posEnd=300, float posStep=0.001, float dirBegin=-1, float dirEnd=1, float dirStep=0.001, float attBegin=0.01, float attEnd=4.0, float attStep=0.001);
		void addSliderPointLights(Light::LightHandler & lightHandler, float posBegin=-300, float posEnd=300, float posStep=0.001, float attBegin=0.01, float attEnd=4.0, float attStep=0.001);

		void addSliderSpline(Geometry::Spline3D & spline, float posBegin=-300, float posEnd=300, float posStep=0.001);

		bool addCheckBox(const char* text, bool checked, bool enabled);
		bool addItem(const char* text, bool enabled);
		bool addCollapse(const char* text, const char* subtext, bool checked, bool enabled);
		void addValue(const char* text);

		void addIndent();
		void addUnindent();
		void addSeparator();
		void addSeparatorLine();

		void scrollAreaEnd();

		int getWindowWidth();
		int getWindowHeight();

		void setWindowWidth(int width);
		void setWindowHeight(int height);

		glm::vec2 getMousePosition();

		bool displayGeneralParameters = false;
		bool displayGeneralLightParameters = false;
		bool displayPointLightParameters = false;
		bool displaySpotLightParameters = false;
		bool displayDirectionalLightParameters = false;
		bool displayCameraSplineParameters = false;
		bool displayPostFxParameters = false;
		bool displayMenu = true;
	};
}


