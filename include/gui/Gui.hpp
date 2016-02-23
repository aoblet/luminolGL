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

        double mousex, mousey;
        const char * name;
        int width;
        int height;
        int DPI;
        unsigned char mbut = '0';
        int mscroll = 0;
        int logScroll = 0;
    	char lineBuffer[512];
        float xwidth = 400;
        float ywidth = 550;
	

	public:

		Gui(int DPI, int width, int height, const char * name);

    	void beginFrame();
    	void getCursorPos(GLFWwindow * window);
    	void updateFrame();
    	void updateMbut(bool leftButtonPress);
    	void setScrollArea();

		void addSlider(const char * name, float * var, float begin, float end, float step);
		void addSlider(Param & p);
		void addSlider(std::vector<Param> & params);

        void addLabel(const char * label);
    	void addLabel(const char * label, float * var);
		void addLabel(const char * label, int var);

		bool addButton(const char * name);

		void addSeparator();

    	void addSliderDirectionalLights(Light::LightHandler & lightHandler, float posBegin=-300, float posEnd=300, float posStep=0.001, float attBegin=0.01, float attEnd=4.0, float attStep=0.001);
    	void addSliderSpotLights(Light::LightHandler & lightHandler, float posBegin=-300, float posEnd=300, float posStep=0.001, float dirBegin=-1, float dirEnd=1, float dirStep=0.001, float attBegin=0.01, float attEnd=4.0, float attStep=0.001);
    	void addSliderPointLights(Light::LightHandler & lightHandler, float posBegin=-300, float posEnd=300, float posStep=0.001, float attBegin=0.01, float attEnd=4.0, float attStep=0.001);

		void addSliderSpline(Geometry::Spline3D & spline, float posBegin=-300, float posEnd=300, float posStep=0.001);


    	void scrollAreaEnd();

    };
}


