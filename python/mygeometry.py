#!/usr/bin/python
# Filename: mygeometry.py

version = '0.1'

import numpy as np
import matplotlib
#import pylab as plt

def distance(p1, p2):
	pd = p1-p2
	return np.linalg.norm(pd);

def mean_distance(points):
	mean = 0.;
	cnt  = 0;
	for i in range(len(points)):
		for j in range(len(points)):
			if i != j:
				mean += np.linalg.norm(points[i] - points[j])
				cnt += 1
	if cnt == 0 : return 0.;
	mean /= float(cnt)
	return mean

from scipy.spatial import KDTree
def mean_free_length(points):
	"""	Determines the mean free length one can travel without
		hitting another point from the list points.
		points is assumed to be a list of 3-tuple numpy arrays"""

	if len(points) < 2: return 0.

	# search for the nearest neighbours
	data = np.array(points)
	tree = KDTree(data)
	mean = 0.;
	for i in points:
		nearest = tree.query(i, k=2) 	# k=2 : we search for 2 nearest nb.
										# The first result is always the point itself.
		mean += nearest[0][1]
		#print(nearest)
	return mean/len(points);


########################################################################
if __name__ == "__main__":
    import doctest
    doctest.testmod()
