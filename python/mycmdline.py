#!/usr/bin/python
# Filename: mycmdline.py

import sys
import os
version = '0.1'

def run(cmd, verbose=False):
	if verbose: print("$ %s " % cmd)
	exec(cmd)


def die(message):
	print("Exit:",message);
	sys.exit(-1);

def tty_size():
	rows, columns = os.popen('stty size', 'r').read().split()
	return (int(rows), int(columns))

def progress(i, sz, columns=60):
	"""prints a progress bar"""
	progress = (i/(sz-1));
	strprogress = "%3.2f%%" % (progress * 100.)
	print('\r' +strprogress + int(progress*(columns-len(strprogress)))*'.', end='');
	sys.stdout.flush();

	if i == sz-1: print();

def human_size(num):
	"""convert bytes to something human readable"""
	for x in ['bytes','KB','MB','GB']:
		if num < 1024.0 and num > -1024.0:
			return "%3.2f%s" % (num, x)
		num /= 1024.0
	return "%3.2f%s" % (num, 'TB')

def line():
	"""constructs a line which is as wide a the terminal"""
	rows, columns = tty_size()
	return columns*'-'

def center(text):
	"""Centers text on the command line"""
	rows, columns = os.popen('stty size', 'r').read().split()
	columns = int(columns)
	columns -= len(text)
	print(int(columns/2)*' '+text+int(columns/2)*' ')

def right(text):
	"""Centers text on the command line"""
	rows, columns = os.popen('stty size', 'r').read().split()
	columns = int(columns)
	columns -= len(text)
	print(int(columns)*' '+text)

def printline():
	"""prints a horizontal line"""
	print(line());

def section(heading):
	printline();
	rows, columns = os.popen('stty size', 'r').read().split()
	columns = int(columns)
	columns -= len(heading)
	print((int(columns/2)-1)*'='+ ' ' +heading+ ' ' + (int(columns/2)-1)*'=')
	#center(heading);
	printline();

def subsection(heading):
	rows, columns = os.popen('stty size', 'r').read().split()
	columns = int(columns)
	columns -= len(heading)
	print((int(columns/2)-1)*'-'+ ' ' +heading+ ' ' + (int(columns/2)-1)*'-')



########################################################################
if __name__ == "__main__":
    import doctest
    doctest.testmod()
