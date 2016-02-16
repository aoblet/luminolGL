#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>


namespace View{
    /**
    * Freefly camera: free movement Camera. Like spectator FPS game camera.
    * Looks by default in Z direction.
    */

    class CameraFreefly {
        glm::mat4 _matrix;          /** Transform matrix obtained by glm::lookAt */
        glm::vec3 _eye;             /** World camera position */
        glm::vec3 _up;              /** Up world vector */
        glm::vec3 _front;           /** Where the camera looks */
        glm::vec3 _frontLeft;       /** Left vector from front: used for Up*/
        glm::vec2 _sphericalAngles; /** In radians */

        void move(const glm::vec3& movement);
        void rotate(const glm::vec2& angles);

    public:
        void computeDirs();
        void update(const glm::vec2& angles, const glm::vec3& movement);
        void updateFromTarget(const glm::vec3& target);  /** Useful for splines orientation */

        void setFront(const glm::vec3& front);
        glm::vec3 getFront() const;

        void setEye(const glm::vec3& eye);
        glm::vec3 getEye() const;

        const glm::mat4& getViewMatrix() const;
        friend std::ostream& operator<<(std::ostream& out, const CameraFreefly& c);
    };
    std::ostream& operator<<(std::ostream& out, const CameraFreefly& c);
}


