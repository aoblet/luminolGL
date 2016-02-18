#pragma once

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

namespace Light{
	
	/**
    * Gui class: Handler for all the Lights.
    */


	enum LightType{
	    POINT,
	    DIRECTIONNAL,
	    SPOT
	};

	struct PointLight
	{
	    glm::vec3 _pos;
	    int _padding;
	    glm::vec3 _color;
	    float _intensity;
	    float _attenuation;

	    PointLight(glm::vec3 pos=glm::vec3(0,0,0), glm::vec3 color=glm::vec3(0,0,0), float intensity=0.8, float attenuation=2.0){
	        update(pos, color, intensity, attenuation);
	    }

	    void update(glm::vec3 pos, glm::vec3 color, float intensity, float attenuation){
	        _pos = pos;
	        _color = color;
	        _intensity = intensity;
	        _attenuation = attenuation;
	    }

	};

	struct DirectionalLight
	{
	    glm::vec3 _pos; // Direction
	    int _padding;
	    glm::vec3 _color;
	    float _intensity;
	    float _attenuation;

	    DirectionalLight(glm::vec3 pos=glm::vec3(0,0,0), glm::vec3 color=glm::vec3(0,0,0), float intensity=0.2, float attenuation=0.2){
	        update(pos, color, intensity, attenuation);
	    }

	    void update(glm::vec3 pos, glm::vec3 color, float intensity, float attenuation){
	        _pos = pos;
	        _color = color;
	        _intensity = intensity;
	        _attenuation = attenuation;
	    }
	};

	struct SpotLight
	{
	    glm::vec3 _pos;
	    int _pad6ding1; //16

	    glm::vec3 _color;

	    float _intensity; //32

	    float _attenuation;
	    int _padding2;
	    int _padding3;
	    int _padding4; //48

	    glm::vec3 _dir;

	    float _angle; //64

	    float _falloff; //68

	    SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float intensity, float attenuation, float angle, float falloff){
	        update(pos, dir, color, intensity, attenuation, angle, falloff);
	    }

	    SpotLight(){};

	    void update(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float intensity, float attenuation, float angle, float falloff){
	        _pos = pos;
	        _dir = dir;
	        _color = color;
	        _intensity = intensity;
	        _attenuation = attenuation;
	        _angle = angle;
	        _falloff = falloff;
	    }
	};



	class LightHandler{

		public:

		    float _lightAttenuation = 8; 							/** Global attenuation for all the light */
		    float _lightIntensity = 1; 								/** Global intensity for all the light */
		    float _lightAttenuationThreshold = 0.01;				/** Global attenuation threshold for all the light */
		    float _specularPower = 20;								/** Global specular power for all the light */

		    std::vector<PointLight> _pointLights;					/** A vector containing all the point lights that we want to render */
   	 		std::vector<DirectionalLight> _directionnalLights;		/** A vector containing all the directionnal lights */	
   	 		std::vector<SpotLight> _spotLights;						/** A vector containing all the spot lights */

			LightHandler();

   	 		void addPointLight(glm::vec3 pos=glm::vec3(0,0,0), glm::vec3 color=glm::vec3(0,0,0), float intensity=0.2, float attenuation=0.2);
   	 		void addPointLight(PointLight pl);
   	 		void addDirectionalLight(glm::vec3 pos=glm::vec3(0,0,0), glm::vec3 color=glm::vec3(0,0,0), float intensity=0.2, float attenuation=0.2);
   	 		void addDirectionalLight(DirectionalLight dl);
   	 		void addSpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float intensity, float attenuation, float angle, float falloff);	
			void addSpotLight(SpotLight sl);

			bool isOnScreen(const glm::mat4 & mvp, std::vector<glm::vec2> & littleQuadVertices, const glm::vec3 & pos, const glm::vec3 & color, const float & intensity, const float & attenuation);

			float getSpecularPower(){ return _specularPower; }
			float getLightAttenuation(){ return _lightAttenuation; }
			float geLightIntensity(){ return _lightIntensity; }
			float geLightAttenuationThreshold(){ return _lightAttenuationThreshold; }

			void setSpecularPower(float specularPower){ _specularPower = specularPower; }
			void setLightAttenuation(float lightAttenuation){ _lightAttenuation = lightAttenuation; }
			void setLightIntensity(float lightIntensity){ _lightIntensity = lightIntensity; }
			void setLightAttenuationTreshold(float lightAttenuationTreshold){ _lightAttenuationThreshold = lightAttenuationTreshold; }



	};

}
