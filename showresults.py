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
y_normed = y/np.trapz(y, x=x)
H_true = 10*np.trapz(y_normed*np.log(y_normed + 1E-300), x=x)
print("logZ_true =", logZ_true)
print("H_true =",    H_true, "nats.", end="\n\n")

# Load output
output = np.loadtxt("output/scalars.csv", delimiter=",")

# Log prior weights
logw = -np.arange(output.shape[0])/1000 # Num particles there...
logw = logw - dn4.logsumexp(logw)

# Un-normalised posterior/canonical weights
logL = output[:,1]/T[0] + output[:,2]/T[1]
logW = logw + logL
logZ = dn4.logsumexp(logW)

# Normalised posterior/canonical weights
P = np.exp(logW - logZ)
H = np.sum(P*(logL - logZ))
print("logZ =", logZ)
print("H =", H, "nats.")

# Plot dead particles
plt.plot(output[:,1], output[:,2], "k.", alpha=0.2, markersize=1)
plt.xlabel("f_1")
plt.ylabel("f_2")
plt.show()

