import numpy 
import math

def sampler_positions(steps):

    theta = numpy.linspace(0, math.pi, steps)
    phi = numpy.linspace(0, 2 * math.pi, steps)

    # oned_positions = numpy.linspace(0, 1, steps + 1)
    twod_positions = numpy.array(numpy.meshgrid(theta, phi)).transpose().reshape(-1)

    return twod_positions

def element_indices(steps):

    def sum(array, output, i):
        output[2 * i] = array[2 * i] * steps + array[2 * i + 1]
        output[2 * i + 1] = ( array[2 * i] + 1 ) * steps + array[2 * i + 1]

    oned_indices = numpy.linspace(0, steps - 1, steps, dtype=numpy.uint32)
    twod_indices = numpy.array(numpy.meshgrid(oned_indices, oned_indices)).transpose().reshape(-1)

    element_indices = numpy.zeros(twod_indices.shape[0], dtype=numpy.uint32)

    list(sum(twod_indices, element_indices, i) for i in range(0, pow(steps, 2)))

    return element_indices

def generate_input_to_texture_generator(l, m, steps):

   with open("/tmp/test.input", 'w') as f:

       f.write("{} {} {}\n".format(l, m, steps))
       numpy.savetxt(f, sampler_positions(steps).reshape(-1, 2))
