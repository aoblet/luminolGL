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

	enum class PointLightBehavior{
		FIXE,
		FIREFLY,
		RANDOM_DISPLACEMENT,
		SUN,
		TORNADO,
		RISING
	};

	/**
     * Point Light structure, it will be used for rendering our different fireflies
     */
	struct PointLight
	{
	    glm::vec3 _pos; 						/** Position of the point light ... */
	    int _padding; 							/** padding for ubo */
	    glm::vec3 _color; 						/** color */
	    float _intensity; 						/** intensity */
	    float _attenuation;						/** attenuation  */
	    PointLightBehavior _type; 				/** type of the firefly: FIXE, RANDOM_DISPLACEMENT, SUN, TORNADO OR RISING_FIREFLY */
	    int _lastChangeDir; 					/** time before a random firefly change direction */
	    float _multVelocity; 					/** multiplicator for velocity so every firefly do not have the same speed */
	    bool _changeDir = true; 				/** boolean to handle if a firefly can change direction or not */
	    int _cptTime = 0; 						/** counter useful for _changeDir */
	    glm::vec3 _vel = glm::vec3(0, 0, 0); 	/** Velocity, speed of the  */

		 /** Point Light constructor with default values */
	    PointLight(glm::vec3 pos=glm::vec3(0,0,0), glm::vec3 color=glm::vec3(0,0,0), float intensity=0.8, float attenuation=2.0, PointLightBehavior type=PointLightBehavior::FIXE, int lastChangeDir=10, float multVelocity=1.f)
	    {
	        update(pos, color, intensity, attenuation, type, lastChangeDir, multVelocity);
	    }

	     /** update a point light struct*/
	    void update(glm::vec3 pos, glm::vec3 color, float intensity, float attenuation, PointLightBehavior type=PointLightBehavior::FIXE, int lastChangeDir = 5, float multVelocity=1.f)
	    {
	        _pos = pos;
	        _color = color;
	        _intensity = intensity;
	        _attenuation = attenuation;
	        _type = type;
	        _lastChangeDir = lastChangeDir;
	        _multVelocity = multVelocity;
	    }

	     /** change the position of a rising fireflies to make it rise toward the sky*/
	    void computeRisingFireflies(float & t)
	    {
	    	_pos += glm::vec3(0, 0.05, 0);
	    	if(_pos.y > 150) _pos.y = -10;
	    }

	     /** change the position of a random fireflies according to its velocity and lastChangeDir parameters */
	    void computeRandomFireflies(float & t) 
	    {
	    	double x = ( rand() % 20 ) - 10;
	    	double y = ( rand() % 20 ) - 10;
	    	double z = ( rand() % 20 ) - 10;

	    	int _time = static_cast<int>(t);
	    	bool changeAnyway = false;
	    	int mod = _time%_lastChangeDir;

	    	if(_pos.y < 0 || _pos.y > 200){ changeAnyway = true; _pos.y = 0.01; }

	    	if(_time > _cptTime && mod == 0) _changeDir = true;

	    	if( ( mod == 0 && _changeDir ) || changeAnyway) {
	    		_vel = glm::vec3(x, y, z); 
	    		_vel /= 80.f;
	    		_vel.y /= 2.f;

	    		_cptTime+=_lastChangeDir;
	    		if(_time == _cptTime) _changeDir = false; 
	    	}

	    	_pos += _vel * _multVelocity;

	    }

	};
	 /** Directional Light structure */
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

	 /** Spot Light structure */
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


	 /** 
	  * LightHandler Class
	  * Instanciate and handle all kind of lights	
	  */
	class LightHandler{

		public:

		    float _lightAttenuation = 8; 							/** Global attenuation for all the light */
		    float _lightIntensity = 1; 								/** Global intensity for all the light */
		    float _lightAttenuationThreshold = 0.01;				/** Global attenuation threshold for all the light */
		    float _specularPower = 100;								/** Global specular power for all the light */

		    std::vector<PointLight> _pointLights;					/** A vector containing all the point lights that we want to render */
   	 		DirectionalLight _directionalLight;
   	 		std::vector<SpotLight> _spotLights;						/** A vector containing all the spot lights */

			LightHandler();

   	 		void addPointLight(glm::vec3 pos=glm::vec3(0,0,0), glm::vec3 color=glm::vec3(0,0,0), float intensity=0.2, float attenuation=0.2, PointLightBehavior type=PointLightBehavior::FIXE, int lastChangeDir=10, float multVelocity=1.f);
   	 		void addPointLight(PointLight pl);
   	 		void setDirectionalLight(glm::vec3 pos = glm::vec3(0, 0, 0), glm::vec3 color = glm::vec3(0, 0, 0),
                                     float intensity = 0.2, float attenuation = 0.2);
   	 		void setDirectionalLight(const DirectionalLight& dl);
   	 		void addSpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float intensity, float attenuation, float angle, float falloff);	
			void addSpotLight(const SpotLight& sl);

			glm::mat4 rotationMatrix(glm::vec3 axis, float angle);

			/** @return bool 
			 * Tell us if a light is visible or not, if so, return a small quad to render the light in it
			 */
			bool isOnScreen(const glm::mat4 & mvp, std::vector<glm::vec2> & littleQuadVertices, const glm::vec3 & pos, const glm::vec3 & color, const float & intensity, const float & attenuation, const PointLightBehavior & type, float & t);
			
			/** Instanciate point lights to act like they are a fireflies tornado  */
			void createFirefliesTornado(int fd, int rayon, int step, const int & nbFireflies, const int & multCounterCircle, float w, float yFirstHeight = 5.5f);
			
			/** Instanciate point lights to act like rising fireflies */
			void createRisingFireflies(const int nbFireflies=10, const int x=50, const int z=50, const int y=20, const glm::vec3 center=glm::vec3(0,0,0));
			
			/** Instanciate point lights to behave randomly 
			 *   @params: x, y and z represent a rectangle with the width x, height y and depth is z.
			 *   	Fireflies will spawn in this rectangle.  
			 */
			void createRandomFireflies(const int nbFireflies, const int x, const int z, const int y, const glm::vec3 center=glm::vec3(0,0,0));

			glm::vec3 getRandomColors(int i);
			glm::vec3 getNotSoRandomColors(int i);

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
