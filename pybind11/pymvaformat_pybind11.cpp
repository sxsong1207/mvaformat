// Copyright 2020 Shaun Song <sxsong1207@qq.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include "pymvaformat_pybind11_scene.h"
#include "pymvaformat_pybind11_types.hpp"

#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>

namespace py = pybind11;
using namespace MVSA;

PYBIND11_MODULE(pymvaformat, m) {
  m.doc() = "MultiView Archive Data compression";

  py::bind_vector<std::vector<bool>>(m, "BoolArr");
  py::bind_vector<std::vector<uint8_t>>(m, "Uint8Arr");
  py::bind_vector<std::vector<uint16_t>>(m, "Uint16Arr");
  py::bind_vector<std::vector<uint32_t>>(m, "Uint32Arr");
  py::bind_vector<std::vector<std::vector<uint32_t>>>(m, "Uint32ArrArr");
  py::bind_vector<std::vector<Scene::Image>>(m, "ImageArr");
  py::bind_vector<std::vector<Scene::Platform>>(m, "PlatformArr");
  py::bind_vector<std::vector<Scene::Platform::Camera>>(m, "CameraArr");
  py::bind_vector<std::vector<Scene::Platform::Pose>>(m, "PoseArr");
  py::bind_vector<std::vector<Scene::Vertex>>(m, "VertexArr");
  py::bind_vector<std::vector<Scene::Vertex::View>>(m, "VerViewArr");
  py::bind_vector<std::vector<Scene::Line>>(m, "LineArr");
  py::bind_vector<std::vector<Scene::Line::View>>(m, "LineViewArr");
  py::bind_vector<std::vector<Scene::Normal>>(m, "NormalArr");
  py::bind_vector<std::vector<Scene::Color>>(m, "ColorArr");
  py::bind_vector<std::vector<Scene::Mesh::Vertex>>(m, "MeshVertexArr");
  py::bind_vector<std::vector<Scene::Mesh::Face>>(m, "MeshFaceArr");
  py::bind_vector<std::vector<Scene::Mesh::Normal>>(m, "MeshNormalArr");
  py::bind_vector<std::vector<Scene::Mesh::TexCoord>>(m, "MeshTexCoordArr");
  py::bind_vector<std::vector<Scene::Mesh::Texture>>(m, "MeshTextureArr");

  auto platform = py::class_<Scene::Platform>(m, "Platform")
      .def(py::init())
      .def_readwrite("name", &Scene::Platform::name)
      .def_readwrite("cameras", &Scene::Platform::cameras)
      .def_readwrite("poses", &Scene::Platform::poses)
      .def("GetK", &Scene::Platform::GetK)
      .def("GetPose", &Scene::Platform::GetPose);

  py::class_<Scene::Platform::Camera>(platform, "Camera")
      .def(py::init())
      .def_readwrite("name", &Scene::Platform::Camera::name)
      .def_readwrite("width", &Scene::Platform::Camera::width)
      .def_readwrite("height", &Scene::Platform::Camera::height)
      .def_readwrite("K", &Scene::Platform::Camera::K)
      .def_readwrite("R", &Scene::Platform::Camera::R)
      .def_readwrite("C", &Scene::Platform::Camera::C)
      .def(py::init())
      .def("HasResolution", &Scene::Platform::Camera::HasResolution)
      .def("IsNormalized", &Scene::Platform::Camera::IsNormalized)
      .def("GetNormalizationScale", py::overload_cast<>(&Scene::Platform::Camera::GetNormalizationScale, py::const_))
      .def_static("ComputeNormalizationScale",py::overload_cast<uint32_t, uint32_t>(&Scene::Platform::Camera::GetNormalizationScale));

  py::class_<Scene::Platform::Pose>(platform, "Pose")
      .def(py::init())
      .def_readwrite("R", &Scene::Platform::Pose::R)
      .def_readwrite("C", &Scene::Platform::Pose::C);

  py::class_<Scene::Image>(m, "Image")
      .def(py::init())
      .def_readwrite("name", &Scene::Image::name)
      .def_readwrite("width", &Scene::Image::width)
      .def_readwrite("height", &Scene::Image::height)
      .def_readwrite("platformID", &Scene::Image::platformID)
      .def_readwrite("cameraID", &Scene::Image::cameraID)
      .def_readwrite("poseID", &Scene::Image::poseID)
      .def_readwrite("ID", &Scene::Image::ID)
      .def("IsValid", &Scene::Image::IsValid)
      .def("__repr__", [](const Scene::Image &a) {
        return "<Image #" + std::to_string(a.ID) + " (" +
               std::to_string(a.width) + "x" + std::to_string(a.height) +
               ")>: " + a.name;
      });

  auto vertex = py::class_<Scene::Vertex>(m, "Vertex")
      .def(py::init())
      .def_readwrite("X", &Scene::Vertex::X)
      .def_readwrite("views", &Scene::Vertex::views);

  py::class_<Scene::Vertex::View>(vertex, "View")
      .def(py::init())
      .def_readwrite("imageID", &Scene::Vertex::View::imageID)
      .def_readwrite("confidence", &Scene::Vertex::View::confidence);

  auto line = py::class_<Scene::Line>(m, "Line")
      .def(py::init())
      .def_readwrite("pt1", &Scene::Line::pt1)
      .def_readwrite("pt2", &Scene::Line::pt2)
      .def_readwrite("views", &Scene::Line::views);

  py::class_<Scene::Line::View>(line, "View")
      .def(py::init())
      .def_readwrite("imageID", &Scene::Line::View::imageID)
      .def_readwrite("confidence", &Scene::Line::View::confidence);

  py::class_<Scene::Normal>(m, "Normal")
      .def(py::init())
      .def_readwrite("n", &Scene::Normal::n);

  py::class_<Scene::Color>(m, "Color")
      .def(py::init())
      .def_readwrite("c", &Scene::Color::c);

  auto mesh = py::class_<Scene::Mesh>(m, "Mesh")
      .def(py::init())
      .def_readwrite("vertices", &Scene::Mesh::vertices)
      .def_readwrite("faces", &Scene::Mesh::faces)
      .def_readwrite("vertexNormals", &Scene::Mesh::vertexNormals)
      .def_readwrite("vertexVertices", &Scene::Mesh::vertexVertices)
      .def_readwrite("vertexFaces", &Scene::Mesh::vertexFaces)
      .def_readwrite("vertexBoundary", &Scene::Mesh::vertexBoundary)
      .def_readwrite("faceNormals", &Scene::Mesh::faceNormals)
      .def_readwrite("faceTexcoords", &Scene::Mesh::faceTexcoords)
      .def_readwrite("faceMapIdxs", &Scene::Mesh::faceMapIdxs)
      .def_readwrite("textureDiffuses", &Scene::Mesh::textureDiffuses);

  py::class_<Scene::Mesh::Vertex>(mesh, "Vertex")
      .def(py::init())
      .def_readwrite("X", &Scene::Mesh::Vertex::X);
  py::class_<Scene::Mesh::Normal>(mesh, "Normal")
      .def(py::init())
      .def_readwrite("n", &Scene::Mesh::Normal::n);
  py::class_<Scene::Mesh::Face>(mesh, "Face")
      .def(py::init())
      .def_readwrite("f", &Scene::Mesh::Face::f);
  py::class_<Scene::Mesh::TexCoord>(mesh, "TexCoord")
      .def(py::init())
      .def_readwrite("tc", &Scene::Mesh::TexCoord::tc);
  py::class_<Scene::Mesh::Texture>(mesh, "Texture")
      .def(py::init())
      .def_readwrite("path", &Scene::Mesh::Texture::path)
      .def_readwrite("width", &Scene::Mesh::Texture::width)
      .def_readwrite("height", &Scene::Mesh::Texture::height)
      .def_readwrite("data", &Scene::Mesh::Texture::data);

  py::class_<Scene>(m, "CScene")
      .def(py::init())
      .def_readwrite("compression", &Scene::compression)
      .def_readwrite("filePath", &Scene::filePath)
      .def_readwrite("platforms", &Scene::platforms)
      .def_readwrite("images", &Scene::images)
      .def_readwrite("vertices", &Scene::vertices)
      .def_readwrite("verticesNormal", &Scene::verticesNormal)
      .def_readwrite("verticesColor", &Scene::verticesColor)
      .def_readwrite("lines", &Scene::lines)
      .def_readwrite("linesNormal", &Scene::linesNormal)
      .def_readwrite("linesColor", &Scene::linesColor)
      .def_readwrite("transform", &Scene::transform)
      .def_readwrite("mesh", &Scene::mesh)
      .def("GetK", &Scene::GetK)
      .def("GetPose", &Scene::GetPose);

  py::enum_<ArchiveFormat>(m, "ArchiveFormat")
      .value("STDIO", ArchiveFormat::STDIO)
      .value("GZSTREAM", ArchiveFormat::GZSTREAM)
      .value("ZSTDSTREAM", ArchiveFormat::ZSTDSTREAM)
      .export_values();

  py::class_<py::pyScene, Scene>(m, "Scene")
      .def(py::init())
      .def(py::init<const std::string &>())
      .def("load", &py::pyScene::load)
      .def("save", &py::pyScene::save, py::arg("filename"),
           py::arg("compression") = ArchiveFormat::STDIO)
      .def("save_gzstream", &py::pyScene::save, py::arg("filename"),
           py::arg("compression") = ArchiveFormat::GZSTREAM)
      .def("save_zstdstream", &py::pyScene::save, py::arg("filename"),
           py::arg("compression") = ArchiveFormat::ZSTDSTREAM)
      .def("info", &py::pyScene::info)
      .def("diagnose", &py::pyScene::diagnose)
      .def("clean_unused_platforms_poses_cameras",
           &py::pyScene::clean_unused_platforms_poses_cameras)
      .def("clean_unused_images", &py::pyScene::clean_unused_images)
      .def("garbage_collect", &py::pyScene::garbage_collect)
      .def("inflate_image_confidence", &py::pyScene::inflate_image_confidence,
           py::arg("scale"))
      .def("append_images", &py::pyScene::append_images, py::arg("other"),
           py::arg("platform_offset"))
      .def("append_vertices_lines", &py::pyScene::append_vertices_lines,
           py::arg("other"), py::arg("image_offset"))
      .def("append_mesh", &py::pyScene::append_mesh, py::arg("other"))
      .def("append", &py::pyScene::append, py::arg("other"));
}