#pragma once

#include <string>

namespace Graphics{
    class Scene;
}

namespace Data{
    /**
     * Abstract class used for loading saving a scene(IN OUT) - Strategy pattern.
     */

    class SceneIO{
    public:
        virtual void load(Graphics::Scene& scene, const std::string& inPath) = 0;
        virtual void save(const Graphics::Scene& scene, const std::string& outPath) = 0;
    };
}