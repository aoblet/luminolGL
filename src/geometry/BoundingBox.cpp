//
// Created by mehdi on 16/02/16.
//

#include <glog/logging.h>
#include <stdexcept>
#include "geometry/BoundingBox.h"
#include "glm/ext.hpp"

namespace Geometry
{
    BoundingBox::BoundingBox() {
        _points.reserve(8);
    }


    BoundingBox::BoundingBox(const glm::vec3 &AABBmin, const glm::vec3 &AABBmax) {
        float xmin, ymin, zmin, xmax, ymax, zmax;
        xmin = AABBmin.x;
        ymin = AABBmin.y;
        zmin = AABBmin.z;

        xmax = AABBmax.x;
        ymax = AABBmax.y;
        zmax = AABBmax.z;

        _points = {
                glm::vec3(xmin,ymin,zmin),
                glm::vec3(xmin,ymin,zmax),
                glm::vec3(xmax,ymin,zmax),
                glm::vec3(xmax,ymin,zmin),
                glm::vec3(xmin,ymax,zmin),
                glm::vec3(xmin,ymax,zmax),
                glm::vec3(xmax,ymax,zmax),
                glm::vec3(xmax,ymax,zmin)
        };

        _AABBmin = AABBmin;
        _AABBmax = AABBmax;
    }

    bool BoundingBox::isVisible(const glm::mat4 &VP)  const {
        glm::vec4 projInitPoint = VP * glm::vec4(_points[0], 1.0);
        projInitPoint /= projInitPoint.w;

        float xmin = projInitPoint.x;
        float xmax = projInitPoint.x;
        float ymin = projInitPoint.y;
        float ymax = projInitPoint.y;
        bool isInFrontOfCamera = false;

        for(auto& point : _points){
            glm::vec4 projPoint = VP * glm::vec4(point, 1.0);

            // no need to be in homogeneous coordinates
            if( projPoint.z > 0) isInFrontOfCamera = true;

            // better behavior with absolute W. Don't know why could be < 0.
            // Any mathematical explanations are welcome :)
            projPoint /= glm::abs(projPoint.w);

            if( projPoint.x < xmin) xmin = projPoint.x;
            if( projPoint.y > ymax) ymax = projPoint.y;
            if( projPoint.x > xmax) xmax = projPoint.x;
            if( projPoint.y < ymin) ymin = projPoint.y;
        }

        if(!isInFrontOfCamera)
            return false;

        // screen space limit (between -1 and 1 for each axis x,y and z)
        float limit = 1;
        return !(
                    (xmax<-limit) ||
                    (ymax<-limit) ||
                    (xmin>limit)  ||
                    (ymin>limit)
                );
    }


    void BoundingBox::transformAAB(const glm::mat4 &trans) {
        _AABBmax = glm::vec3(trans * glm::vec4(_AABBmax, 1));
        _AABBmin = glm::vec3(trans * glm::vec4(_AABBmin, 1));
    }

    bool BoundingBox::intersect(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection, const glm::mat4 &transformation, float *intersectionDistance) const {
        // Intersection method from Real-Time Rendering and Essential Mathematics for Games
        float tMin = 0.0f;
        float tMax = 100000.0f;

        glm::vec3 OBBposition_worldspace(transformation[3].x, transformation[3].y, transformation[3].z);

        glm::vec3 delta = OBBposition_worldspace - rayOrigin;

        // Test intersection with the 2 planes perpendicular to the OBB's X axis
        {
            glm::vec3 xaxis(transformation[0].x, transformation[0].y, transformation[0].z);
            float e = glm::dot(xaxis, delta);
            float f = glm::dot(rayDirection, xaxis);

            if ( fabs(f) > 0.001f ){ // Standard case

                float t1 = (e+_AABBmin.x)/f; // Intersection with the "left" plane
                float t2 = (e+_AABBmax.x)/f; // Intersection with the "right" plane
                // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

                // We want t1 to represent the nearest intersection,
                // so if it's not the case, invert t1 and t2
                if (t1>t2){
                    float w=t1;t1=t2;t2=w; // swap t1 and t2
                }

                // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
                if ( t2 < tMax )
                    tMax = t2;
                // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
                if ( t1 > tMin )
                    tMin = t1;

                // And here's the trick :
                // If "far" is closer than "near", then there is NO intersection.
                // See the images in the tutorials for the visual explanation.
                if (tMax < tMin )
                    return false;

            }else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
                if(-e+_AABBmin.x > 0.0f || -e+_AABBmax.x < 0.0f)
                    return false;
            }
        }


        // Test intersection with the 2 planes perpendicular to the OBB's Y axis
        // Exactly the same thing than above.
        {
            glm::vec3 yaxis(transformation[1].x, transformation[1].y, transformation[1].z);
            float e = glm::dot(yaxis, delta);
            float f = glm::dot(rayDirection, yaxis);

            if ( fabs(f) > 0.001f ){

                float t1 = (e+_AABBmin.y)/f;
                float t2 = (e+_AABBmax.y)/f;

                if (t1>t2){float w=t1;t1=t2;t2=w;}

                if ( t2 < tMax )
                    tMax = t2;
                if ( t1 > tMin )
                    tMin = t1;
                if (tMin > tMax)
                    return false;

            }else{
                if(-e+_AABBmin.y > 0.0f || -e+_AABBmax.y < 0.0f)
                    return false;
            }
        }

        // Test intersection with the 2 planes perpendicular to the OBB's Z axis
        // Exactly the same thing than above.
        {
            glm::vec3 zaxis(transformation[2].x, transformation[2].y, transformation[2].z);
            float e = glm::dot(zaxis, delta);
            float f = glm::dot(rayDirection, zaxis);

            if ( fabs(f) > 0.001f ){

                float t1 = (e+_AABBmin.z)/f;
                float t2 = (e+_AABBmax.z)/f;

                if (t1>t2){float w=t1;t1=t2;t2=w;}

                if ( t2 < tMax )
                    tMax = t2;
                if ( t1 > tMin )
                    tMin = t1;
                if (tMin > tMax)
                    return false;

            }else{
                if(-e+_AABBmin.z > 0.0f || -e+_AABBmax.z < 0.0f)
                    return false;
            }
        }

        *intersectionDistance = tMin;
        return true;
    }

    void BoundingBox::compute(const std::vector<Graphics::VertexDescriptor> &vertices) {

        float xmin, ymin, zmin;
        float xmax, ymax, zmax;

        xmin = xmax = vertices[0].position.x;
        ymin = ymax = vertices[0].position.y;
        zmin = zmax = vertices[0].position.z;

        for(auto& vertice : vertices){
            if (vertice.position.x < xmin) xmin = vertice.position.x;
            if (vertice.position.x > xmax) xmax = vertice.position.x;
            if (vertice.position.y < ymin) ymin = vertice.position.y;
            if (vertice.position.y > ymax) ymax = vertice.position.y;
            if (vertice.position.z < zmin) zmin = vertice.position.z;
            if (vertice.position.z > zmax) zmax = vertice.position.z;
        }

        _points = {
              glm::vec3(xmin,ymin,zmin),
              glm::vec3(xmin,ymin,zmax),
              glm::vec3(xmax,ymin,zmax),
              glm::vec3(xmax,ymin,zmin),
              glm::vec3(xmin,ymax,zmin),
              glm::vec3(xmin,ymax,zmax),
              glm::vec3(xmax,ymax,zmax),
              glm::vec3(xmax,ymax,zmin)
        };

        _AABBmin = glm::vec3(xmin, ymin, zmin);
        _AABBmax = glm::vec3(xmax, ymax, zmax);

    }

    const std::vector<glm::vec3> &BoundingBox::getVector() const{
        return _points;
    }

    BoundingBox operator*(const glm::mat4 &trans, const BoundingBox& box1) {
        Geometry::BoundingBox box;

        for(auto &point : box1._points){
            glm::vec4 transPoint = trans * glm::vec4(point,1);
            box._points.push_back(glm::vec3(transPoint));
        }

        return box;
    }


}

