import matplotlib.pyplot as plt
import numpy as np
import subprocess
import yaml

# Plot dead particles
output = np.loadtxt("output/scalars.csv", delimiter=",")

plt.figure(figsize=(9, 6))
plt.plot(output[:,1], output[:,2], "k.", alpha=0.2, markersize=1)
plt.xlabel("$f_1$")
plt.ylabel("$f_2$")
plt.show()

f = open("output/run_options.yaml")
run_options = yaml.load(f, Loader=yaml.SafeLoader)
f.close()
num_particles = run_options["num_particles"]

subprocess.run("rm output/*.png", shell=True)

plt.figure(figsize=(9, 6))
for i in range(5*output.shape[0]//num_particles):

    points = output[0:(((i+1)*num_particles)//5), :]

    plt.clf()
    plt.plot(points[:-num_particles,1], points[:-num_particles,2], "k.", alpha=0.2, markersize=1)
    plt.plot(points[-num_particles:,1], points[-num_particles:,2], "b.", alpha=0.5, markersize=2)
    plt.xlabel("$f_1$")
    plt.ylabel("$f_2$")

    filename = "output/" + "%0.6d"%(i+1) + ".png"
    plt.savefig(filename)
    print(filename)

subprocess.run("ffmpeg -framerate 20 -i output/%06d.png -c:v libx264 -b:v 1M output/movie.mp4", shell=True)

