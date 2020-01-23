import dnest4.classic as dn4
import matplotlib.pyplot as plt
import numpy as np

particles = np.loadtxt("output/particles.csv", delimiter=",")

for i in range(0, particles.shape[0]):
    x = particles[i, 1:].reshape((32, 32))
    plt.imshow(x, interpolation='nearest')
    plt.title(str(i+1) + '/' + str(particles.shape[0]))
    plt.show()

