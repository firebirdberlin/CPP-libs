//		./mylib.h
//
//		Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
//
//		This program is free software; you can redistribute it and/or modify
//		it under the terms of the GNU General Public License as published by
//		the Free Software Foundation; either version 2 of the License, or
//		(at your option) any later version.
//
//		This program is distributed in the hope that it will be useful,
//		but WITHOUT ANY WARRANTY; without even the implied warranty of
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//		GNU General Public License for more details.
//
//		You should have received a copy of the GNU General Public License
//		along with this program; if not, write to the Free Software
//		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//		MA 02110-1301, USA.
/**
 * \if MYLIBS
 *		\mainpage
 * \else
 *		\page page_mylibs The library mylibs
 * \endif
\section intro Introduction
\subsection intro Intro
This is a collection of my personal libraries.
All the classes may be completely different in meaning. Changes made in this library
must be made very carefully since I'm using a lot of functions in several projects.


\subsection doc Documentation
 The documentation can be created using doxygen

 \verbatim
 $ doxygen Doxyfile
 \endverbatim

 or simply using

 \verbatim
 $ make doc
 \endverbatim


\subsection compile Compilation and installation
All *.cpp files will be compiled into object code by the makefile.
They will be put together into a library lib/libmylib.a.

 \verbatim
 $  export MYLOCAL=$HOME/local_`uname -m`
 $  ./configure --prefix=$MYLOCAL
 $ make OPT=-O3
 \endverbatim

In order to add debug code to the library replace -O3 by -g.

For simplicity you can install the libraries into a folder of your
* choice, e.g to install the lib to $HOME/local/stow/mylibs run

If your preferred directory is called different you'll have to replace
MYSTOW=$HOME/local/stow by the path of your choice. The installation
procedure needs a working installation of 'stow'.

\verbatim
  $ export PATH=${MYLOCAL}/bin/:${PATH}
  $ make install
\endverbatim
*

Please update your standard search path by editing the environment variable
PATH. If you are using bash, then  add the foollowing line to the
file $HOME/.bashrc :
 \verbatim
	export PATH=$HOME/local/bin:$PATH
 \endverbatim


\subsection usage Usage
For every class (e.g. mystring) you'd like to utilize in your project add the
appropriate *.hpp file to your header.
 \verbatim
 #include <mylibs/mystring.hpp>
 \endverbatim

Programs can include this library using

\verbatim
g++ [your stuff] -I[path/to/include/] -L[path/to/libmylib.a] -lmylib
\endverbatim

or even simpler, if you installed the library as explained above
 \verbatim
 g++ [your stuff] `mylibs-config --ldflags``mylibs-config --cflags`
 \endverbatim

(C 2012) Stefan Fruhner
**/

#include "myexception.hpp"
#include "myinifiles.hpp"
#include "cmdline.hpp"
#include "gipl.h"
#include "lists.h"
#include "lookuptable.hpp"
#include "maps.h"
#include "mylib.h"
#include "myline.hpp"
#include "mymatrix.hpp"
#include "mymesh.hpp"
#include "my_regular_grid.hpp"
#include "mystack.h"
#include "mystring.hpp"
#include "NeighbourSearch.h"
#include "point.hpp"
