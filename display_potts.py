import matplotlib.pyplot as plt
import numpy as np
import subprocess

particles = np.atleast_2d(np.loadtxt("output/particles.csv", delimiter=","))
subprocess.run("rm output/potts*", shell=True)

for i in range(particles.shape[0]):
    size = int(np.sqrt(particles.shape[1] - 1))
    x = particles[i, 1:].reshape((size, size))
    plt.clf()
    plt.imshow(x, interpolation='nearest')
    plt.title(str(i+1) + '/' + str(particles.shape[0]))
    fname = "output/potts" + "%0.6d" % (i+1) + ".png"
    plt.savefig(fname)
    print("Saved {fname}.".format(fname=fname))

subprocess.run("ffmpeg -r 20 -i output/potts%06d.png " \
                    + "-r 20 -c:v libx264 -b:v 10000k output/potts.mp4",
                shell=True)

