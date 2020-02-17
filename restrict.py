import matplotlib.pyplot as plt
import numpy as np
import numpy.random as rng

N = 5
alpha = -1
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

    # Related to X from classic NS.
    # Existing particles have X in {0.5, 1.5, ..., N-0.5} (length N)
    # Any new particles will have X in {0, 1, ..., N} (length N+1)
    X = N - x_ranks - 0.5
    Y = N - y_ranks - 0.5

    # Target scalar at particles
    Q = N**2/(X*Y + N**2)

    # Worst particle
    worst = np.nonzero(Q == Q.min())[0]
    thresh = Q[worst][0]

    # Grid of Q-values
    Xgrid, Ygrid = np.meshgrid(np.arange(N+1), 1+np.arange(N+1))
    Qgrid = N**2/(Xgrid*Ygrid + N**2)

    plt.figure(figsize=(8, 8))
    plt.plot(X[Q > thresh], Y[Q > thresh], "wo", markersize=10)
    plt.plot(X[Q == thresh], Y[Q == thresh], "yo", markersize=10)
    plt.imshow((Qgrid < thresh).astype("int64"), origin="lower",
               cmap="coolwarm")
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



