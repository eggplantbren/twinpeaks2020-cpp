import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import subprocess

output = pd.read_csv("output/scalars.csv")

# Log prior weights
logw = output["ln_w_unnormed"].to_numpy()
depth = -logw.copy()
depth[output["sampler_id"] != 0] += np.log(0.5)

# Scalars
f1 = output["f1"].to_numpy()
f2 = output["f2"].to_numpy()

# Plot dead particles
plt.figure(figsize=(9, 9))
plt.plot(f1, f2, "k.", alpha=0.2, markersize=1)
plt.xlabel("$f_1$")
plt.ylabel("$f_2$")
plt.show()

subprocess.run("rm output/*.png", shell=True)
subprocess.run("rm output/movie.mp4", shell=True)

plt.figure(figsize=(9, 9))
num_frames = -int(5*logw.min())
for i in range(num_frames):
    d = 0.2*i
    covered = depth <= d
    recent = covered & (depth >= d-1)

    plt.clf()
    plt.plot(f1[covered], f2[covered], "k.", alpha=0.2, markersize=1)
    plt.plot(f1[recent & (output["sampler_id"] == 0)],
             f2[recent & (output["sampler_id"] == 0)], "bo", alpha=0.5, markersize=5)
    plt.plot(f1[recent & (output["sampler_id"] == 1)],
             f2[recent & (output["sampler_id"] == 1)], "go", alpha=0.5, markersize=5)
    plt.plot(f1[recent & (output["sampler_id"] == 2)],
             f2[recent & (output["sampler_id"] == 2)], "ro", alpha=0.5, markersize=5)
    plt.xlabel("$f_1$")
    plt.ylabel("$f_2$")
    plt.title(f"Depth = {d:.1f} nats.")

    filename = "output/" + "%0.6d"%(i+1) + ".png"
    plt.savefig(filename)
    print("Saved " + filename + f" [{num_frames} total]", end="\r")

subprocess.run("ffmpeg -framerate 20 -i output/%06d.png -c:v libx264 -b:v 1M output/movie.mp4", shell=True)

