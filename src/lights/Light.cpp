
#include "lights/Light.hpp"

#define DEBUG 0

namespace Light{



	LightHandler::LightHandler(){

	}

	void LightHandler::addPointLight(glm::vec3 pos, glm::vec3 color, float intensity, float attenuation, PointLightBehavior type, int lastChangeDir, float multVelocity){
		_pointLights.push_back(PointLight(pos,color,intensity,attenuation, type, lastChangeDir, multVelocity));
	}

	void LightHandler::addPointLight(PointLight pl){
		_pointLights.push_back(PointLight(pl._pos, pl._color, pl._intensity, pl._attenuation, pl._type));
	}

	void LightHandler::setDirectionalLight(glm::vec3 pos, glm::vec3 color, float intensity, float attenuation){
        setDirectionalLight(DirectionalLight(pos, color, intensity, attenuation));
	}

	void LightHandler::setDirectionalLight(const DirectionalLight& dl){
		_directionalLight = dl;
	}

	void LightHandler::addSpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float intensity, float attenuation, float angle, float falloff){
		_spotLights.push_back(SpotLight(pos,dir,color,intensity,attenuation,angle,falloff));
	}

	void LightHandler::addSpotLight(const SpotLight& sl){
		_spotLights.push_back(sl);
	}

	glm::mat4 LightHandler::rotationMatrix(glm::vec3 axis, float angle)
	{
	    axis = normalize(axis);
	    float s = sin(angle);
	    float c = cos(angle);
	    float oc = 1.0 - c;

	    return glm::mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
	                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
	                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
	                0.0,                                0.0,                                0.0,                                1.0);
	}

	bool LightHandler::isOnScreen(const glm::mat4 & MVP, std::vector<glm::vec2> & littleQuadVertices, const glm::vec3 &pos, const glm::vec3 & color, const float & intensity, const float & attenuation, const PointLightBehavior & type, float & t){

		float linear = 1.7;
        float maxBrightness = std::max(std::max(color.r, color.g), color.b);
		float radius = 1.0;
	  	radius = ( (-linear + std::sqrt(linear * linear - 4 * attenuation * (1.0 - (256.0 / 5.0) 
	        	* maxBrightness))) / (2 * attenuation) ) /4 ;


		// radius =  intensity / attenuation * 50;
		
		float dx = radius;

        if(DEBUG) std::cout << "radius: " << dx << " && attenuation: " << attenuation << " && maxBrightness: " << maxBrightness << std::endl;

        // création d'un cube d'influence autour de la point light
        std::vector<glm::vec3> cube;
        float px = pos.x; float py = pos.y; float pz = pos.z;
        cube.push_back(glm::vec3(px-dx,py-dx,pz-dx)); // left bot back
        cube.push_back(glm::vec3(px+dx,py-dx,pz-dx)); // right bot back
        cube.push_back(glm::vec3(px-dx,py-dx,pz+dx)); // left bot front
        cube.push_back(glm::vec3(px+dx,py-dx,pz+dx)); // right bot front
        cube.push_back(glm::vec3(px+dx,py+dx,pz-dx)); // right top back
        cube.push_back(glm::vec3(px-dx,py+dx,pz-dx)); // left top back
        cube.push_back(glm::vec3(px-dx,py+dx,pz+dx)); // left top front
        cube.push_back(glm::vec3(px+dx,py+dx,pz+dx)); // right top front

        glm::vec3 axis = glm::vec3(0.0,1.0,0.0);
        float w = 0;
        if(type == PointLightBehavior::TORNADO){
        	axis = glm::vec3(0.0,-1.0,0.0);	
        	w = t;
        }

        glm::mat4 rotateMatrix = rotationMatrix(axis , w);
		glm::vec4 projInitPoint = MVP * rotateMatrix * glm::vec4(cube[0], 1.0);

	    if(projInitPoint.z < 0) return false;

        projInitPoint /= projInitPoint.w;

        float xmin = projInitPoint.x;
        float xmax = projInitPoint.x;
        float ymin = projInitPoint.y;
        float ymax = projInitPoint.y;

        for(auto& point : cube){
            glm::vec4 projPoint = MVP * rotateMatrix * glm::vec4(point, 1.0);
            projPoint /= projPoint.w;
            if( projPoint.x < xmin) xmin = projPoint.x;
            if( projPoint.y > ymax) ymax = projPoint.y;
            if( projPoint.x > xmax) xmax = projPoint.x;
            if( projPoint.y < ymin) ymin = projPoint.y;
        }


        if(DEBUG) std::cout << "Left: " << xmin << " Right: " << xmax << " Top: " << ymax << " Bottom: " << ymin << std::endl; 

        float m = 0.02f; // mult
        if(type == PointLightBehavior::SUN){
        	m = 2.f;
        }
        littleQuadVertices.push_back(glm::vec2(xmin-m, ymin-m));
        littleQuadVertices.push_back(glm::vec2(xmax+m, ymin-m));
        littleQuadVertices.push_back(glm::vec2(xmin-m, ymax+m));
        littleQuadVertices.push_back(glm::vec2(xmax+m, ymax+m));

        float limit = 1.1;
		// if( ( (xmin > -limit && xmin < limit) || (xmax > -limit && xmax < limit) ) && ( (ymax > -limit && ymax < limit) || (ymin > -limit && ymin < limit) )  )
        if(!( (xmax<-limit) || (ymax<-limit) || (xmin>limit) || (ymin>limit) ) )
        {
        	if(DEBUG) std::cout << "visible" << std::endl;
        	return true;
        }

		if(DEBUG) std::cout << "NON visible" << std::endl;
		return false;
	}	


	void LightHandler::createFirefliesTornado(int fd, int rayon, int step, const int & nbFireflies, const int & multCounterCircle, float w, float yFirstHeight){
		srand (time(NULL));

	    int counterCircle = 0; 
	    for(int i = 0; i < nbFireflies; ++i){

	        if( i == counterCircle*multCounterCircle ){ 
	          counterCircle++;
	          rayon += step; 
	        } 

	        glm::vec3 fireflyPosition = glm::vec3( 
	            fd + 3 * cos(i+w*2* M_PI /nbFireflies)  
	            ,yFirstHeight + rayon
	            ,fd + 3 * sin(i+w*2* M_PI /nbFireflies) 
	        );

	        glm::vec3 fireflyColor = getRandomColors(counterCircle);
	        fireflyColor = glm::vec3(0.8,0.2,0.8);
	        float intensity = 0.1;

	        addPointLight(fireflyPosition, fireflyColor, intensity, 2.0, Light::PointLightBehavior::TORNADO);
		}	

	}

	void LightHandler::createRisingFireflies(const int nbFireflies, const int x, const int z, const int y, const glm::vec3 center){
		srand (time(NULL));

		for(int i = 0; i < nbFireflies; ++i){
			
			glm::vec3 fireflyPosition = center + glm::vec3( 
	            rand() % (2*x) + 1 - x
	            , - rand() % y
	            , rand() % (2*z) + 1 - z
	        );

	        glm::vec3 fireflyColor = getRandomColors(i);
	        float multVelocity = ( rand() % 30 + 10 ) / 10;
	        float intensity = 0.1;

	        addPointLight(fireflyPosition, fireflyColor, intensity, 2.0, Light::PointLightBehavior::RISING, 5, multVelocity);
	    }

	}



	void LightHandler::createRandomFireflies(const int nbFireflies, const int x, const int z, const int y, const glm::vec3 center){
		
		srand (time(NULL));

		for(int i = 0; i < nbFireflies; ++i){
			
			glm::vec3 fireflyPosition = center + glm::vec3( 
	            rand() % (2*x) + 1 - x
	            , rand() % y + 5
	            , rand() % (2*z) + 1 - z
	        );


	        glm::vec3 fireflyColor = getRandomColors(i);

	        int lastChangeDir = rand() % 15 + 4;
	        float multVelocity = ( rand() % 30 + 10 ) / 10;
	        float intensity = 0.1;

	        addPointLight(fireflyPosition, fireflyColor, intensity, 2.0, Light::PointLightBehavior::RANDOM_DISPLACEMENT, lastChangeDir, multVelocity);
	    }

	}

	glm::vec3 LightHandler::getRandomColors(int i){
		
		// float red = fmaxf(sin(i)+cos(i), 0.1);
	 	//	float green = fmaxf(cos(i), 0.1);
	 	//	float blue = fmaxf(sin(i)+cos(i), 0.1);

	    float red = ( rand() % 10 ) / 10.f;
	    float green = ( rand() % 10 ) / 10.f;
	    float blue = ( rand() % 10 ) / 10.f;

		// si la lumière n'est pas assez puissante on va booster un channel au hasard
	    if(red<0.4 && green <0.4 && blue < 0.4){
	        int r = rand() % 3 + 1;
	        if(r==1) blue +=0.5; else if(r==2) green +=0.5; else red +=0.5; 
	    } 
	    else if(red > 0.8 && green > 0.8 && blue > 0.8){
	    	int r = rand() % 3 + 1;
	        if(r==1) blue -=0.6; else if(r==2) green -=0.6; else red -=0.6; 
	    }
	    else if(red > 0.8 && blue > 0.8){
	    	int r = rand() % 2 + 1;
	        if(r==1) blue -=0.3; else red -=0.3; 
	    }

	    // std::cout << red << " " << green << " " << blue << std::endl;
	    return glm::vec3( red , green , blue);
	}


}
