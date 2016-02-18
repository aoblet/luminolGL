
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

	bool LightHandler::isOnScreen(const glm::mat4 & mvp, std::vector<glm::vec2> & littleQuadVertices, const glm::vec3 &pos, const glm::vec3 & color, const float & intensity, const float & attenuation){

		// pow(length(illu.l), PointLight.Attenuation)
		float linear = 1.7;
		// // linear = 1.3;
  //       float quadratic = attenuation; 
  //       // quadratic = 2.0;
		float radius = 1.0;
        float maxBrightness = std::max(std::max(color.r, color.g), color.b);
  //	radius = ( (-linear + std::sqrt(linear * linear - 4 * quadratic * (1.0 - (256.0 / 5.0) 
  //       	* maxBrightness))) / (2 * quadratic) ) /2;
        
        // float ret = (-Light.Attenuation.Linear + sqrtf(Light.Attenuation.Linear * Light.Attenuation.Linear -
        // 4 * Light.Attenuation.Exp * (Light.Attenuation.Exp - 256 * MaxChannel * Light.DiffuseIntensity)))
        //     /
        // (2 * Light.Attenuation.Exp);

        radius = (-linear + sqrtf(linear * linear -
        4 * std::exp(attenuation) * (std::exp(attenuation) - 256 * maxBrightness * intensity)))
            /
        (2 * std::exp(attenuation));

        float dx = radius;

        if(DEBUG) std::cout << "radius: " << dx << " && attenuation: " << attenuation << std::endl;

        // création d'un cube d'influence autour de la point light
        std::vector<glm::vec3> cube;
        float px = pos.x;
        float py = pos.y;
        float pz = pos.z;
        cube.push_back(glm::vec3(px-dx,py-dx,pz-dx)); // left bot back
        cube.push_back(glm::vec3(px+dx,py-dx,pz-dx)); // right bot back
        cube.push_back(glm::vec3(px-dx,py-dx,pz+dx)); // left bot front
        cube.push_back(glm::vec3(px+dx,py-dx,pz+dx)); // right bot front
        cube.push_back(glm::vec3(px+dx,py+dx,pz-dx)); // right top back
        cube.push_back(glm::vec3(px-dx,py+dx,pz-dx)); // left top back
        cube.push_back(glm::vec3(px-dx,py+dx,pz+dx)); // left top front
        cube.push_back(glm::vec3(px+dx,py+dx,pz+dx)); // right top front
    
        // float wt = t;
        // wt = 0;
        // glm::mat4 rotateMatrix = rotationMatrix(glm::vec3(0.,-1.,0.) , wt);

        // glm::vec4 projInitPoint = mvp * rotateMatrix * glm::vec4(cube[0], 1.0);
        glm::vec4 projInitPoint = mvp * glm::vec4(cube[0], 1.0);
        projInitPoint /= projInitPoint.w;
        glm::vec2 mostLeft(glm::vec2(projInitPoint.x,projInitPoint.y));
        glm::vec2 mostRight(glm::vec2(projInitPoint.x,projInitPoint.y));
        glm::vec2 mostTop(glm::vec2(projInitPoint.x,projInitPoint.y));
        glm::vec2 mostBottom(glm::vec2(projInitPoint.x,projInitPoint.y));

        for(int k=1; k<8; ++k){
            // glm::vec4 projPoint = mvp * rotateMatrix *  glm::vec4(cube[k], 1.0);
            glm::vec4 projPoint = mvp *  glm::vec4(cube[k], 1.0);
            projPoint /= projPoint.w;
            if( projPoint.x < mostLeft.x) mostLeft = glm::vec2(projPoint.x, projPoint.y);
            if( projPoint.y > mostTop.y) mostTop = glm::vec2(projPoint.x, projPoint.y);
            if( projPoint.x > mostRight.x) mostRight = glm::vec2(projPoint.x, projPoint.y);
            if( projPoint.y < mostBottom.y) mostBottom = glm::vec2(projPoint.x, projPoint.y);

        }   

        if(DEBUG) std::cout << "Left: " << mostLeft.x << " Right: " << mostRight.x << " Top: " << mostTop.y << " Bottom: " << mostBottom.y << std::endl; 

        littleQuadVertices.push_back(glm::vec2(mostLeft.x, mostBottom.y));
        littleQuadVertices.push_back(glm::vec2(mostRight.x, mostBottom.y));
        littleQuadVertices.push_back(glm::vec2(mostLeft.x, mostTop.y));
        littleQuadVertices.push_back(glm::vec2(mostRight.x, mostTop.y));

        // Render quad si au moins une partie dela light est dans l'écran
        float limit = 2; // tmp value for debugging (have to be 1 for maximum optimisation!)
        
        if( ( (mostLeft.x > -limit && mostLeft.x < limit) || (mostRight.x > -limit && mostRight.x < limit) ) 
            &&  ( (mostTop.y > -limit && mostTop.y < limit) || (mostBottom.y > -limit && mostBottom.y < limit) ) )
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