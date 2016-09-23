#!/usr/bin/python
# Filename: mymatplotlib.py

version = '0.1'

import math
import numpy as np
import matplotlib
#import pylab as plt

def SI_Prefix(unit, exponent):
	exponent = int(exponent)
	if exponent != 0: # fix the units
		if   exponent ==  24: return "Y" +unit;
		elif exponent ==  21: return "Z" +unit;
		elif exponent ==  18: return "E" +unit;
		elif exponent ==  15: return "P" +unit;
		elif exponent ==  12: return "T" +unit;
		elif exponent ==   9: return "G" +unit;
		elif exponent ==   6: return "M" +unit;
		elif exponent ==   3: return "k" +unit;
		elif exponent ==   2: return "h" +unit;
		elif exponent ==   1: return "da"+unit;
		if   exponent ==  -1: return "m" +unit;
		if   exponent ==  -2: return "m" +unit;
		if   exponent ==  -3: return "m" +unit;
		elif exponent ==  -6: return "u" +unit;
		elif exponent ==  -9: return "n" +unit;
		elif exponent == -12: return "p" +unit;
		elif exponent == -15: return "f" +unit;
		elif exponent == -18: return "a" +unit;
		elif exponent == -21: return "z" +unit;
		elif exponent == -24: return "y" +unit;
		else:
			return "10^(%d) %s" % (int(exponent), unit)
	return unit

def add_axes_at(fig, x,y,w,h, autoscale=False, verbose=False):
	"""Adds an axes instance at a certain coordinate in the plot."""
	subfig = fig.add_subplot(111) # add a subfig that shares the same box
	# first convert data coordinates into [0,1] of the axes
	trans = subfig.transLimits
	Bbox = matplotlib.transforms.Bbox.from_bounds(x, y, w, h)
	x1, y1, w1, h1 = matplotlib.transforms.TransformedBbox(Bbox, trans).bounds

	Bbox = matplotlib.transforms.Bbox.from_bounds(x1, y1, w1, h1)
	trans = subfig.transAxes + fig.transFigure.inverted()
	x1, y1, w1, h1 = matplotlib.transforms.TransformedBbox(Bbox, trans).bounds
	axins = fig.add_axes([x1, y1, w1, h1])

	if verbose:
		print("Transformed into 0,1 coordinates:")
		print("\t",x,"=>", x1);
		print("\t",y,"=>", y1);
		print("\t",w,"=>", w1);
		print("\t",h,"=>", h1);

	return axins

def add_axes_at_Bbox(fig, Bbox, autoscale=False, verbose=False, min_fontsize=3):
	"""Adds an axes instance at a certain coordinate in the plot."""
	subfig = fig.add_subplot(111) # add a subfig that shares the same box
	# first convert data coordinates into [0,1] of the axes
	trans = subfig.transLimits
	x1, y1, w1, h1 = matplotlib.transforms.TransformedBbox(Bbox, trans).bounds

	Bbox1 = matplotlib.transforms.Bbox.from_bounds(x1, y1, w1, h1)
	trans = subfig.transAxes + fig.transFigure.inverted()
	x1, y1, w1, h1 = matplotlib.transforms.TransformedBbox(Bbox1, trans).bounds
	ax = fig.add_axes([x1, y1, w1, h1])
	if verbose:
		x,y,w,h = Bbox.bounds
		print("Transformed into 0,1 coordinates:")
		print("\t",x,"=>", x1);
		print("\t",y,"=>", y1);
		print("\t",w,"=>", w1);
		print("\t",h,"=>", h1);

	if autoscale:
		# w1 and h1 give the percentages in width and height
		# to determine the new font sizes, we use the largest of them
		s = w1
		if h1 > s : s = h1;

		for label in ax.get_xticklabels() + ax.get_yticklabels():
			v = math.ceil(label.get_size() * 1.5 * s);
			if v < min_fontsize : v = min_fontsize # lower bound for font size
			label.set_fontsize(v)

	return ax, (w1,h1)

def add_axes_relative(fig, x,y,w,h, verbose=False):
	"""Adds an axes instance at a certain coordinate in the plot."""
	subfig = fig.add_subplot(111) # add a subfig that shares the same box
	Bbox = matplotlib.transforms.Bbox.from_bounds(x, y, w, h)
	trans = subfig.transAxes + fig.transFigure.inverted()
	x1, y1, w1, h1 = matplotlib.transforms.TransformedBbox(Bbox, trans).bounds
	axins = fig.add_axes([x1, y1, w1, h1])
	if verbose:
		print("Transformed into 0,1 coordinates:")
		print("\t",x,"=>", x1);
		print("\t",y,"=>", y1);
		print("\t",w,"=>", w1);
		print("\t",h,"=>", h1);
	return axins

class myBox(matplotlib.transforms.Bbox):
	#def __init__(self, x,y,w,h):
		#pass
	def __init__(self, points, **kwargs):
		"""
		*points*: a 2x2 numpy array of the form [[x0, y0], [x1, y1]]

		If you need to create a :class:`Bbox` object from another form
		of data, consider the static methods :meth:`unit`,
		:meth:`from_bounds` and :meth:`from_extents`.
		"""
		matplotlib.transforms.Bbox.__init__(self, points, **kwargs)

	def __gt__(self, other):
		# only compare upper right points
		if   (self._points[0][0] > other._points[0][0]): return True;
		elif (self._points[0][1] > other._points[0][1]): return True;
		return False;

	def __lt__(self, other):
		# only compare upper right points
		if   (self._points[0][0] < other._points[0][0]): return True;
		elif (self._points[0][1] < other._points[0][1]): return True;
		return False;

	@staticmethod
	def from_bounds(x0, y0, width, height):
		"""
		(staticmethod) Create a new :class:`Bbox` from *x0*, *y0*,
		*width* and *height*.

		*width* and *height* may be negative.
		"""
		return myBox.from_extents(x0, y0, x0 + width, y0 + height)

	@staticmethod
	def from_extents(*args):
		"""
		(staticmethod) Create a new Bbox from *left*, *bottom*,
		*right* and *top*.

		The *y*-axis increases upwards.
		"""
		points = np.array(args, dtype=np.float_).reshape(2, 2)
		return myBox(points)

	def __repr__(self):
		return 'myBox(%r)' % repr(self._points)

	def expanded(self, sw, sh):
		"""
		Return a new :class:`Bbox` which is this :class:`Bbox`
		expanded around its center by the given factors *sw* and
		*sh*.
		"""
		width = self.width
		height = self.height
		deltaw = (sw * width - width) / 2.0
		deltah = (sh * height - height) / 2.0
		a = np.array([[-deltaw, -deltah], [deltaw, deltah]])
		return myBox(self._points + a)

	def padded(self, p):
		"""
		Return a new :class:`Bbox` that is padded on all four sides by
		the given value.
		"""
		points = self.get_points()
		return myBox(points + [[-p, -p], [p, p]])

	def translated(self, tx, ty):
		"""
		Return a copy of the :class:`Bbox`, statically translated by
		*tx* and *ty*.
		"""
		return myBox(self._points + (tx, ty))

	def get_overlapy(self, Bbox2):
		if self.overlaps(Bbox2):
			p1 = self.get_points();
			p2 = Bbox2.get_points();
			return (p1[1][1] - p2[0][1]);
		return 0.;

	def get_overlapx(self, Bbox2):
		if self.overlaps(Bbox2):
			p1 = self.get_points();
			p2 = Bbox2.get_points();
			return (p1[1][0] - p2[0][0]);
		return 0.;

class myFigure(matplotlib.pyplot.Figure):
	def __init__(self,
				figsize   = None,  # defaults to rc figure.figsize
				dpi       = None,  # defaults to rc figure.dpi
				facecolor = None,  # defaults to rc figure.facecolor
				edgecolor = None,  # defaults to rc figure.edgecolor
				linewidth = 0.0,   # the default linewidth of the frame
				frameon = True,    # whether or not to draw the figure frame
				subplotpars = None, # default to rc
				tight_layout = None, # default to rc figure.autolayout
				):
		Figure.__init__(figsize, dpi, facecolor, edgecolor, linewidth, frameon, subplotpars, tight_layout);

########################################################################
# 3d plots
from mpl_toolkits.mplot3d import Axes3D
#draw a vector
from matplotlib.patches import FancyArrowPatch
from mpl_toolkits.mplot3d import proj3d

class Arrow3D(FancyArrowPatch):
    def __init__(self, xs, ys, zs, *args, **kwargs):
        FancyArrowPatch.__init__(self, (0,0), (0,0), *args, **kwargs)
        self._verts3d = xs, ys, zs

    def draw(self, renderer):
        xs3d, ys3d, zs3d = self._verts3d
        xs, ys, zs = proj3d.proj_transform(xs3d, ys3d, zs3d, renderer.M)
        self.set_positions((xs[0],ys[0]),(xs[1],ys[1]))
        FancyArrowPatch.draw(self, renderer)
########################################################################
if __name__ == "__main__":
	import doctest
	doctest.testmod()
