import matplotlib.pyplot as plt
import numpy as np
import numpy.random as rng

N = 99
alpha = -0.99999
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

    # Existing particles have xx in {0.5, 1.5, ..., N-0.5}/N (length N)
    xr, yr = x_ranks + 0.5, y_ranks + 0.5
    X = 1.0 - xr/N
    Y = 1.0 - yr/N

    # Target scalar at particles
    Q = 1.0/(X*Y + 1.0)

    # Worst particle
    worst = np.nonzero(Q == Q.min())[0]
    thresh = Q[worst][0]

    # Grid - new particles would land here
    # New particles have xr in {0, 1, ..., N}/N (length N)
    xr_grid, yr_grid = np.meshgrid(np.arange(N+1), np.arange(N+1))
    X_grid = 1.0 - xr_grid/N
    Y_grid = 1.0 - yr_grid/N
    Q_grid = 1.0/(X_grid*Y_grid + 1.0)

#    plt.close("all")
#    plt.figure(figsize=(8, 8))
#    plt.plot(xr[Q > thresh], yr[Q > thresh], "ys", markersize=10)
#    plt.plot(xr[Q == thresh], yr[Q == thresh], color="y", marker="X",
#                linestyle="None", markersize=10)

#    plt.plot(xr_grid[Q_grid > thresh].flatten(),
#             yr_grid[Q_grid > thresh].flatten(), color="lightgreen",
#                    linestyle="None", marker="*")
#    plt.plot(xr_grid[Q_grid < thresh].flatten(),
#             yr_grid[Q_grid < thresh].flatten(), color="red",
#                    linestyle="None", marker="*")
#    plt.plot(xr_grid[Q_grid == thresh].flatten(),
#             yr_grid[Q_grid == thresh].flatten(), color="yellow",
#                    linestyle="None", marker="*")
#    plt.show()



    # Generate a new particle
    x_new = rng.randn()
    y_new = alpha*x_new + beta*rng.randn()
    xr_new = np.sum(xs < x_new)
    yr_new = np.sum(ys < y_new)
    X_new = 1.0 - xr_new/N
    Y_new = 1.0 - yr_new/N
    Q_new = 1.0/(X_new*Y_new + 1.0)

    if Q_new == thresh:
        print("D'oh")

    good.append(Q_new > thresh)
    if (rep+1)%100 == 0:
        print(rep+1, np.mean(good), np.std(good)/np.sqrt(rep+1))



