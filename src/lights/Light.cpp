
#include "lights/Light.hpp"

#define DEBUG 0

namespace Light{



	LightHandler::LightHandler(){

	}

	void LightHandler::addPointLight(glm::vec3 pos, glm::vec3 color, float intensity, float attenuation){
		_pointLights.push_back(PointLight(pos,color,intensity,attenuation));
	}

	void LightHandler::addPointLight(PointLight pl){
		_pointLights.push_back(PointLight(pl._pos, pl._color, pl._intensity, pl._attenuation));
	}

	void LightHandler::addDirectionalLight(glm::vec3 pos, glm::vec3 color, float intensity, float attenuation){
		_directionnalLights.push_back(DirectionalLight(pos,color,intensity,attenuation));
	}

	void LightHandler::addDirectionalLight(DirectionalLight dl){
		_directionnalLights.push_back(DirectionalLight(dl._pos, dl._color, dl._intensity, dl._attenuation));
	}

	void LightHandler::addSpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float intensity, float attenuation, float angle, float falloff){
		_spotLights.push_back(SpotLight(pos,dir,color,intensity,attenuation,angle,falloff));
	}

	void LightHandler::addSpotLight(SpotLight sl){
		_spotLights.push_back(SpotLight(sl._pos, sl._dir, sl._color, sl._intensity, sl._attenuation, sl._angle, sl._falloff));
	}

	// bool LightHandler::isVisible(const glm::mat4 &MVP)  const {
	//         glm::vec4 projInitPoint = MVP * glm::vec4(_points[0], 1.0);

	//         if(projInitPoint.z < 0) return false;

	//         projInitPoint /= projInitPoint.w;

	//         float xmin = projInitPoint.x;
	//         float xmax = projInitPoint.x;
	//         float ymin = projInitPoint.y;
	//         float ymax = projInitPoint.y;

	//         for(auto& point : _points){
	//             glm::vec4 projPoint = MVP * glm::vec4(point, 1.0);
	//             projPoint /= projPoint.w;
	//             if( projPoint.x < xmin) xmin = projPoint.x;
	//             if( projPoint.y > ymax) ymax = projPoint.y;
	//             if( projPoint.x > xmax) xmax = projPoint.x;
	//             if( projPoint.y < ymin) ymin = projPoint.y;
	//         }

	//         float limit = 1;
	//         return !(
	//                     (xmax<-limit) ||
	//                     (ymax<-limit) ||
	//                     (xmin>limit)  ||
	//                     (ymin>limit)
	//                 );
	//     }


	bool LightHandler::isOnScreen(const glm::mat4 & MVP, std::vector<glm::vec2> & littleQuadVertices, const glm::vec3 &pos, const glm::vec3 & color, const float & intensity, const float & attenuation){

		float linear = 1.7;
        float maxBrightness = std::max(std::max(color.r, color.g), color.b);
		float radius = 1.0;
	  	radius = ( (-linear + std::sqrt(linear * linear - 4 * attenuation * (1.0 - (256.0 / 5.0) 
	        	* maxBrightness))) / (2 * attenuation) ) /2;

		float dx = radius;

        if(DEBUG) std::cout << "radius: " << dx << " && attenuation: " << attenuation << std::endl;

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

		glm::vec4 projInitPoint = MVP * glm::vec4(cube[0], 1.0);

	    // if(projInitPoint.z < 0) return false;

        projInitPoint /= projInitPoint.w;

        float xmin = projInitPoint.x;
        float xmax = projInitPoint.x;
        float ymin = projInitPoint.y;
        float ymax = projInitPoint.y;

        for(auto& point : cube){
            glm::vec4 projPoint = MVP * glm::vec4(point, 1.0);
            projPoint /= projPoint.w;
            if( projPoint.x < xmin) xmin = projPoint.x;
            if( projPoint.y > ymax) ymax = projPoint.y;
            if( projPoint.x > xmax) xmax = projPoint.x;
            if( projPoint.y < ymin) ymin = projPoint.y;
        }

        if(DEBUG) std::cout << "Left: " << xmin << " Right: " << xmax << " Top: " << ymax << " Bottom: " << ymin << std::endl; 

        littleQuadVertices.push_back(glm::vec2(xmin, ymin));
        littleQuadVertices.push_back(glm::vec2(xmax, ymin));
        littleQuadVertices.push_back(glm::vec2(xmin, ymax));
        littleQuadVertices.push_back(glm::vec2(xmax, ymax));

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






}



//         //------------------------------------ Point Lights
//        // point light shaders
//        pointLightShader.useProgram();

//        // Bind quad vao
//        glBindVertexArray(vao[2]);

//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);
//        glActiveTexture(GL_TEXTURE2);
//        glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);

//        unsigned int nbLightsByCircle[] = {6, 12, 18, 24, 30, 36, 42, 48, 54, 60, 66, 72, 78};
//        int counterCircle = 0;
//        unsigned int nbPointLights = 30;
//        float xOffset = glm::sqrt(float(instanceNumber))/2;
//        float zOffset = glm::sqrt(float(instanceNumber))/2;

//        float rayon = sqrt(xOffset*2 + zOffset*2);


//        int cptVisiblePointLight = 0;

//        std::vector<Light> lights;

//        for(size_t i = 0; i < nbPointLights; ++i){

//            Light light(glm::vec3(0,0,0), glm::vec3(1,1,1), lightIntensity, lightAttenuation);

//            if( i == nbLightsByCircle[counterCircle] ){
//              counterCircle++;
//              rayon += 3;
//            }


//            float coeff = rayon * sin(t);
//            float w = t + t;

// //            coeff = 20;
// //            w = 0;

//            light._pos = glm::vec3(
//                coeff * cos(i+ M_PI /nbPointLights) + xOffset,
//                pointLightsYOffset,
//                coeff * sin(i+ M_PI /nbPointLights) + zOffset);

//            light._color.x = cos(i);
//            light._color.y = sin(3*i);
//            light._color.y = cos(i*2);


//            glBindBuffer(GL_UNIFORM_BUFFER, ubo[0]);
//            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Light), &light);
//            glBindBuffer(GL_UNIFORM_BUFFER, 0);

// //            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

//            lights.push_back(light);

//        }