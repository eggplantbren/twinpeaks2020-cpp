import matplotlib.pyplot as plt
import numpy as np
import subprocess
import yaml

particles = np.atleast_2d(np.loadtxt("output/particles.csv", delimiter=","))
N = (particles.shape[1] - 1)//2

subprocess.run("rm output/atoms*", shell=True)
f = open("output/run_options.yaml")
run_options = yaml.load(f, Loader=yaml.SafeLoader)
f.close()

plt.figure(figsize=(8, 8))
for i in range(particles.shape[0]):
    
    sample = particles[i, 1:]
    xs = sample[0:N]
    ys = sample[N:]
    depth = particles[i, 0]/run_options["num_particles"]

    plt.clf()
    plt.plot(xs, ys, "ko", alpha=0.3)
    plt.axis([0, 1, 0, 1])
    plt.title(f"Depth = {depth} nats.")
    fname = "output/atoms" + "%0.6d" % (i+1) + ".png"
    plt.savefig(fname)
    print("Saved {fname}.".format(fname=fname))

