import pandas as pd
import matplotlib.pyplot as plt
import argparse

parser = argparse.ArgumentParser(description='Produce a plot of the data from the file.')
parser.add_argument('--file-path', dest='file_path', required=True, help='Path to the file containing the data to plot.')

args = parser.parse_args()

df = pd.read_csv(args.file_path)
freqs = df['Frequency']
mags = df['Magnitude']
# time = df['Time']
# amplitude = df['Amplitude']

plt.plot(freqs, mags)
# plt.plot(time, amplitude)
plt.show()

# import sys
# import matplotlib.pyplot as plt
# import numpy as np
# import time
 
# import re

# parser = argparse.ArgumentParser(description='Produce a plot of the data from the file.')
# parser.add_argument('--file-path', dest='file_path', required=True, help='Path to the file containing the data to plot.')

# args = parser.parse_args()

# plt.ion() # Stop matplotlib windows from blocking
# plt.ylabel('Amplitude')

# window_size = 44100
# times = np.zeros(44100)
# samples = np.zeros(window_size)

# fig = plt.figure()
# ax = fig.add_subplot(111)
# ax.set_ylim(-1.0, 1.0)
# line1, = ax.plot(times, samples, 'b-')

# lines = open(args.file_path).read().splitlines()
# for line in lines:
#     if re.match('^[a-zA-Z]', line):
#         continue

#     time,amplitude =line.split(',')

#     current_amplitude = float(amplitude)
#     samples[:-1] = samples[1:]
#     samples[-1] = current_amplitude

#     current_time = float(time)
#     times[:-1] = times[1:]
#     times[-1] = current_time

#     line1.set_xdata(times)
#     line1.set_ydata(samples)
#     fig.canvas.draw()
#     fig.canvas.flush_events()


# import matplotlib.pyplot as plt
# import matplotlib.animation as animation
# import argparse

# parser = argparse.ArgumentParser(description='Produce a plot of the data from the file.')
# parser.add_argument('--file-path', dest='file_path', required=True, help='Path to the file containing the data to plot.')

# args = parser.parse_args()

# fig = plt.figure()
# #creating a subplot 
# ax1 = fig.add_subplot(1,1,1)

# def animate(i):
#     data = open(args.file_path, 'r').read()
#     lines = data.split('\n')
#     times = []
#     amplitudes = []
   
#     for line in lines:
#         values = line.split(',')
#         if values[0] == '':
#             continue

#         time = values[0]
#         amplitude = values[1]
#         times.append(int(time))
#         amplitudes.append(float(amplitude))
    
#     ax1.clear()
#     ax1.plot(times, amplitudes)

#     plt.xlabel('Time')
#     plt.ylabel('Amplitude')
#     plt.title('Amplitude of recording over time') 
    
    
# ani = animation.FuncAnimation(fig, animate, interval=1000) 
# plt.show()
