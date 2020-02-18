import matplotlib.pyplot as plt
import numpy as np
import numpy.random as rng

N = 5
alpha = 0
beta = np.sqrt(1.0 - alpha**2)

reps = 1
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

    # Existing particles have xx in {0.5, 1.5, ..., N-0.5} (length N)
    xx = x_ranks + 0.5
    yy = y_ranks + 0.5

    # Target scalar at particles
    qq = N/((N-xx)*(N-yy) + 1)

    # Worst particle
    worst = np.nonzero(qq == qq.min())[0]
    thresh = qq[worst][0]

    # Grid - new particles would land here
    xx_grid, yy_grid = np.meshgrid(np.arange(N+1), np.arange(N+1))
    qq_grid = N/((N - xx_grid)*(N - yy_grid) + 1)

    plt.figure(figsize=(8, 8))
    plt.plot(xx[qq > thresh],  yy[qq > thresh],   "ys", markersize=10)
    plt.plot(xx[qq == thresh], yy[qq == thresh], color="y", marker="X",
                linestyle="None", markersize=10)

    plt.plot(xx_grid[qq_grid > thresh].flatten(),
             yy_grid[qq_grid > thresh].flatten(), color="lightgreen",
                    linestyle="None", marker="*")
    plt.plot(xx_grid[qq_grid < thresh].flatten(),
             yy_grid[qq_grid < thresh].flatten(), "r*")
    plt.plot(xx_grid[qq_grid == thresh].flatten(),
             yy_grid[qq_grid == thresh].flatten(), "y*")


#    plt.plot(Xgrid.flatten(), Ygrid.flatten(), "o", color="gray")
#    plt.imshow((Qgrid > thresh).astype("int64"), origin="lower",
#               cmap="coolwarm_r")
    plt.show()



#    # Generate a new particle
#    xnew = rng.randn()
#    ynew = alpha*xs + beta*rng.randn()
#    Xnew = N - (np.sum(xs < xnew) - 0.5)
#    Ynew = N - (np.sum(ys < ynew) - 0.5)
#    Qnew = 1.0/(Xnew*Ynew)

#    good.append(Qnew >= thresh)
#    if (rep+1)%100 == 0:
#        print(rep+1, np.mean(good), np.std(good)/np.sqrt(rep+1))



