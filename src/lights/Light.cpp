
#include "lights/Light.hpp"



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

	bool LightHandler::isOnScreen(const glm::mat4 & mvp, std::vector<glm::vec2> & littleQuadVertices, const glm::vec3 &pos, const glm::vec3 & color, const float & intensity){

		float linear = 1.7;
        float quadratic = 0.5; 
        float maxBrightness = std::max(std::max(color.r, color.g), color.b);
        float radius = ( (-linear + std::sqrt(linear * linear - 4 * quadratic * (1.0 - (256.0 / 5.0) * maxBrightness))) / (2 * quadratic) ) /2;
        float dx = radius;

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

        littleQuadVertices.push_back(glm::vec2(mostLeft.x, mostBottom.y));
        littleQuadVertices.push_back(glm::vec2(mostRight.x, mostBottom.y));
        littleQuadVertices.push_back(glm::vec2(mostLeft.x, mostTop.y));
        littleQuadVertices.push_back(glm::vec2(mostRight.x, mostTop.y));

        // Render quad si au moins une partie dela light est dans l'écran
        float limit = 1.;
        
        if( ( (mostLeft.x > -limit && mostLeft.x < limit) || (mostRight.x > -limit && mostRight.x < limit) ) 
            &&  ( (mostTop.y > -limit && mostTop.y < limit) || (mostBottom.y > -limit && mostBottom.y < limit) ) )
       	{
       		// std::cout << "visible" << std::endl;
       		return true;
       	}
       	// else std::cout << "NON visible" << std::endl;



       return false;
	}




}