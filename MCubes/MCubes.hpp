//		./MCubes/MCubes.hpp
//
//		Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
//		Copyright 2011 Arash Azhand <azhand@itp.tu-berlin.de>
//		Copyright 2011 Rico Buchholz <buchholz@itp.tu-berlin.de>
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

#ifndef _MCUBES_LIBRARY
#define  _MCUBES_LIBRARY

/*! \todo 	Ausführliche Einleitung schreiben, Bilder zur Veranschaulichung.
 */

/**	MCubes.hpp
 *	$Revision: 527 $
 *	$Date: 2011-03-23 09:48:33 +0100 (Wed, 23 Mar 2011) $
 *	$Author: stefan $
 *
 * \if MCUBES
 *		\mainpage
 * \else
 *		\page page_MCubes The MCubes-library
 * \endif
 *
 *	\section sec_mc_overview Overview
 *	 -# \ref sec_mc_definition
 *	 -# \ref sec_mc_compile
 *	 -# \ref subsec_mc_packages
 *	 -# \ref subsec_mc_co
 *	 -# \ref subsec_mc_compile
 *	 -# \ref sec_mc_run
 *	 -# \ref sec_mc_copyright
 *
 *	\section sec_mc_definition MCubes
 *	Realises a marching cubes algorithm.
 *
 *  \section sec_mc_compile Creating the libary.
 *  \subsection subsec_mc_packages Packages needed for Ubuntu 10.10
 *  Using Ubuntu 10.10 you will need to install the following packages:
 *  - <a href="http://wiki.ubuntuusers.de/GCC" target ="new">C++ Compiler</a>:
 *   - <a href="apt://build-essential,libmpfr-dev,libgmp3-dev,libmpfr1ldbl,libgmp3c2">build-essential,libmpfr-dev,libgmp3-dev,libmpfr1ldbl,libgmp3c2</a>
 *
 *  - <a href="http://wiki.ubuntuusers.de/Doxygen" target ="new">Doxygen</a>
 *   - <a href="apt://doxygen,doxygen-gui,doxygen-doc,texlive-latex-base">doxygen,doxygen-gui,doxygen-doc,texlive-latex-base</a>
 *
 *  - <a href="http://wiki.ubuntuusers.de/Graphviz" target ="new">GraphViz</a>
 *   - <a href="apt://graphviz">graphviz</a>
 *
 * - libxft-dev
 *	 - <a href="apt://libxft-dev,xutils-dev">libxft-dev,xutils-dev</a>
 *
 * For the testprogramm you need additionaly these packages:
 * - SDL
 *   - <a href="apt://libsdl1.2-dev">libsdl1.2-dev</a>
 *
 * - GL/glut
 * 	 - <a href="apt://libglut3-dev">libglut3-dev</a>
 * 	 - <a href="apt://libglut3">libglut3</a>
 *
 *  For version control we use <a href="http://wiki.ubuntuusers.de/Subversion" target="new">SVN</a>
 *  - <a href="apt://subversion">subversion</a>
 *
 *  A good programming editor is <a href="http://wiki.ubuntuusers.de/Geany" target="new">Geany</a>:
 *  - <a href="apt://geany geany-plugins-common geany-plugin-extrasel">geany geany-plugins-common geany-plugin-extrasel</a>
 *
 *	\subsection subsec_mc_co Checking out the libary
 *
 *	\verbatim $ svn checkout http://pyran.googlecode.com/svn/CPP-libs/MCubes MCubes-read-only
 *
 *	\subsection subsec_mc_compile Commands
 \verbatim
 $ make libMCubes.a
 $ make install
 $ make doc
 \endverbatim
 *
 * \section sec_mc_run Using the libary
 *	To use the libary your project has to include MCubes.hpp, via this file the
 *	other needed files will be included. Then have to create a Grid object with
 *	the constructor that takes a constant Point for the three pixel dimensions
 *	and three size_t values max_x, max_y and max_z to set up the spatial
 *	dimensions. This is done by:
 *
 \verbatim
	#include <MCubes.hpp>
		...
	Grid mcubes_grid(const Point pixel_dims, size_t max_x, size_t max_y, size_t max_z);
		...
 \endverbatim
 *
 *	Based upon this parameters the constructor sets up a onedimensional array of
 *	cubes, the CubeList. This is done in order to set the cubes just one time
 *	before the algorithm itself starts. Afterwards only the values at each
 *	vertex have to be renewed and not the whole list of cubes at each simulation
 *	time step. Now the algorithm can be evaluated for each time step as follows:
 *
 \verbatim
	mcubes_grid.RunAlgorithm(const double *data, const double iso);
 \endverbatim
 *
 *	Here one has to feed the data set and an iso value into the algorithm. Based
 *	upon the data and the iso value the algorithm will create a list of
 *	triangles that represent the iso concentration planes. These list can then
 *	be accessed via a list<MCubes::Triangle>::cons_iterator which iterates over
 *	the \a TriangleList member of the MCubes::Grid object. For example could the
 *	triangle be drawn by the following code (a proper coordinate system is
 *	assumed):
 *
 \verbatim
	glBegin( GL_TRIANGLES );
	list<MCubes::Triangle>::const_iterator it;
	for (it = mcubes_algorithm->TriangleList.begin(); it != mcubes_algorithm->TriangleList.end(); it++) {
		MCubes::Triangle const &t = *it;
		glNormal3f(t.NormalVector.x, t.NormalVector.y, t.NormalVector.z);
		glColor4f(0., 0., 0.9, 1.0);

		glVertex3f( t.v1.x, t.v1.y, t.v1.z );
		glVertex3f( t.v2.x, t.v2.y, t.v2.z );
		glVertex3f( t.v3.x, t.v3.y, t.v3.z );
	}
 \endverbatim
 *
 * \section sec_mc_copyright Copyright
 *
 * - Arash Azhand <azhand@itp.tu-berlin.de>
 * - Stefan Fruhner <stefan.fruhner@gmail.com>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *	MA 02110-1301, USA.
 */

#include "Grid.hpp"
#include "Mesh.hpp"
#include "Triangle.hpp"

namespace MCubes {

// Please add new code always within the namespace MCubes.

}



#endif
