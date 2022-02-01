#!/usr/bin/env python
import argparse
import sys
import os
sys.path.append(os.path.dirname(__file__))
from os.path import exists
import mvaformat as mva
import numpy as np
import open3d as o3d
import copy

def parseArgs():
    parser = argparse.ArgumentParser(description='visualize')
    parser.add_argument('input', type=str)
    parser.add_argument('--camera', dest='camera', default=False, action='store_true')
    parser.add_argument('--bgr', dest='bgr', default=False, action='store_true')
    return parser.parse_args()

def visualize(scene, **kwargs):
    show_camera = kwargs.get("camera", False)
    flip_rgb = kwargs.get("flip_rgb", False)
    if(show_camera):
        ###
        # Read Camera
        ###
        camjson = []
        num_images = len(scene.images)

        for i in range(num_images):
            img = scene.images[i]
            plat = scene.platforms[img.platformID]
            pose = plat.poses[img.poseID]
            cam = plat.cameras[img.cameraID]
            K = cam.K
            if cam.IsNormalized():
                K[:2, :] *= cam.GetNormalizationScale(img.width, img.height)

            camnode = {
                "C": [pose.C[0], pose.C[1], pose.C[2]],
                "K": [[K[0, 0], K[0, 1], K[0, 2]],
                      [K[1, 0], K[1, 1], K[1, 2]],
                      [K[2, 0], K[2, 1], K[2, 2]]],
                "R": [[pose.R[0, 0], pose.R[0, 1], pose.R[0, 2]],
                      [pose.R[1, 0], pose.R[1, 1], pose.R[1, 2]],
                      [pose.R[2, 0], pose.R[2, 1], pose.R[2, 2]]],
                "height": img.height,
                "width": img.width
            }
            camjson.append(camnode)

    ###
    # Read point cloud
    ###
    num_pts = len(scene.vertices)
    num_color = len(scene.verticesColor)
    num_normal = len(scene.verticesNormal)
    vertices = np.zeros((num_pts, 3), dtype=np.float64)
    verticesColor = np.zeros((num_color, 3), dtype=np.float64)
    verticesNormal = np.zeros((num_normal, 3), dtype=np.float64)

    for i in range(num_pts):
        vertices[i, :] = scene.vertices[i].X
    for i in range(num_color):
        verticesColor[i, :] = np.asarray(scene.verticesColor[i].c, dtype=np.float64) / 255.
    for i in range(num_normal):
        verticesNormal[i, :] = scene.verticesNormal[i].n

    if flip_rgb:
        verticesColor = np.fliplr(verticesColor)

    # Visualization
    pcd = o3d.geometry.PointCloud()
    pcd.points = o3d.utility.Vector3dVector(vertices)
    pcd.colors = o3d.utility.Vector3dVector(verticesColor)
    pcd.normals = o3d.utility.Vector3dVector(verticesNormal)

    if show_camera:
        baseFM = o3d.geometry.TriangleMesh.create_cone(radius = 1.0, height=2, resolution=4, split=1)

        baseFM = baseFM.rotate(o3d.geometry.get_rotation_matrix_from_xyz((np.pi, 0, 0)), np.zeros((3,1)))

        frameList = []
        for c in camjson:
            fm = copy.deepcopy(baseFM).rotate(np.asarray(c['R']).transpose(), np.zeros((3,1))).translate(c['C'])
            frameList.append(fm)
        o3d.visualization.draw_geometries([pcd, *frameList])
    else:
        o3d.visualization.draw_geometries([pcd])

if __name__ == '__main__':
    args = parseArgs()
    if not exists(args.input):
        print(f'Input [{args.input}] not exist.')
        sys.exit(1)
    print(args)
    scene = mva.Scene(args.input)

    visualize(scene, camera=args.camera, flip_rgb = args.bgr)
    scene.info()

