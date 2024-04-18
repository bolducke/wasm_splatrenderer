import numpy
from plyfile import PlyData, PlyElement

plydata = PlyData.read("../data/splat/point_cloud_subset.cleaned.ply")
el = PlyElement.describe(plydata["vertex"].data[:], 'vertex')

PlyData([el]).write("../data/splat/point_cloud_subset.ply")