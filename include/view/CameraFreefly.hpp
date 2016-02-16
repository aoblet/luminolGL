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
        glm::mat4 _viewMatrix;      /** Transform matrix obtained by glm::lookAt */
        glm::mat4 _projectionMatrix;/** Transform matrix obtained by glm::perspective */
        glm::vec3 _eye;             /** World camera position */
        glm::vec3 _up;              /** Up world vector */
        glm::vec3 _front;           /** Where the camera looks */
        glm::vec3 _frontLeft;       /** Left vector from front: used for Up*/
        glm::vec2 _sphericalAngles; /** In radians */

        float _fov;
        glm::vec2 _viewPort;
        glm::vec2 _nearFar;

        void move(const glm::vec3& movement);
        void rotate(const glm::vec2& angles);

    public:
        CameraFreefly(const glm::vec2& viewPort, const glm::vec2& nearFar, float fov=45);
        void computeDirs();
        void update(const glm::vec2& angles, const glm::vec3& movement);
        void updateFromTarget(const glm::vec3& target);  /** Useful for splines orientation */
        void updateProjectionProperties(const glm::vec2& viewPort, const glm::vec2& nearFar, float fov);
        void updateProjection();
        void setFront(const glm::vec3& front);
        void setEye(const glm::vec3& eye);

        glm::vec3 getEye() const;
        glm::vec3 getFront() const;
        glm::vec2 getViewPort() const;
        glm::vec2 getNearFar() const;
        float getFOV() const;


        const glm::mat4& getViewMatrix() const;
        const glm::mat4& getProjectionMatrix() const;
        friend std::ostream& operator<<(std::ostream& out, const CameraFreefly& c);
    };
    std::ostream& operator<<(std::ostream& out, const CameraFreefly& c);
}


