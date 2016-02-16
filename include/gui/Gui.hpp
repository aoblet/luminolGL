#pragma once
 
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "stb/stb_image.h"
#include "imgui/imgui.h" 
#include "imgui/imguiRenderGL3.h"
#include "lights/Light.hpp"
#include "geometry/Spline3D.h"

namespace Gui{

    /**
    * Gui class: Handler for imgui.
    */

    class Gui {

    private:

    	char lineBuffer[512];
        float xwidth = 400;
        float ywidth = 550;
        int width;
        int height;
        int DPI;
        double mousex, mousey;
        unsigned char mbut = '0';
        int mscroll = 0;
        int logScroll = 0;
	

	public:

		Gui(int DPI, int width, int height);
    	Gui(int DPI, int width, int height, float xwidth, float ywidth, 
    		unsigned char mbut, int mscrool, int logScrool);
    
    	void beginFrame();
    	void getCursorPos(GLFWwindow * window);
    	void updateFrame();
    	void updateMbut(bool leftButtonPress);
    	void scrollArea(std::map<std::string, float*> & imguiParams, Light::LightHandler & lightHandler, Geometry::Spline3D & spline);
    	void scrollAreaEnd();

    };
}


