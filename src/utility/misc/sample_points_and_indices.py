import numpy as np

newline = ' '

steps = 100
theta_samples = np.linspace( 1 / steps, 1, steps - 1, endpoint=False)
phi_samples = np.linspace(0, 1, steps + 1)
sample_points = np.array(np.meshgrid(theta_samples, phi_samples)).T.reshape(-1, 2)

with open("/tmp/sample-points", "w") as f:

    np.savetxt(f, sample_points, "%.3f", newline=newline)


indices = []
f = open("/tmp/test", "w")

for row in range(0, steps):
# for row in range(0, 2):

    row_length = steps + 1

    pairwise_indices = [np.linspace(start, start + steps, steps + 1, dtype='int16') for start in map(lambda x: row_length * x, range(row, row+2))]

    for x, y in np.nditer(pairwise_indices):

        indices.append(np.array([x, y], dtype='int16'))

    # indices.append(np.array([pairwise_indices[0][0], pairwise_indices[1][0]], dtype='int16'))

# Python list to numpy array
indices = np.vstack((index for index in indices))

with open("/tmp/indices", "w") as f:

    np.savetxt(f, indices, "%d", newline=newline)

for index in indices.reshape(-1):

    # print("{} -> {}".format(index, sample_points[index]))
    pass
