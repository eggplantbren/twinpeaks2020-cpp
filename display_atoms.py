import matplotlib.pyplot as plt
import numpy as np
import subprocess

particles = np.atleast_2d(np.loadtxt("output/particles.csv", delimiter=","))
N = (particles.shape[1] - 1)//2

subprocess.run("rm output/atoms*", shell=True)

plt.figure(figsize=(8, 8))
for i in range(particles.shape[0]):
    
    sample = particles[i, 1:]
    xs = sample[0:N]
    ys = sample[N:]

    plt.clf()
    plt.plot(xs, ys, "ko")
    plt.axis([0, 1, 0, 1])
    plt.title(str(i+1) + '/' + str(particles.shape[0]))
    fname = "output/atoms" + "%0.6d" % (i+1) + ".png"
    plt.savefig(fname)
    print("Saved {fname}.".format(fname=fname))

