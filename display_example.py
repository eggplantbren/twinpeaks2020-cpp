import matplotlib.pyplot as plt
import numba
import numpy as np
import yaml

# Compute true log(Z) and H at the temperatures requested
@numba.jit("float64[:](float64[:])")
def canonical_true(T):
    x = np.linspace(0.0, 1.0, 10001)
    y = np.exp(-0.5*(x-0.5)**2/0.01**2/T[0] - x/0.01/T[1])
    logZ_true = 100*np.log(np.trapz(y, x=x))
    y_normed = y/np.trapz(y, x=x)
    H_true = 100*np.trapz(y_normed*np.log(y_normed + 1E-300), x=x)
    return np.array([logZ_true, H_true])


@numba.jit("float64(float64[:])")
def logsumexp(values):
	biggest = np.max(values)
	x = values - biggest
	result = np.log(np.sum(np.exp(x))) + biggest
	return result



# Load output and run parameters
output = np.loadtxt("output/scalars.csv", delimiter=",")
f = open("output/run_options.yaml")
run_options = yaml.load(f, Loader=yaml.SafeLoader)
f.close()

# Log prior weights
logw = -np.arange(output.shape[0])/run_options["num_particles"]
logw = logw - logsumexp(logw)


@numba.jit("float64[:](float64[:])")
def canonical(T):

    # Un-normalised posterior/canonical weights
    logL = output[:,1]/T[0] + output[:,2]/T[1]
    logW = logw + logL
    logZ = logsumexp(logW)

    # Normalised posterior/canonical weights
    P = np.exp(logW - logZ)
    H = np.sum(P*(logL - logZ))
    return np.array([logZ, H])


@numba.jit(forceobj=True)
def grid():

    # Temperature grid to look at
    T1 = 10.0**np.linspace(-1.0, 3.0, 51)
    T2 = 10.0**np.linspace(-1.0, 3.0, 51)
    T1, T2 = np.meshgrid(T1, T2)

    # Grids for true and estimated logZ and H
    true_logZ = T1.copy()
    est_logZ  = T1.copy()
    true_H = T1.copy()
    est_H  = T1.copy()

    for i in range(T1.shape[0]):
        for j in range(T1.shape[1]):
            t1, t2 = T1[i, j], T2[i, j]

            true = canonical_true(np.array([t1, t2]))
            est  = canonical(np.array([t1, t2]))

            true_logZ[i, j] = true[0]
            est_logZ[i, j]  = est[0]
            true_H[i, j] = true[1]
            est_H[i, j]  = est[1]

        print(i+1)

    return [T1, T2, true_logZ, est_logZ, true_H, est_H]


T1, T2, true_logZ, est_logZ, true_H, est_H = grid()

plt.subplot(2, 3, 1)
plt.imshow(true_logZ, origin="lower")
plt.title("True log(Z)")

plt.subplot(2, 3, 2)
plt.imshow(est_logZ, origin="lower")
plt.title("Estimated log(Z)")

plt.subplot(2, 3, 3)
resid = est_logZ - true_logZ
biggest = np.max(np.abs(resid))
plt.imshow(resid, origin="lower", cmap="coolwarm",
            vmin=-biggest, vmax=biggest)
plt.title("Resid")

plt.subplot(2, 3, 4)
plt.imshow(true_H, origin="lower")
plt.title("True H")

plt.subplot(2, 3, 5)
plt.imshow(est_H, origin="lower")
plt.title("Estimated H")

plt.subplot(2, 3, 6)
resid = est_H - true_H
biggest = np.max(np.abs(resid))
plt.imshow(resid, origin="lower", cmap="coolwarm",
            vmin=-biggest, vmax=biggest)
plt.title("Resid")



plt.show()

