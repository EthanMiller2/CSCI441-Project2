from matplotlib import pyplot as plt
from matplotlib import style
import numpy as np
import sys

inputFile = sys.argv[1]



style.use('ggplot')

x,y = np.loadtxt(inputFile, unpack = True, delimiter = ',')

plt.plot(x,y)

plt.title('Render Time for Hierarchical Partioning on Cubes')
plt.ylabel('Time(ms)')
plt.xlabel('# Boxes Being Rendered')

plt.show()