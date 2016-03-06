
#include "gui/Gui.hpp"


namespace Gui{

    Gui::Gui(int DPI, int screenWidth, int screenHeight, int windowWidth, int windowHeight, const char * name):
            _dpi(DPI),
            _screenWidth(screenWidth),
            _screenHeight(screenHeight),
            _windowWidth(windowWidth),
            _windowHeight(windowHeight),
            _name(name)
    { }


    void Gui::beginFrame(){
        imguiBeginFrame(_mouseX, _mouseY, _mbut, _mscroll);
    }

    void Gui::getCursorPos(GLFWwindow * window){
        glfwGetCursorPos(window, &_mouseX, &_mouseY);
    }

    void Gui::updateFrame(){
        _mouseX *= _dpi;
        _mouseY *= _dpi;
        _mouseY = _screenHeight - _mouseY;
    }

    void Gui::updateMbut(bool leftButtonPress){
        _mscroll = 0;
        if(leftButtonPress)
            _mbut |= IMGUI_MBUT_LEFT;
        else _mbut = '0';
    }

    void Gui::init(GLFWwindow * window){
        beginFrame();
        getCursorPos(window);
        updateFrame();
        beginScrollArea();
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

    void Gui::addSliderInt(const char* text, int* val, float vmin, float vmax, float vinc, bool enabled){
        imguiSliderInt(text, val, vmin, vmax, vinc, enabled);
    }

    void Gui::addLabel(const char * label){
        imguiLabel(label);
    }

    void Gui::addLabel(const char * label, float * var){
        sprintf(_lineBuffer, "%s %f", label, *var );
        imguiLabel(_lineBuffer);
    }

    void Gui::addLabel(const char * label, int var){
        sprintf(_lineBuffer, "%s %d", label, var );
        imguiLabel(_lineBuffer);
    }


    void Gui::beginScrollArea(){
        imguiBeginScrollArea(_name, _screenWidth - _windowWidth - 10, _screenHeight - _windowHeight - 10, _windowWidth,
                             _windowHeight, &_logScroll);
    }

    bool Gui::addButton(const char * name){
        return(imguiButton(name));
    }

    bool Gui::addButton(const char * name, bool & display){
        if(imguiButton(name))
            display = !display;
        return display;
    }

    void Gui::addSliderDirectionalLights(Light::LightHandler & lightHandler, float posBegin, float posEnd, float posStep, float attBegin, float attEnd, float attStep){
        addIndent();
        addLabel("Directional Light: 0");
        addIndent();
        imguiSlider("DL: direction.x", &lightHandler._directionalLight._pos.x, posBegin, posEnd, posStep);
        imguiSlider("DL: direction.y", &lightHandler._directionalLight._pos.y, posBegin, posEnd, posStep);
        imguiSlider("DL: direction.z", &lightHandler._directionalLight._pos.z, posBegin, posEnd, posStep);
        imgui3Slider("Red", &lightHandler._directionalLight._color.r, 0, 1, 0.01, 1);
        imgui3Slider("Green", &lightHandler._directionalLight._color.g, 0, 1, 0.01, 2);
        imgui3Slider("Blue", &lightHandler._directionalLight._color.b, 0, 1, 0.01, 3);
        imguiSlider("DL: intensity", &lightHandler._directionalLight._intensity, attBegin, attEnd, attStep);
        imguiSlider("DL: attenuation", &lightHandler._directionalLight._attenuation, attBegin, attEnd, attStep);
        addUnindent();
        addSeparatorLine();
    }

    void Gui::addSliderSpotLights(Light::LightHandler & lightHandler, float posBegin, float posEnd, float posStep, float dirBegin, float dirEnd, float dirStep, float attBegin, float attEnd, float attStep){

        for(size_t i = 0; i < lightHandler._spotLights.size(); ++i){
            addIndent();
            addLabel("Spot Light: ", (int)i);
            addIndent();
            imguiSlider("SL: position.x", &lightHandler._spotLights[i]._pos.x, posBegin, posEnd, posStep);
            imguiSlider("SL: position.y", &lightHandler._spotLights[i]._pos.y, posBegin, posEnd, posStep);
            imguiSlider("SL: position.z", &lightHandler._spotLights[i]._pos.z, posBegin, posEnd, posStep);

            imguiSlider("SL: direction.x", &lightHandler._spotLights[i]._dir.x, -1, 1, 0.001);
            imguiSlider("SL: direction.y", &lightHandler._spotLights[i]._dir.y, -1, 1, 0.001);
            imguiSlider("SL: direction.z", &lightHandler._spotLights[i]._dir.z, -1, 1, 0.001);

            imgui3Slider("Red", &lightHandler._spotLights[i]._color.r, 0, 1, 0.01, 1);
            imgui3Slider("Green", &lightHandler._spotLights[i]._color.g, 0, 1, 0.01, 2);
            imgui3Slider("Blue", &lightHandler._spotLights[i]._color.b, 0, 1, 0.01, 3);

            imguiSlider("SL: angle", &lightHandler._spotLights[i]._angle, 0, 180, 0.001);
            imguiSlider("SL: falloff", &lightHandler._spotLights[i]._falloff, 0, 180, 0.001);
            imguiSlider("SL: intensity", &lightHandler._spotLights[i]._intensity, 0, 1, 0.001);
            imguiSlider("SL: attenuation", &lightHandler._spotLights[i]._attenuation, attBegin, attEnd, attStep);
            addUnindent();
        }
        addSeparatorLine();
    }

    void Gui::addSliderPointLights(Light::LightHandler & lightHandler, float posBegin, float posEnd, float posStep, float attBegin, float attEnd, float attStep){

        for(size_t i = 0; i < lightHandler._pointLights.size(); ++i){
            addIndent();
            addLabel("Point Light: ", (int)i);
            addIndent();
            imguiSlider("PL: position.x", &lightHandler._pointLights[i]._pos.x, posBegin, posEnd, posStep);
            imguiSlider("PL: position.y", &lightHandler._pointLights[i]._pos.y, -3.0, 12.0, posStep);
            imguiSlider("PL: position.z", &lightHandler._pointLights[i]._pos.z, posBegin, posEnd, posStep);
            imguiSlider("PL: intensity", &lightHandler._pointLights[i]._intensity, 0, 1, 0.001);
            imguiSlider("PL: attenuation", &lightHandler._pointLights[i]._attenuation, attBegin, attEnd, attStep);
            imgui3Slider("Red", &lightHandler._pointLights[i]._color.r, 0, 1, 0.01, 1);
            imgui3Slider("Green", &lightHandler._pointLights[i]._color.g, 0, 1, 0.01, 2);
            imgui3Slider("Blue", &lightHandler._pointLights[i]._color.b, 0, 1, 0.01, 3);
            addUnindent();
        }
        addSeparatorLine();
    }

    void Gui::addSliderSpline(Geometry::Spline3D & spline, float posBegin, float posEnd, float posStep){

        addLabel("Spline Handler");
        addSeparator();
        for(size_t i = 0; i < spline.size(); ++i){
            addIndent();
            addLabel("point");
            imguiSlider("x", &spline[i].x, posBegin, posEnd, posStep);
            imguiSlider("y", &spline[i].y, posBegin, posEnd, posStep);
            imguiSlider("z", &spline[i].z, posBegin, posEnd, posStep);
            addUnindent();
        }
    }

    bool Gui::addCheckBox(const char* text, bool checked, bool enabled){
        return imguiCheck(text, checked, enabled);
    }

    bool Gui::addItem(const char* text, bool enabled){
        return imguiItem(text, enabled);
    }

    bool Gui::addCollapse(const char* text, const char* subtext, bool checked, bool enabled){
        return imguiCollapse(text, subtext, checked, enabled);
    }

    void Gui::addValue(const char* text){
        imguiValue(text);
    }

    void Gui::addIndent()
    {
        imguiIndent();
    }

    void Gui::addUnindent()
    {
        imguiUnindent();
    }

    void Gui::addSeparator()
    {
        imguiSeparator();
    }

    void Gui::addSeparatorLine()
    {
        imguiSeparatorLine();
    }

    void Gui::scrollAreaEnd(){
        imguiEndScrollArea();
        imguiEndFrame();
        imguiRenderGLDraw(_screenWidth, _screenHeight);
    }


    int Gui::getWindowWidth() {
        return _windowWidth;
    }

    int Gui::getWindowHeight() {
        return _windowHeight;
    }

    void Gui::setWindowWidth(int width) {
        _windowWidth = width;
    }

    void Gui::setWindowHeight(int height) {
        _windowHeight = height;
    }


    glm::vec2 Gui::getMousePosition() {
        return glm::vec2(_mouseX, _mouseY);
    }
}