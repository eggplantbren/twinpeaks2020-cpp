import matplotlib.pyplot as plt
import numpy as np

# Plot dead particles
output = np.loadtxt("output/scalars.csv", delimiter=",")

plt.plot(output[:,1], output[:,2], "k.", alpha=0.2, markersize=1)
plt.xlabel("$f_1$")
plt.ylabel("$f_2$")
plt.show()

