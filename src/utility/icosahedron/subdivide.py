import os
import math

import numpy as np

from stl import mesh
from orderedset import OrderedSet

output_directory = 'build/icosahedron'

def output(relative_path):

    return os.path.join(output_directory, relative_path)

def subdivide_icosahedron(icosahedron):

    subdivided_size = 4 * icosahedron.data.shape[0]
    subdivided_icosahedron = mesh.Mesh(np.zeros(subdivided_size, dtype=mesh.Mesh.dtype))
    
    for index, face in enumerate(icosahedron):

        v0 = face[0:3]
        v1 = face[3:6]
        v2 = face[6:9]
        v3 = 0.5 * np.add(v0, v1)
        v4 = 0.5 * np.add(v1, v2)
        v5 = 0.5 * np.add(v2, v0)

        v3 = v3 / np.linalg.norm(v3)
        v4 = v4 / np.linalg.norm(v4)
        v5 = v5 / np.linalg.norm(v5)

        subdivided_icosahedron[4 * index + 0] = np.concatenate((v0, v3, v5), axis=None)
        subdivided_icosahedron[4 * index + 1] = np.concatenate((v3, v1, v4), axis=None)
        subdivided_icosahedron[4 * index + 2] = np.concatenate((v4, v2, v5), axis=None)
        subdivided_icosahedron[4 * index + 3] = np.concatenate((v3, v4, v5), axis=None)

        # 0 3 5
        # 3 1 4
        # 4 2 5
        # 3 4 5

    return subdivided_icosahedron

def iterated_subdivisions(count):

    subdivided_icosahedron = icosahedron

    for i in range(1, count + 1):

        subdivided_icosahedron = subdivide_icosahedron(subdivided_icosahedron)
        subdivided_icosahedron.save(output('subdivided-{}.stl'.format(i)))

def spherical(xyz):
    xy = xyz[0]**2 + xyz[1]**2

    theta = np.arctan2(np.sqrt(xy), xyz[2]) # for elevation angle defined from Z-axis down
    phi = np.arctan2(xyz[1], xyz[0])


    return (theta / math.pi, phi / ( 2.0 * math.pi ) )

def vertices_and_indices():

    icosahedron = mesh.Mesh.from_file(output('subdivided-5.stl'))

    vertices = OrderedSet()
    
    for face in icosahedron:

        v1 = face[0:3]
        v2 = face[3:6]
        v3 = face[6:9]

        vertices.add(tuple(v1))
        vertices.add(tuple(v2))
        vertices.add(tuple(v3))

    with open(output("vertices"), 'w') as vertices_output:

        for vertex in vertices: 

            theta, phi = spherical(vertex)
            vertices_output.write(f"{theta}, {phi},\n")

    with open(output("indices"), 'w') as indices_output:

        for face in icosahedron:

            v1 = vertices.index(tuple(face[0:3]))
            v2 = vertices.index(tuple(face[3:6]))
            v3 = vertices.index(tuple(face[6:9]))

            indices_output.write(f"{v1}, {v2}, {v3},\n")

    return vertices

icosahedron = mesh.Mesh.from_file('src/utility/icosahedron/icosahedron.stl')
iterated_subdivisions(6)
vertices_and_indices()
