steps = 100

pole_phi_samples = np.linspace(0, 1, steps + 1)

# north_pole = np.array([0.0, 0.0])
# south_pole = np.array([1.0, 0.0])

north_pole_theta_samples = np.array([ 1 / steps ])
south_pole_theta_samples = np.array([ 1.0 - ( 1 / steps ) ])

north_pole_sample_points = np.array(np.meshgrid(north_pole_theta_samples, pole_phi_samples)).T.reshape(-1, 2)
south_pole_sample_points = np.array(np.meshgrid(south_pole_theta_samples, pole_phi_samples)).T.reshape(-1, 2)

with open("/tmp/south-pole-sample-points", "w") as f:

    np.savetxt(f, south_pole_sample_points, "%.03f", newline=newline)


with open("/tmp/north-pole-sample-points", "w") as f:

    np.savetxt(f, north_pole_sample_points, "%.03f", newline=newline)
