import matplotlib.pyplot as plt
import numba
import numpy as np
import pandas as pd

# Compute true log(Z) and H at the temperatures requested
@numba.jit("float64[:](float64[:])")
def canonical_true(T):
    x = np.linspace(0.0, 1.0, 10001)
    logZ_true, H_true = 0.0, 0.0

    y = np.exp(-0.5*(x-0.5)**2/0.01**2/T[0] - x/0.01/T[1])
    logZ_true += 20*np.log(np.trapz(y, x=x))
    y_normed = y/np.trapz(y, x=x)
    H_true += 20*np.trapz(y_normed*np.log(y_normed + 1E-300), x=x)

    return np.array([logZ_true, H_true])


@numba.jit("float64(float64[:])")
def logsumexp(values):
	biggest = np.max(values)
	x = values - biggest
	result = np.log(np.sum(np.exp(x))) + biggest
	return result



# Load output and run parameters
output = pd.read_csv("output/scalars.csv")

# Log prior weights
logw = output["ln_w_unnormed"].to_numpy()
depth = np.max(logw) - np.min(logw)
logw = logw - logsumexp(logw)

# Scalars
f1 = output["f1"].to_numpy()
f2 = output["f2"].to_numpy()

@numba.jit("float64[:](float64[:])")
def canonical(T):

    # Un-normalised posterior/canonical weights
    logL = f1/T[0] + f2/T[1]
    logW = logw + logL
    logZ = logsumexp(logW)

    # Normalised posterior/canonical weights
    P = np.exp(logW - logZ)
    H = np.sum(P*(logL - logZ))
    ess = np.exp(-np.sum(P*np.log(P + 1E-300)))
    return np.array([logZ, H, ess])


@numba.jit(forceobj=True)
def grid():

    # Temperature grid to look at
    T1 = 10.0**np.linspace(-0.5, 3.0, 51)
    T2 = 10.0**np.linspace(-0.5, 3.0, 51)
    T1, T2 = np.meshgrid(T1, T2)

    # Grids for true and estimated logZ and H
    true_logZ = T1.copy()
    est_logZ  = T1.copy()
    true_H = T1.copy()
    est_H  = T1.copy()
    ess  = T1.copy()

    for i in range(T1.shape[0]):
        for j in range(T1.shape[1]):
            t1, t2 = T1[i, j], T2[i, j]

            true = canonical_true(np.array([t1, t2]))
            est  = canonical(np.array([t1, t2]))

            true_logZ[i, j] = true[0]
            est_logZ[i, j]  = est[0]
            true_H[i, j] = true[1]
            est_H[i, j]  = est[1]
            ess[i, j] = est[2]

        print(i+1)

    return [T1, T2, true_logZ, est_logZ, true_H, est_H, ess]


T1, T2, true_logZ, est_logZ, true_H, est_H, ess = grid()

# Apply some masking
limit = 0.0
if depth < 10.0:
    limit = depth - 5.0
elif depth < 50.0:
    limit = 0.5*depth
elif depth < 100.0:
    limit = 0.7*depth
elif depth < 200.0:
    limit = 0.8*depth
else:
    limit = 0.9*depth
print(f"Setting H limit to {limit} nats.")

mask = (est_H > limit) | (ess < 5.0)

est_logZ = np.ma.masked_where(mask, est_logZ)
est_H = np.ma.masked_where(mask, est_H)

plt.subplot(2, 3, 1)
plt.imshow(true_logZ, origin="lower")
plt.title("True log(Z)")

plt.subplot(2, 3, 2)
plt.imshow(est_logZ, origin="lower")
plt.title("Estimated ln(Z)")

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

