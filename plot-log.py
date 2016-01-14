#!/usr/bin/python

import sys
import matplotlib.pyplot as plt
import numpy as np

# Create array
best = []
worst = []
mean = []
for line in sys.stdin:
    words = line.split(' ');
    best.append(int(words[0]))
    worst.append(int(words[1]))
    mean.append(int(words[2]))

plt.figure(1)
plt.plot(np.log(best), 'b')

#plt.subplot(212)
plt.plot(np.log(worst), 'r')

#plt.subplot(213)
plt.plot(np.log(mean), 'y')

plt.show()
