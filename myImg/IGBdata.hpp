//		./IGBdata.h
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
 *  IGBDATA.h
 *
 *	to use this class you will need:
 * 	 - IGBheader.h and IGBheader.cpp
 *   - zlib ( -lz )
 */

//#define DEBUG

#ifndef __IGBDATA_H
#define __IGBDATA_H

#include <iostream>
#include <sstream>  // String-Ein-/Ausgabe
#include <zlib.h>
#include <string>
#include <zlib.h>

#include <stdio.h>
#include "IGBheader.hpp"

using namespace std;

class IGBdata {
		float			     *f;			//!< in these objects the data is stored
		double			     *d;			//!< ... double values
		int				     *i;			//!< ... integer values
		short			     *s;			//!< ... short values
		char 			     *c;			//!< ... char values
		int				   type;			//!< according to the IGB_Type defines
		size_t		   v_frames;			//!< # of total frames in the data
		size_t   v_frames_read;			//!< # of frames in memory
		size_t    v_max_frames;			//!< # of maximum frames stored in memory
		size_t v_frames_offset;			//!< actual offset in frames
		size_t	 slice_elements;			//!< length of every slice
		string		   filename;			//!< file the data was read

		bool	 v_data_read;			//!< if the igb data was read
		bool  v_data_changed;			//!< if something was changed in the data
		float color_max_i, color_min_i; //!< maxima
		float scale_min_i, scale_max_i; //!< maxima in real world values
		bool scaling_enabled;

public:
		IGBheader *header;
		IGBdata();
		IGBdata(string igbfilename, size_t maxframes=(size_t)-1, size_t maxbytes=1073741824);
		~IGBdata();

		template <typename Typ>
		void freeMem_if_needed(Typ **ptr, string name);

		float&  get(const size_t frame, const size_t voxel);
		float   getv(const size_t frame, const size_t voxel);
		double& get_double(const size_t frame, const size_t voxel);
		int&    get_int(const size_t frame, const size_t voxel);
		short&  get_short(const size_t frame, const size_t voxel);
		char&   get_char(const size_t frame, const size_t voxel);
		float  &operator()(const size_t frame, const size_t voxel);
		float  &operator()(const size_t frame, const size_t voxel, const float standard_value);
		double &operator()(const size_t frame, const size_t voxel, const double standard_value);
		int    &operator()(const size_t frame, const size_t voxel, const int standard_value);

		bool readFrames(string igbfilename, size_t maxframes= (size_t)-1, size_t maxbytes=1073741824);
		inline size_t frames(				) const {return v_frames;}
		inline size_t frames_read(		) const {return v_frames_read;}
		inline size_t frames_offset(		) const {return v_frames_offset;}
		inline size_t max_frames(			) const {return v_max_frames;}
		inline size_t getSliceSize(		) const {return slice_elements;}
		inline bool data_read(				) const {return v_data_read;};
		inline bool data_changed(			) const {return v_data_changed;};		 //get method for v_data_changed
		inline void data_changed(bool val	)		 {v_data_changed = val;}; //set method for v_data_changed


		float color_min(size_t frame); 			// minimum color value within a given frame
		float color_max(size_t frame); 			// maximum color value within a given frame
		float color_min(){return color_min_i;}	// global min
		float color_max(){return color_max_i;}	// global max

		float get_color_min(){return color_min_i;};
		float get_color_max(){return color_max_i;};
		void  set_color_bounds(float lower, float upper){color_min_i = lower; color_max_i = upper;}

		float rescale(float val);

		//! \todo Think about deleting these methods ...
		void  set_scale(float mi, float ma);
		float scale_min(){ // real world minimum value
			if (scaling_enabled)	return scale_min_i;
			else 					return color_min_i;
			}
		float scale_max(){ // real world maximum value
			if (scaling_enabled)	return scale_max_i;
			else 					return color_max_i;
		}


		void clear();
		void info();
		void* get_data();
		void* data(){return get_data();};
		void data(float *data);
		void data(double *data);
		void data(short *data);
		void data(char *data);
		void* get_framedata(size_t frame);

		// get the time to a given frame#
		double time(size_t frameNr){
			return header->org_t() + frameNr * header->inc_t();
		}

		// get the frame# for a given time
		size_t frame(const double time){
			size_t fr = (size_t) ((time - header->org_t()) / header->inc_t());
			return (fr > v_frames - 1) ? v_frames -1 : fr;
		}

private:
		void boundaries();
		void read_igb_data(long offset=0);
		void max_frames(size_t maxval);
		void max_memory(size_t maxbytes);
};

#endif
