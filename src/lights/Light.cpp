
#include "lights/Light.hpp"



namespace Light{



	LightHandler::LightHandler(){

	}

	void LightHandler::addDirectionalLight(glm::vec3 pos, glm::vec3 color, float intensity, float attenuation){
		_directionnalLights.push_back(DirectionalLight(pos,color,intensity,attenuation));
	}


	void LightHandler::addSpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float intensity, float attenuation, float angle, float falloff){
		_spotLights.push_back(SpotLight(pos,dir,color,intensity,attenuation,angle,falloff));
	}




}