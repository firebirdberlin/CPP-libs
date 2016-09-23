# Plot this file using $ gnuplot 'tip.dat'
# Before publication remind to set an appropriate title and axes labels.

set terminal postscript landscape enhanced  color 20 dl 2.5 lw 3.0

# for black and white just erase 'color'
#set terminal postscript landscape enhanced  20 dl 2.5 lw 3.0

set output 'tip.eps'
set xlabel 'time / a.u.'
set ylabel 'distance / a.u.'
set title 'Tip position vs. time'
 
plot 'tip.dat' u 1:2 title 'x' w lines, '' u 1:3 title 'y' w lines
