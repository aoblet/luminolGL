#pragma once

#include "data/SceneIO.hpp"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <rapidjson/document.h>
#include <graphics/ModelMeshInstanced.hpp>

namespace Data{

    /**
     * Handle IO scene with json format
     * The json structure is not fully optimized for manual modification purposes.
     *
     *  {
     *      "data": [
     *          {
     *              "mesh_path": '../assets/<model>",
     *              "mesh_transformations": {
     *                  "positions": [
     *                      {"value": [x1, y1, z1]},
     *                      {"value": [x2, y2, z2]},
     *                  ],
     *                  "rotations": [
     *                      {"value": [x1, y1, z1, theta1]},
     *                      {"value": [x2, y2, z2, theta2]},
     *                  ],
     *                  "scales": [
     *                      {"value": [x1, y1, z1]},
     *                      {"value": [x2, y2, z2]},
     *                  ],
     *              }
     *          },
     *          {
     *              other_mesh
     *          }
     *      ],
     *
     *      "water":[
     *          "mesh_transformations": {}
     *      ]
     *
     *  }
     */

    namespace SceneIOJsonKeys{
        const char value[] = "value";
        const char data[] = "data";
        const char mesh_path[] = "mesh_path";
        const char mesh_transformations[] = "mesh_transformations";
        const char positions[] = "positions";
        const char rotations[] = "rotations";
        const char scales[] = "scales";
        const char water[] = "water";
    }

    class SceneIOJson: public SceneIO{
        bool _isPrettyOutput;

        void addVec3ToJson(const glm::vec3& vec, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator);
        void addVec4ToJson(const glm::vec4& vec, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator);
        glm::vec3 jsonArrayToVec3(const rapidjson::Value& value);
        glm::vec4 jsonArrayToVec4(const rapidjson::Value& value);

        void writeTransformations(const Graphics::ModelMeshInstanced& mesh, rapidjson::Value& to, rapidjson::Document::AllocatorType& allocator);
        std::vector<Geometry::Transformation> readTransformationsFromJS(rapidjson::Value& transformationsJS);

    public:
        SceneIOJson(bool isPrettyOutput=true);
        void load(Graphics::Scene &scene, const std::string& inPath) override ;
        void save(const Graphics::Scene &scene, const std::string& outPath) override ;

    };
}