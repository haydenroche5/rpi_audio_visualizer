import sys
import matplotlib.pyplot as plt
import numpy as np
import time

plt.ion() # Stop matplotlib windows from blocking
plt.ylabel('Amplitude')

window_size = 500
indices = np.arange(500)
samples = np.zeros(window_size)

fig = plt.figure()
ax = fig.add_subplot(111)
ax.set_ylim(-1.0, 1.0)
line1, = ax.plot(indices, samples, 'b-')

while True:
    current_sample_float = float(sys.stdin.readline().rstrip())
    samples[:-1] = samples[1:]
    samples[-1] = current_sample_float

    line1.set_ydata(samples)
    fig.canvas.draw()
    fig.canvas.flush_events()
