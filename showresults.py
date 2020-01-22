import dnest4.classic as dn4
import matplotlib.pyplot as plt
import numpy as np

# For the demo example
# Temperatures
T = (1.0, 1.0)

# Compute true log(Z)
x = np.linspace(0.0, 1.0, 10001)
y = np.exp(-0.5*(x-0.45)**2/0.01**2 - 0.5*(x - 0.55)**2/0.01**2)
logZ_true = 10*np.log(np.trapz(y, x=x))
print("logZ_true =", logZ_true)

# Load output
output = np.loadtxt("output/scalars.csv", delimiter=",")

# Log prior weights
logw = -np.arange(output.shape[0])/1000 # Num particles there...
logw = logw - dn4.logsumexp(logw)

# Compute logZ
logZ = dn4.logsumexp(logw + output[:,1]/T[0] + output[:,2]/T[1])
print("logZ =", logZ)

# Plot dead particles
plt.plot(output[:,1], output[:,2], "k.", alpha=0.2, markersize=1)
plt.xlabel("f_1")
plt.ylabel("f_2")
plt.show()

