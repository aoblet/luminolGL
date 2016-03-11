//
// Created by mehdi on 16/02/16.
//

#ifndef LUMINOLGL_BOUNDINGBOX_H
#define LUMINOLGL_BOUNDINGBOX_H

#include <vector>
#include <glm/detail/type_vec.hpp>

#include "graphics/VertexDescriptor.h"
#include "view/CameraFreefly.hpp"

namespace Geometry
{
    /** Geometry class to describe a BoundingBox */
    class BoundingBox {
    private:
        std::vector<glm::vec3> _points; /** points which are composing the bounding box */
        glm::vec3 _AABBmin; /** min coordinates in AABB model (axis-aligned bounding box) */
        glm::vec3 _AABBmax; /** max coordinates in AABB model (axis-aligned bounding box) */
    public:
        BoundingBox();
        BoundingBox(const glm::vec3& AABBmin, const glm::vec3& AABBmax);

        bool isVisible(const glm::mat4 &MVP) const; /** Check if the bounding is visible
                                                     * by a matrix (usually MVP)
                                                     */

        bool intersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::mat4& trans, float* distance) const;


        void compute(const std::vector<Graphics::VertexDescriptor>& vertices); /** Construct a the boundingBox using vertices as parameter
                                                                                *  this method finds the extreme points of the list to create a Box
                                                                                */


        const std::vector<glm::vec3>& getVector() const; /** Returns the 8 points composing the BoundingBox */

        void transformAAB(const glm::mat4& trans);


        friend BoundingBox operator*(const glm::mat4 &trans, const BoundingBox& box1); /** This operator makes things easier :
                                                                                        * You can directly multiply a BoundingBox
                                                                                        * with a Tranformation matrix to get a new one !
                                                                                        */
    };
    BoundingBox operator*(const glm::mat4 &trans, const BoundingBox& box1);

}






#endif //LUMINOLGL_BOUNDINGBOX_H
