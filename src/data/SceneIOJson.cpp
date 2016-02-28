#include "data/SceneIOJson.hpp"
#include "fstream"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <glog/logging.h>
#include <graphics/Scene.h>
#include <rapidjson/prettywriter.h>


using namespace Data;
using namespace rapidjson;

void SceneIOJson::load(const Graphics::Scene &scene, const std::string &inPath) {
    DLOG(INFO) << "Loading scene " << inPath;
}

void SceneIOJson::save(const Graphics::Scene &scene, const std::string &outPath) {
    DLOG(INFO) << "Saving scene " << outPath;

    std::ofstream outFile(outPath);
    if(!outFile.is_open())
        throw std::runtime_error("SceneIOJson::save cannot open the file " + outPath);

    // Js init config
    Document dom;
    dom.SetObject();
    Document::AllocatorType& allocator = dom.GetAllocator();
    Value rootMeshes(kArrayType);

    for(const auto& meshInstanced : scene.meshInstances()){
        Value mesh(kObjectType);
        std::string mPath = meshInstanced->modelPath();
        mesh.AddMember(SceneIOJsonKeys::mesh_path, Value(mPath.c_str(), mPath.size(), allocator), allocator);

        Value transformations(kObjectType), positions(kArrayType), rotations(kArrayType), scales(kArrayType);
        for(const auto& transf : meshInstanced->getTransformations()){
            Value pos(kArrayType);
            Value rot(kArrayType);
            Value scale(kArrayType);

            pos.PushBack<float>(transf.position.x, allocator);
            pos.PushBack<float>(transf.position.y, allocator);
            pos.PushBack<float>(transf.position.z, allocator);

            rot.PushBack<float>(transf.rotation.x, allocator);
            rot.PushBack<float>(transf.rotation.y, allocator);
            rot.PushBack<float>(transf.rotation.z, allocator);
            rot.PushBack<float>(transf.rotation.w, allocator);

            scale.PushBack<float>(transf.scale.x, allocator);
            scale.PushBack<float>(transf.scale.y, allocator);
            scale.PushBack<float>(transf.scale.z, allocator);

            positions.PushBack(Value().SetObject().AddMember(SceneIOJsonKeys::value, pos.Move(), allocator), allocator);
            rotations.PushBack(Value().SetObject().AddMember(SceneIOJsonKeys::value, rot.Move(), allocator), allocator);
            scales.PushBack(Value().SetObject().AddMember(SceneIOJsonKeys::value, scale.Move(), allocator), allocator);
        }

        transformations.AddMember(SceneIOJsonKeys::positions, positions, allocator);
        transformations.AddMember(SceneIOJsonKeys::rotations, rotations, allocator);
        transformations.AddMember(SceneIOJsonKeys::scales, scales, allocator);
        mesh.AddMember(SceneIOJsonKeys::mesh_transformations, transformations, allocator);
        rootMeshes.PushBack(mesh.Move(), allocator);
    }
    dom.AddMember(SceneIOJsonKeys::data, rootMeshes, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuf);
    dom.Accept(writer);
    outFile << strbuf.GetString();
}

