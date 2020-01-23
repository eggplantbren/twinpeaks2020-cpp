import matplotlib.pyplot as plt
import numpy as np
import subprocess

particles = np.loadtxt("output/particles.csv", delimiter=",")
subprocess.run("rm output/potts*.png", shell=True)

for i in range(particles.shape[0]):
    x = particles[i, 1:].reshape((32, 32))
    plt.clf()
    plt.imshow(x, interpolation='nearest')
    plt.title(str(i+1) + '/' + str(particles.shape[0]))
    fname = "output/potts" + "%0.6d" % (i+1) + ".png"
    plt.savefig(fname)
    print("Saved {fname}.".format(fname=fname))

