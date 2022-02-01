#include "mvaformat.h"

#include <filesystem/ghc/filesystem.hpp>
#include <cxxopts.hpp>
#include <spdlog/spdlog.h>
#include <tinyply.h>

namespace fs = ghc::filesystem;
using namespace tinyply;

cxxopts::Options parseOptions() {
  cxxopts::Options options("pointextract", "extract point cloud from mva file");
  options.add_options()
      ("i,input", "Input mva file (*.mva).", cxxopts::value<std::string>())
      ("o,output", "Output cloud file (*.ply).", cxxopts::value<std::string>()->default_value(""))
      ("c,color", "Extract colors.", cxxopts::value<bool>()->default_value("true"))
      ("n,normal", "Extract normals.", cxxopts::value<bool>()->default_value("true"));
  options.parse_positional("input");
  options.positional_help("output");
  return options;
}

int main(int argc, char** argv)
{   
    spdlog::set_level(spdlog::level::info);
    auto options = parseOptions();
    auto args = options.parse(argc, argv);
    if(args.count("input")==0)
    {
        spdlog::error("No input specified.");
        spdlog::info(options.help());
        return 0;
    }
    auto input_path = args["input"].as<std::string>();
    auto output_path = args["output"].as<std::string>();
    if(output_path.empty())
    {
        output_path = fs::path(input_path).replace_extension(".ply");
    }
    auto color = args["color"].as<bool>();
    auto normal = args["normal"].as<bool>();
    spdlog::info("Input {0}", input_path);
    spdlog::info("Output {0}", output_path);
    spdlog::info("Color {0} Normal {1}", color, normal);

    std::filebuf fb_binary;
    fb_binary.open(output_path, std::ios::out | std::ios::binary);
    std::ostream outstream_binary(&fb_binary);
    if (outstream_binary.fail())
    {
        spdlog::critical("Cannot open output file: {0}", output_path);
        return 1;
    }

    // Read in 
    std::shared_ptr<MVSA::Scene> scene (new MVSA::Scene);
    int format;
    uint32_t version;
    MVSA::MVAIO::SerializeLoad(*scene, input_path, &format, &version);
    spdlog::debug("mva: Format {0} Version {1}", format, version);
    spdlog::info("Vertices: {0} Colors: {1} Normals: {2}",
                scene->vertices.size(),
                scene->verticesColor.size(),
                scene->verticesNormal.size());
    size_t num_vtx = scene->vertices.size();
    color = color && (num_vtx == scene->verticesColor.size());
    normal = normal && (num_vtx == scene->verticesNormal.size());

    // Output
    std::vector<float> vdata;
    std::vector<uint8_t> vcdata;
    std::vector<float> vndata;
    vdata.reserve(3*num_vtx);
    if(color) vcdata.reserve(3*num_vtx);
    if(normal) vndata.reserve(3*num_vtx);
    for(auto i=0;i<num_vtx;++i)
    {
        vdata.emplace_back(scene->vertices[i].X.x);
        vdata.emplace_back(scene->vertices[i].X.y);
        vdata.emplace_back(scene->vertices[i].X.z);

        if(color)
        {
            vcdata.emplace_back(scene->verticesColor[i].c.x);
            vcdata.emplace_back(scene->verticesColor[i].c.y);
            vcdata.emplace_back(scene->verticesColor[i].c.z);
        }
        if(normal)
        {
            vndata.emplace_back(scene->verticesNormal[i].n.x);
            vndata.emplace_back(scene->verticesNormal[i].n.y);
            vndata.emplace_back(scene->verticesNormal[i].n.z);
        }
    }

    scene.reset();
    
    tinyply::PlyFile plyfile;
    plyfile.add_properties_to_element(
        "vertex", {"x","y","z"}, Type::FLOAT32, num_vtx,
        reinterpret_cast<uint8_t *>(vdata.data()), Type::INVALID,0);
    
    if(color)
        plyfile.add_properties_to_element(
            "vertex", {"red","green","blue"}, Type::UINT8, num_vtx,
            reinterpret_cast<uint8_t *>(vcdata.data()), Type::INVALID,0);

    if(normal)
        plyfile.add_properties_to_element(
            "vertex", {"nx","ny","nz"}, Type::FLOAT32, num_vtx,
            reinterpret_cast<uint8_t *>(vndata.data()), Type::INVALID,0);
    
    plyfile.get_comments().push_back("Extracted from MVA");
    
    plyfile.write(outstream_binary, true);
    fb_binary.close();

    spdlog::info("Done.");
    return 0;
}