import matplotlib.pyplot as plt
import numpy as np
import numpy.random as rng

N = 9
alpha = 0#0#-0.999999
beta = np.sqrt(1.0 - alpha**2)

reps = 1000000
good = []
for rep in range(reps):

    # Particle stuff
    xs = rng.randn(N)
    ys = alpha*xs + beta*rng.randn(N)
    x_indices = np.argsort(xs)
    y_indices = np.argsort(ys)
    x_ranks = np.empty(N)
    x_ranks[x_indices] = np.arange(0, N)
    y_ranks = np.empty(N)
    y_ranks[y_indices] = np.arange(0, N)

    xr, yr = x_ranks + 0.5, y_ranks + 0.5
    X = N - xr
    Y = N - yr

    # Target scalar at particles
    Q = -(X**2 + Y**2)

    # Kevin's space filling curve for ties
    tot = xr + yr
    S = 0.5*tot*(tot-1) + tot
    S[tot % 2 == 0] += xr[tot % 2 == 0]
    S[tot % 2 != 0] += yr[tot % 2 != 0]
    print(tot)

    # Worst particle
    thresh = min(list(zip(Q, S)))

    # Grid - new particles would land here
    # New particles have xr in {0, 1, ..., N}/N (length N)
    xr_grid, yr_grid = np.meshgrid(np.arange(N+1), np.arange(N+1))
    X_grid = N - xr_grid
    Y_grid = N - yr_grid
    Q_grid = -((X_grid + 1E-3)**2 + Y_grid**2)

    tot = xr_grid + yr_grid
    S_grid = 0.5*tot*(tot-1) + tot
    S_grid[tot % 2 == 0] += xr_grid[tot % 2 == 0]
    S_grid[tot % 2 != 0] += yr_grid[tot % 2 != 0]

#    print(Q, end="\n\n")
#    print(Q_grid[::-1, :])
#    plt.plot(xr, yr, "ys", markersize=10)
#    plt.show()
#    exit()

#    plt.close("all")
#    plt.figure(figsize=(8, 8))
#    plt.plot(xr[Q > thresh], yr[Q > thresh], "ys", markersize=10)
#    plt.plot(xr[Q == thresh], yr[Q == thresh], color="y", marker="X",
#                linestyle="None", markersize=10)


#    if Q_new == thresh:
#        print("D'oh")

#    for i in range(len(xs)):
#        print(xs[i], ys[i], Q[i], S[i], 0)
#    for i in range(100000):
    # Generate a new particle
    x_new = rng.randn()
    y_new = alpha*x_new + beta*rng.randn()
    xr_new = np.sum(xs < x_new)
    yr_new = np.sum(ys < y_new)
    X_new = N - xr_new
    Y_new = N - yr_new
    Q_new = -(X_new**2 + Y_new**2)

    tot = xr_new + yr_new
    S_new = 0.5*tot*(tot-1) + tot
    if tot%2 == 0:
        S_new += xr_new
    else:
        S_new += yr_new
#    if Q_new > thresh:
#            print(x_new, y_new, Q_new, S_new, 1)
#    exit()

    good.append((Q_new, S_new) > thresh)
    if (rep+1)%1000 == 0:
        print(rep+1, np.mean(good), np.std(good)/np.sqrt(rep+1))



