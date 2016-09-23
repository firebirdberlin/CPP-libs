//		./PNGwrite.h
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
/*
	flounder - copyright 2002 Edward J. Vigmond

	This file is part of flounder.

	flounder is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	flounder is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with flounder; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <png.h>
#include <stdio.h>
#include <iostream>

class PNGwrite{
	public:
		PNGwrite( FILE * );
		~PNGwrite( void );
		int write( void * );
		inline void size( int const w, int const h)	{ width = w; height = h; }
		inline void depth( int const d )			{ colour_depth = d; }
		inline void colour_type( int const c )		{ ctype = c; }
		inline void interlace( int const i )		{ interlace_type = i; }
		inline void palette( png_colorp p )			{ cpalette = p; }

	private:
		png_structp png_ptr;
		png_infop   info_ptr;
		png_colorp  cpalette;
		FILE *fp;
		png_uint_32 width;
		png_uint_32 height;
		int ctype;
		int interlace_type;
		int colour_depth;
};

