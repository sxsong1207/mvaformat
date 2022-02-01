#!/usr/bin/env python
import argparse
import sys
import os
sys.path.append(os.path.dirname(__file__))
from os.path import exists

import pymvaformat as mva
import numpy as np
import open3d as o3d
from plyfile import PlyData, PlyElement


def parseArgs():
    parser = argparse.ArgumentParser(description='extract point cloud')
    parser.add_argument('input', type=str)
    parser.add_argument('output', nargs='?', default='', type=str)
    parser.add_argument('--color', default=True, type=bool)
    parser.add_argument('--normal', default=True, type=bool)
    parser.add_argument('--vis', action='store_true')
    return parser.parse_args()


if __name__ == '__main__':
    args = parseArgs()
    if args.output == '':
        args.output = args.input + '.ply'
    if not exists(args.input):
        print(f'Input [{args.input}] not exist.')
        sys.exit(1)
    print(args)
    scene = mva.Scene(args.input)

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

    pcd = o3d.geometry.PointCloud()
    pcd.points = o3d.utility.Vector3dVector(vertices)
    if args.color:
        pcd.colors = o3d.utility.Vector3dVector(verticesColor)
    if args.normal:
        pcd.normals = o3d.utility.Vector3dVector(verticesNormal)
    if args.vis:
        o3d.visualization.draw_geometries([pcd])
    # o3d.io.write_point_cloud(args.output, pcd)

    xarr = vertices[:, 0].astype(np.float32)
    yarr = vertices[:, 1].astype(np.float32)
    zarr = vertices[:, 2].astype(np.float32)
    datalist = [xarr, yarr, zarr]
    formatlist = ['f4','f4','f4']
    namelist = ['x','y','z']
    dtypelist = [('x', 'f4'), ('y', 'f4'), ('z', 'f4')]
    if args.color and num_color == num_pts:
        rarr = (255 * verticesColor[:, 0]).astype(np.uint8)
        garr = (255 * verticesColor[:, 1]).astype(np.uint8)
        barr = (255 * verticesColor[:, 2]).astype(np.uint8)
        datalist.extend([rarr, garr, barr])
        dtypelist.extend((['red', 'u1'], ['green', 'u1'], ['blue', 'u1']))
        formatlist.extend(['u1','u1','u1'])
        namelist.extend(['red','green','blue'])
    if args.normal and num_normal == num_pts:
        nxarr = verticesNormal[:, 0].astype(np.float32)
        nyarr = verticesNormal[:, 1].astype(np.float32)
        nzarr = verticesNormal[:, 2].astype(np.float32)
        datalist.extend([nxarr, nyarr, nzarr])
        dtypelist.extend([('nx', 'f4'), ('ny', 'f4'), ('nz', 'f4')])
        formatlist.extend(['f4','f4','f4'])
        namelist.extend(['nx','ny','nz'])

    plyv = np.rec.fromarrays(datalist, formats=formatlist, names=namelist)

    plydata = PlyData(
        [
            PlyElement.describe(
                plyv, 'vertex',
                comments=['vertices']
            ),
            # PlyElement.describe(face, 'face')
        ],
        text=False,
        # byte_order=byte_order,
        comments=['generated from mvaformat']
    )
    plydata.write(args.output)
