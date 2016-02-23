
#include "gui/Gui.hpp"

  
namespace Gui{
	
	Gui::Gui(int _DPI, int _width, int _height, const char * _name): 
		DPI(_DPI), width(_width), height(_height), name(_name)
	{

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

	void Gui::addSlider(const char * name, float * var, float begin, float end, float step){
		imguiSlider(name, var, begin, end, step);
	}

    void Gui::addSlider(Param & p){
        imguiSlider(p._name, p._var, p._begin, p._end, p._step);
    }

	void Gui::addSlider(std::vector<Param> & params){
        for(size_t i=0; i<params.size(); ++i){
            addSlider(params[i]);
        }
	}

    void Gui::addLabel(const char * label){
        imguiLabel(label);
    }

    void Gui::addLabel(const char * label, float * var){
        sprintf(lineBuffer, "%s %f", label, var );
        imguiLabel(lineBuffer);
    }

    void Gui::addLabel(const char * label, int var){
        sprintf(lineBuffer, "%s %d", label, var );
        imguiLabel(lineBuffer);
    }

    void Gui::setScrollArea(){
        imguiBeginScrollArea(name, width - xwidth - 10, height - ywidth - 10, xwidth, ywidth, &logScroll);
    }

    bool Gui::addButton(const char * name){
        return(imguiButton(name));
    }

    void Gui::addSeparator(){
        imguiSeparatorLine();
    }

    void Gui::addSliderDirectionalLights(Light::LightHandler & lightHandler, float posBegin, float posEnd, float posStep, float attBegin, float attEnd, float attStep){

        addLabel("Directional Light: 0");
 
        imguiSlider("DL: direction.x", &lightHandler._directionnalLights[0]._pos.x, posBegin, posEnd, posStep);
        imguiSlider("DL: direction.y", &lightHandler._directionnalLights[0]._pos.y, posBegin, posEnd, posStep);
        imguiSlider("DL: direction.z", &lightHandler._directionnalLights[0]._pos.z, posBegin, posEnd, posStep);
        imguiSlider("DL: color.R", &lightHandler._directionnalLights[0]._color.r, 0, 1, 0.01);
        imguiSlider("DL: color.G", &lightHandler._directionnalLights[0]._color.g, 0, 1, 0.01);
        imguiSlider("DL: color.B", &lightHandler._directionnalLights[0]._color.b, 0, 1, 0.01);
        imguiSlider("DL: intensity", &lightHandler._directionnalLights[0]._intensity, attBegin, attEnd, attStep);
        imguiSlider("DL: attenuation", &lightHandler._directionnalLights[0]._attenuation, attBegin, attEnd, attStep);
        
        addSeparator();
    }

    void Gui::addSliderSpotLights(Light::LightHandler & lightHandler, float posBegin, float posEnd, float posStep, float dirBegin, float dirEnd, float dirStep, float attBegin, float attEnd, float attStep){

        for(size_t i = 0; i < lightHandler._spotLights.size(); ++i){
            addLabel("Spot Light: ", (int)i);

            imguiSlider("SL: position.x", &lightHandler._spotLights[i]._pos.x, posBegin, posEnd, posStep);
            imguiSlider("SL: position.y", &lightHandler._spotLights[i]._pos.y, posBegin, posEnd, posStep);
            imguiSlider("SL: position.z", &lightHandler._spotLights[i]._pos.z, posBegin, posEnd, posStep);

            imguiSlider("SL: direction.x", &lightHandler._spotLights[i]._dir.x, -1, 1, 0.001);
            imguiSlider("SL: direction.y", &lightHandler._spotLights[i]._dir.y, -1, 1, 0.001);
            imguiSlider("SL: direction.z", &lightHandler._spotLights[i]._dir.z, -1, 1, 0.001);

            imguiSlider("SL: angle", &lightHandler._spotLights[i]._angle, 0, 180, 0.001);
            imguiSlider("SL: falloff", &lightHandler._spotLights[i]._falloff, 0, 180, 0.001);
            imguiSlider("SL: intensity", &lightHandler._spotLights[i]._intensity, 0, 1, 0.001);
            imguiSlider("SL: attenuation", &lightHandler._spotLights[i]._attenuation, attBegin, attEnd, attStep);
        }
        addSeparator();
    }

    void Gui::addSliderPointLights(Light::LightHandler & lightHandler, float posBegin, float posEnd, float posStep, float attBegin, float attEnd, float attStep){

        for(size_t i = 0; i < lightHandler._spotLights.size(); ++i){
            addLabel("Point Light: ", (int)i);

            imguiSlider("PL: position.x", &lightHandler._pointLights[i]._pos.x, posBegin, posEnd, posStep);
            imguiSlider("PL: position.y", &lightHandler._pointLights[i]._pos.y, -3.0, 12.0, posStep);
            imguiSlider("PL: position.z", &lightHandler._pointLights[i]._pos.z, posBegin, posEnd, posStep);
            imguiSlider("PL: color.R", &lightHandler._pointLights[i]._color.r, 0, 1, 0.01);
            imguiSlider("PL: color.G", &lightHandler._pointLights[i]._color.g, 0, 1, 0.01);
            imguiSlider("PL: color.B", &lightHandler._pointLights[i]._color.b, 0, 1, 0.01);
            imguiSlider("PL: intensity", &lightHandler._pointLights[i]._intensity, 0, 1, 0.001);
            imguiSlider("PL: attenuation", &lightHandler._pointLights[i]._attenuation, attBegin, attEnd, attStep);
        
        }
        addSeparator();
    }

    void Gui::addSliderSpline(Geometry::Spline3D & spline, float posBegin, float posEnd, float posStep){

        addLabel("Spline Handler");
        addSeparator();
        for(size_t i = 0; i < spline.size(); ++i){
            addLabel("point");
            imguiSlider("x", &spline[i].x, posBegin, posEnd, posStep);
            imguiSlider("y", &spline[i].y, posBegin, posEnd, posStep);
            imguiSlider("z", &spline[i].z, posBegin, posEnd, posStep);
        }
    }

	void Gui::scrollAreaEnd(){
		imguiEndScrollArea();
        imguiEndFrame();
        imguiRenderGLDraw(width, height);
	}

}