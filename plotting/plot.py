import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('/home/hayden/proj/leds/matrix/bin/build/output.txt', header=None, names=['Amplitude'])

df.plot.line()
plt.show()
