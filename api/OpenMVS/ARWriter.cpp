//
// Created by song.1634 on 1/25/2022.
//

#include "ARWriter.h"

namespace ARCH {
    bool SaveInterface(const MVS::Scene &scene, const std::string fileName,
                       bool compress) {
        MVSA::Scene obj;
        ////////////////////
        /// Export platforms
        obj.platforms.reserve(scene.platforms.GetSize());
        FOREACH(i, scene.platforms) {
            const MVS::Platform &platform = scene.platforms[i];
            MVSA::Scene::Platform plat;
            plat.name = platform.name;
            plat.cameras.reserve(platform.cameras.GetSize());
            FOREACH(j, platform.cameras) {
                const MVS::Platform::Camera &camera = platform.cameras[j];
                MVSA::Scene::Platform::Camera cam;
                cam.K = camera.K;
                cam.R = camera.R;
                cam.C = camera.C;
                plat.cameras.push_back(cam);
            }
            plat.poses.reserve(platform.poses.GetSize());
            FOREACH(j, platform.poses) {
                const MVS::Platform::Pose &pose = platform.poses[j];
                MVSA::Scene::Platform::Pose p;
                p.R = pose.R;
                p.C = pose.C;
                plat.poses.push_back(p);
            }
            obj.platforms.push_back(plat);
        }
        /// Export platforms
        ////////////////////

        /////////////////
        /// Export images
        obj.images.resize(scene.images.GetSize());
        FOREACH(i, scene.images) {
            const MVS::Image &imageData = scene.images[i];
            MVSA::Scene::Image &image = obj.images[i];
            image.name = MAKE_PATH_REL(WORKING_FOLDER_FULL, imageData.name);
            image.poseID = imageData.poseID;
            image.platformID = imageData.platformID;
            image.cameraID = imageData.cameraID;
            image.width = imageData.width;
            image.height = imageData.height;
        }
        /// Export images
        /////////////////

        ///////////////////////
        /// Export point cloud
        obj.vertices.resize(scene.pointcloud.points.GetSize());
        FOREACH(i, scene.pointcloud.points) {
            const MVS::PointCloud::Point &point = scene.pointcloud.points[i];
            const MVS::PointCloud::ViewArr &views = scene.pointcloud.pointViews[i];
            MVSA::Scene::Vertex &vertex = obj.vertices[i];
            ASSERT(sizeof(vertex.X.x) == sizeof(point.x));
            vertex.X = point;
            vertex.views.resize(views.GetSize());
            views.ForEach([&](MVS::PointCloud::ViewArr::IDX v) {
                MVSA::Scene::Vertex::View &view = vertex.views[v];
                view.imageID = views[v];
                view.confidence = (scene.pointcloud.pointWeights.IsEmpty()
                                   ? 0.f
                                   : scene.pointcloud.pointWeights[i][v]);
            });
        }

        obj.verticesNormal.resize(scene.pointcloud.normals.GetSize());
        FOREACH(i, scene.pointcloud.normals) {
            const MVS::PointCloud::Normal &normal = scene.pointcloud.normals[i];
            MVSA::Scene::Normal &vertexNormal = obj.verticesNormal[i];
            vertexNormal.n = normal;
        }

        obj.verticesColor.resize(scene.pointcloud.colors.GetSize());
        FOREACH(i, scene.pointcloud.colors) {
            const MVS::PointCloud::Color &color = scene.pointcloud.colors[i];
            MVSA::Scene::Color &vertexColor = obj.verticesColor[i];
            vertexColor.c.x = color.r;
            vertexColor.c.y = color.g;
            vertexColor.c.z = color.b;
        }

        /// Export point cloud
        //////////////////////

        //////////////////////
        /// Export Mesh
        int nMeshVertices = scene.mesh.vertices.size();
        int nMeshFaces = scene.mesh.faces.size();
#ifndef OFFICIAL_OPENMVS
        int nMeshTextures = scene.mesh.textureDiffuses.size();
#endif
        int nMeshTexCoords = scene.mesh.faceTexcoords.size();

        obj.mesh.vertices.resize(nMeshVertices);
        obj.mesh.faces.resize(nMeshFaces);

        for (int i = 0; i < nMeshVertices; ++i) {
            const MVS::Mesh::Vertex &point = scene.mesh.vertices[i];
            MVSA::Scene::Mesh::Vertex &vertex = obj.mesh.vertices[i];
            vertex.X = point;
        }

        for (int i = 0; i < nMeshFaces; ++i) {
            const MVS::Mesh::Face &f = scene.mesh.faces[i];
            MVSA::Scene::Mesh::Face &face = obj.mesh.faces[i];
            face.f = f;
        }

        if (!scene.mesh.vertexNormals.empty()) {
            obj.mesh.vertexNormals.resize(nMeshVertices);
            for (int i = 0; i < nMeshVertices; ++i) {
                const MVS::Mesh::Normal &normal = scene.mesh.vertexNormals[i];
                MVSA::Scene::Mesh::Normal &vertexNormal = obj.mesh.vertexNormals[i];
                vertexNormal.n = normal;
            }
        }

        if (!scene.mesh.vertexVertices.empty()) {
            obj.mesh.vertexVertices.resize(nMeshVertices);
            for (int i = 0; i < nMeshVertices; ++i) {
                const MVS::Mesh::VertexIdxArr &vv = scene.mesh.vertexVertices[i];
                MVSA::Scene::Mesh::VertexIdxArr &vertexVertex =
                        obj.mesh.vertexVertices[i];
                vertexVertex.resize(vv.GetSize());
                for (int j = 0, jend = vv.GetSize(); j < jend; ++j) {
                    vertexVertex[j] = vv[j];
                }
            }
        }

        if (!scene.mesh.vertexFaces.empty()) {
            obj.mesh.vertexFaces.resize(nMeshVertices);
            for (int i = 0; i < nMeshVertices; ++i) {
                const MVS::Mesh::FaceIdxArr &vf = scene.mesh.vertexFaces[i];
                MVSA::Scene::Mesh::FaceIdxArr &vertexFaces = obj.mesh.vertexFaces[i];
                vertexFaces.resize(vf.GetSize());
                for (int j = 0, jend = vf.GetSize(); j < jend; ++j) {
                    vertexFaces[j] = vf[j];
                }
            }
        }

        if (!scene.mesh.vertexBoundary.empty()) {
            obj.mesh.vertexBoundary.resize(nMeshVertices);
            for (int i = 0; i < nMeshVertices; ++i) {
                obj.mesh.vertexBoundary[i] = scene.mesh.vertexBoundary[i] ? 255 : 0;
            }
        }

        if (!scene.mesh.faceNormals.empty()) {
            obj.mesh.faceNormals.resize(nMeshFaces);
            for (int i = 0; i < nMeshFaces; ++i) {
                const MVS::Mesh::Normal &normal = scene.mesh.faceNormals[i];
                MVSA::Scene::Mesh::Normal &fNormal = obj.mesh.faceNormals[i];
                fNormal.n = normal;
            }
        }

        if (!scene.mesh.faceTexcoords.empty()) {
            obj.mesh.faceTexcoords.resize(nMeshTexCoords);
            for (int i = 0; i < nMeshTexCoords; ++i) {
                const MVS::Mesh::TexCoord &tc = scene.mesh.faceTexcoords[i];
                MVSA::Scene::Mesh::TexCoord &texCoord = obj.mesh.faceTexcoords[i];
                texCoord.tc = tc;
            }
        }
#ifdef OFFICIAL_OPENMVS
        const SEACAVE::Image8U3 &image = scene.mesh.textureDiffuse;
        obj.mesh.textureDiffuses.resize(1);
        MVSA::Scene::Mesh::Texture &texture = obj.mesh.textureDiffuses[0];
        texture.width = image.width();
        texture.height = image.height();
        size_t datalength = texture.width * texture.height * 3;
        ASSERT(datalength == (image.row_stride() * image.height()));
        texture.data.resize(datalength);
        std::copy(image.data, image.data + datalength, texture.data.data());
#else // OFFICIAL_OPENMVS
        if (!scene.mesh.faceMapIdxs.empty()) {
    obj.mesh.faceMapIdxs.resize(nMeshFaces);
    for (int i = 0; i < nMeshFaces; ++i) {
      obj.mesh.faceMapIdxs[i] = scene.mesh.faceMapIdxs[i];
    }
  }

  if (!scene.mesh.textureDiffuses.empty()) {
    obj.mesh.textureDiffuses.resize(nMeshTextures);
    for (int i = 0; i < nMeshTextures; ++i) {
      const SEACAVE::Image8U3 &image = scene.mesh.textureDiffuses[i];
      MVSA::Scene::Mesh::Texture &texture = obj.mesh.textureDiffuses[i];
      texture.width = image.width();
      texture.height = image.height();
      size_t datalength = texture.width * texture.height * 3;
      ASSERT(datalength == (image.row_stride() * image.height()));
      texture.data.resize(datalength);
      std::copy(image.data, image.data + datalength, texture.data.data());
    }
  }
#endif //OFFICIAL_OPENMVS
        /// Export Mesh
        //////////////////////

        // serialize out the current state
        if (!MVSA::MVAIO::SerializeSave(obj, fileName, compress)) return false;
        return true;
    }

    bool AutoSaveScene(MVS::Scene &scene, const std::string fileName,
                       int compression) {
        const String ext(Util::getFileExt(fileName).ToLower());
        if (ext == _T(".mvs")) {
            if (!scene.Save(fileName, compression==0 ? ARCHIVE_BINARY : ARCHIVE_BINARY_ZIP ))
                return false;
        } else if (ext == _T(".mva")) {
            if (!SaveInterface(scene, fileName, compression)) return false;
        }
        return true;
    }
}  // namespace ARCH