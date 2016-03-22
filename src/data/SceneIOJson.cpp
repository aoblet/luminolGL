#include "data/SceneIOJson.hpp"
#include "graphics/Scene.h"
#include <fstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <glog/logging.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>
#include <string>


using namespace Data;
using namespace rapidjson;

SceneIOJson::SceneIOJson(bool isPrettyOutput): _isPrettyOutput(isPrettyOutput){ }

void SceneIOJson::load(Graphics::Scene &scene, const std::string &inPath) {
    DLOG(INFO) << "Loading scene " << inPath;

    std::ifstream inFile(inPath);
    std::stringstream buffer;
    buffer << inFile.rdbuf();

    if(!inFile.is_open())
        throw std::runtime_error("SceneIOJson::save cannot open the file " + inPath);

    Document dom;
    dom.Parse(buffer.str().c_str());

    if(dom.HasParseError())
        throw std::runtime_error("SceneIOJson::load error while parsing json data: " + std::string(GetParseError_En(dom.GetParseError())));
    assert(dom.IsObject());

    Value& data = dom[SceneIOJsonKeys::data];
    assert(data.IsArray());

    scene.meshInstances().reserve(scene.meshInstances().size() + data.Size());

    for(auto it = data.Begin(); it != data.End(); ++it){
        Value& meshJS = *it;
        assert(meshJS.IsObject());

        Graphics::ModelMeshInstanced mesh(meshJS[SceneIOJsonKeys::mesh_path].GetString());

        Value& transformationsJS = meshJS[SceneIOJsonKeys::mesh_transformations];
        assert(transformationsJS.IsObject());
        mesh.setTransformations(readTransformationsFromJS(transformationsJS));
        scene.meshInstances().push_back(std::move(mesh));
    }

    try{
        Value& transformationsJS = dom[SceneIOJsonKeys::water][0][SceneIOJsonKeys::mesh_transformations];
        scene.water().setTransformations(readTransformationsFromJS(transformationsJS));
    }
    catch (...) {
        DLOG(INFO) << "No water found";
    }
}

void SceneIOJson::save(const Graphics::Scene &scene, const std::string &outPath) {
    DLOG(INFO) << "Saving scene at " << outPath;

    std::ofstream outFile(outPath);
    if(!outFile.is_open())
        throw std::runtime_error("SceneIOJson::save cannot open the file " + outPath);

    // Dom init config
    Document dom;
    dom.SetObject();
    Document::AllocatorType& allocator = dom.GetAllocator();
    Value rootMeshes(kArrayType);
    Value water(kArrayType);

    for(const auto& meshInstanced : scene.meshInstances())
        writeTransformations(meshInstanced, rootMeshes, allocator);

    writeTransformations(scene.water(), water, allocator);

    dom.AddMember(SceneIOJsonKeys::data, rootMeshes, allocator);
    dom.AddMember(SceneIOJsonKeys::water, water, allocator);

    // Write to file
    StringBuffer strbuf;

    // polymorphism does not work here with Writer(no virtual methods)
    if(_isPrettyOutput){
        PrettyWriter<StringBuffer> writer(strbuf);
        dom.Accept(writer);
    }
    else{
        Writer<StringBuffer> writer(strbuf);
        dom.Accept(writer);
    }

    outFile << strbuf.GetString();

    DLOG(INFO) << "Scene saved ! ";
}


void SceneIOJson::addVec3ToJson(const glm::vec3 &vec, rapidjson::Value &value, rapidjson::Document::AllocatorType &allocator) {
    value.PushBack<float>(vec.x, allocator);
    value.PushBack<float>(vec.y, allocator);
    value.PushBack<float>(vec.z, allocator);
}

void SceneIOJson::addVec4ToJson(const glm::vec4 &vec, rapidjson::Value &value, rapidjson::Document::AllocatorType &allocator) {
    addVec3ToJson(glm::vec3(vec), value, allocator);
    value.PushBack<float>(vec.w, allocator);
}

glm::vec3 SceneIOJson::jsonArrayToVec3(const rapidjson::Value &value) {
    return glm::vec3((float)value[0].GetDouble(), (float)value[1].GetDouble(), (float)value[2].GetDouble());
}

glm::vec4 SceneIOJson::jsonArrayToVec4(const rapidjson::Value &value) {
    return glm::vec4(jsonArrayToVec3(value), (float) value[3].GetDouble());
}

void SceneIOJson::writeTransformations(const Graphics::ModelMeshInstanced &mesh, rapidjson::Value &to, rapidjson::Document::AllocatorType& allocator) {
    Value meshJson(kObjectType);
    std::string mPath = mesh.modelPath();
    meshJson.AddMember(SceneIOJsonKeys::mesh_path, Value(mPath.c_str(), (int)mPath.size(), allocator), allocator);

    Value transformations(kObjectType), positions(kArrayType), rotations(kArrayType), scales(kArrayType);

    for(const auto& transf : mesh.getTransformations()){
        Value pos(kArrayType);
        Value rot(kArrayType);
        Value scale(kArrayType);

        addVec3ToJson(transf.position, pos, allocator);
        addVec4ToJson(transf.rotation, rot, allocator);
        addVec3ToJson(transf.scale, scale, allocator);

        positions.PushBack(Value().SetObject().AddMember(SceneIOJsonKeys::value, pos.Move(), allocator), allocator);
        rotations.PushBack(Value().SetObject().AddMember(SceneIOJsonKeys::value, rot.Move(), allocator), allocator);
        scales.PushBack(Value().SetObject().AddMember(SceneIOJsonKeys::value, scale.Move(), allocator), allocator);
    }

    transformations.AddMember(SceneIOJsonKeys::positions, positions, allocator);
    transformations.AddMember(SceneIOJsonKeys::rotations, rotations, allocator);
    transformations.AddMember(SceneIOJsonKeys::scales, scales, allocator);
    meshJson.AddMember(SceneIOJsonKeys::mesh_transformations, transformations, allocator);
    to.PushBack(meshJson.Move(), allocator);
}


std::vector<Geometry::Transformation> SceneIOJson::readTransformationsFromJS(rapidjson::Value &transformationsJS) {
    int nbTransformations = transformationsJS[SceneIOJsonKeys::positions].Size();
    std::vector<Geometry::Transformation> transformations;
    transformations.reserve((size_t)nbTransformations);

    for(int i=0; i<nbTransformations; ++i){
        Value& pos = transformationsJS[SceneIOJsonKeys::positions][i][SceneIOJsonKeys::value];
        Value& rot = transformationsJS[SceneIOJsonKeys::rotations][i][SceneIOJsonKeys::value];
        Value& scale = transformationsJS[SceneIOJsonKeys::scales][i][SceneIOJsonKeys::value];

        // Arrays structure check
        assert(pos.IsArray() && rot.IsArray() && scale.IsArray());
        assert(pos.Size() == 3  && rot.Size() == 4 && scale.Size() == 3);

        Geometry::Transformation tr(jsonArrayToVec3(pos), jsonArrayToVec4(rot), jsonArrayToVec3(scale));
        transformations.push_back(std::move(tr));
    }
    return transformations;
}
