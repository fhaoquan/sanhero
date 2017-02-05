import numpy as np
import pylab as pl

a = np.loadtxt('pather_struct.csv')
b = np.loadtxt('pather_path.csv')
print b
print b[0,0]

pl.scatter(a[:,0], a[:,1], linewidths=20)
pl.plot(b[:,0], b[:,1], color='r')
pl.show()
