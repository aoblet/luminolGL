#pragma once

#include "data/SceneIO.hpp"


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
     *                      {"v": [x1, y1, z1]},
     *                      {"v": [x2, y2, z2]},
     *                  ],
     *                  "rotations": [
     *                      {"v": [x1, y1, z1, theta1]},
     *                      {"v": [x2, y2, z2, theta2]},
     *                  ],
     *                  "scales": [
     *                      {"v": [x1, y1, z1]},
     *                      {"v": [x2, y2, z2]},
     *                  ],
     *              }
     *          },
     *          {
     *              other_mesh
     *          }
     *      ]
     *
     *  }
     */

    namespace SceneIOJsonKeys{
        const char value[] = "v";
        const char data[] = "data";
        const char mesh_path[] = "mesh_path";
        const char mesh_transformations[] = "mesh_transformations";
        const char positions[] = "positions";
        const char rotations[] = "rotations";
        const char scales[] = "scales";
    }

    class SceneIOJson: public SceneIO{
    public:
        void load(const Graphics::Scene &scene, const std::string& inPath) override ;
        void save(const Graphics::Scene &scene, const std::string& outPath) override ;
    };
}