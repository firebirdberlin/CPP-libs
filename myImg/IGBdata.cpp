//		./IGBdata.cpp
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
#include "IGBdata.hpp"
//! \todo free memory of the IGB data object
IGBdata::IGBdata() : f(0),d(0),i(0),s(0),c(0),
					 scale_min_i(0),	scale_max_i(0), scaling_enabled(false),
					 v_frames(0), v_frames_read(0),
					 v_max_frames(100), v_frames_offset(0),
					 header(0),
					 color_min_i(0.),
					 color_max_i(0.){ // constructor
	v_data_read = false;
	v_data_changed = false;

	/** \todo Set the value of  v_max_frames = 1000 dependent of the avaliable ram */
	header= new IGBheader(); // some standard header
	slice_elements = header->x()*header->y()*header->z(); // size of elements per slice
	return;
}

IGBdata::IGBdata(string igbfilename, size_t maxframes, size_t maxbytes) : f(0),d(0),i(0),s(0),c(0),
									   scale_min_i(0), scale_max_i(0), scaling_enabled(false),
									   v_frames(0), v_frames_read(0),
									   v_max_frames(maxframes), v_frames_offset(0),
									   header(0) { // constructor
	v_data_read = false;
	v_data_changed = false;

	this->readFrames(igbfilename,maxframes, maxbytes); // reads in the header, then the data
	return;
}

//! \todo other types than IGB_FLOAT
IGBdata::~IGBdata() { // destructor
	this->clear();
	return;
}

#define IGB_NONE 0 // no datatype

/*! \attention Because a reference is returned we can
	set the value like:
	igbdat.get(i,j) = 99.99;
	\todo some more checking ... ranges and more datatypes
 */
float &IGBdata::get(const size_t frame,	const size_t voxel) {
	if (voxel >= this->getSliceSize())	throw myexception(EXCEPTION_ID+"Voxel index out of range");
	if (frame > this->v_frames) {
		cout << frame << " > " << v_frames << endl;
		throw myexception(EXCEPTION_ID+"Frame number to large");
	}

	if ((frame >= v_frames_read + v_frames_offset) or (frame < v_frames_offset)) {
		read_igb_data(frame - v_max_frames/2 );
	}

	switch (type) {
		case IGB_FLOAT:
			if (this->f == 0) throw myexception(EXCEPTION_ID+"There is no data in the IGBdata container.");
			return f[(frame-v_frames_offset)*slice_elements + voxel];
			break;
		default: // falls der Typ unbekannt ist, dann irgendwas zurueck
			cerr << "Type : " << type << endl;
			throw myexception(EXCEPTION_ID+"Can only deal with floats ... sorry");
			break;
	}
}

float IGBdata::getv(const size_t frame,const size_t voxel) {
	if (voxel >= this->getSliceSize())	throw myexception(EXCEPTION_ID+"Voxel index out of range");
	if (frame > this->v_frames) throw myexception(EXCEPTION_ID+"Frame number to large");

	if ((frame >= v_frames_read + v_frames_offset) or (frame < v_frames_offset)) {
		read_igb_data(frame - v_max_frames/2 );
	}
	switch (type) {
		case IGB_FLOAT:
			if (this->f == 0) throw myexception(EXCEPTION_ID+"There is no data in the IGBdata container.");
			return f[(frame-v_frames_offset)*slice_elements + voxel];
			break;
		case IGB_DOUBLE:
			if (this->d == 0) throw myexception(EXCEPTION_ID+"There is no data in the IGBdata container.");
			return (float)d[(frame-v_frames_offset)*slice_elements + voxel];
			break;
		case IGB_INT:
			if (this->i == 0) throw myexception(EXCEPTION_ID+"There is no data in the IGBdata container.");
			return (float)i[(frame-v_frames_offset)*slice_elements + voxel];
			break;
		case IGB_SHORT:
			if (this->s == 0) throw myexception(EXCEPTION_ID+"There is no data in the IGBdata container.");
			return (float)s[(frame-v_frames_offset)*slice_elements + voxel];
			break;
		case IGB_CHAR:
			if (this->c == 0) throw myexception(EXCEPTION_ID+"There is no data in the IGBdata container.");
			return (float)c[(frame-v_frames_offset)*slice_elements + voxel];
			break;
		default: // falls der Typ unbekannt ist, dann irgendwas zurueck
			cerr << "Type : " << type << endl;
			throw myexception(EXCEPTION_ID+"Can only deal with floats ... sorry");
			break;
	}
}

double &IGBdata::get_double(size_t frame, size_t voxel) {
	if (voxel >= this->getSliceSize())	throw myexception(EXCEPTION_ID+"Voxel index out of range");
	if (this->d == 0) throw myexception(EXCEPTION_ID+"There is no data in the IGBdata container.");
	if (frame > this->v_frames) throw myexception(EXCEPTION_ID+"Frame number to large");

	// desired frame is not stored in memory
	if ((frame >= v_frames_read + v_frames_offset) or (frame < v_frames_offset)) {
		read_igb_data(frame - v_max_frames/2 ); // new frame should be centered in mem
	}

	switch (type) {
		case IGB_DOUBLE:
			return d[(frame-v_frames_offset)*slice_elements + voxel];
			break;
		default: // falls der Typ unbekannt ist, dann irgendwas zurueck
			throw myexception(EXCEPTION_ID+"Can only deal with doubles ... sorry");
			break;
	}
}

int &IGBdata::get_int(size_t frame, size_t voxel) {
	if (voxel >= this->getSliceSize())	throw myexception(EXCEPTION_ID+"Voxel index out of range");
	if (this->i == 0) throw myexception(EXCEPTION_ID+"There is no data in the IGBdata container.");
	if (frame > this->v_frames) throw myexception(EXCEPTION_ID+"Frame number to large");

	if ((frame >= v_frames_read + v_frames_offset) or (frame < v_frames_offset)) {
		read_igb_data(frame - v_max_frames/2 );
	}

	switch (type) {
		case IGB_INT:
			return i[(frame-v_frames_offset)*slice_elements + voxel];
			break;
		default: // falls der Typ unbekannt ist, dann irgendwas zurueck
			throw myexception(EXCEPTION_ID+"Can only deal with ints ... sorry");
			break;
	}
}

short &IGBdata::get_short(size_t frame, size_t voxel) {
	if (voxel >= this->getSliceSize())	throw myexception(EXCEPTION_ID+"Voxel index out of range");
	if (this->s == 0) throw myexception(EXCEPTION_ID+"There is no data in the IGBdata container.");
	if (frame > this->v_frames) throw myexception(EXCEPTION_ID+"Frame number to large");

	if ((frame >= v_frames_read + v_frames_offset) or (frame < v_frames_offset)) {
		read_igb_data(frame - v_max_frames/2 );
	}

	switch (type) {
		case IGB_SHORT:
			return s[(frame-v_frames_offset)*slice_elements + voxel];
			break;
		default: // falls der Typ unbekannt ist, dann irgendwas zurueck
			throw myexception(EXCEPTION_ID+"Can only deal with ints ... sorry");
			break;
	}
}

char &IGBdata::get_char(size_t frame, size_t voxel) {
	if (voxel >= this->getSliceSize())	throw myexception(EXCEPTION_ID+"Voxel index out of range");
	if (this->c == 0) throw myexception(EXCEPTION_ID+"There is no data in the IGBdata container.");
	if (frame > this->v_frames) throw myexception(EXCEPTION_ID+"Frame number to large");

	if ((frame >= v_frames_read + v_frames_offset) or (frame < v_frames_offset)) {
		read_igb_data(frame - v_max_frames/2 );
	}

	switch (type) {
		case IGB_CHAR:
			return c[(frame-v_frames_offset)*slice_elements + voxel];
			break;
		default: // falls der Typ unbekannt ist, dann irgendwas zurueck
			throw myexception(EXCEPTION_ID+"Can only deal with ints ... sorry");
			break;
	}
}

/*! \attention Because a reference is returned we can
	set the value like:
	igbdat(i,j) = 99.99;
 */
float &IGBdata::operator()(size_t frame, size_t voxel) {
	return get(frame, voxel);
}

float& IGBdata::operator()(size_t frame, size_t voxel, float standard_value ) {
	return this->get(frame, voxel);
}

double& IGBdata::operator()(size_t frame, size_t voxel, double standard_value ) {
	return this->get_double(frame, voxel);
}

int& IGBdata::operator()(size_t frame, size_t voxel, int standard_value ) {
	return this->get_int(frame, voxel);
}


 /** name: IGBdata::readFrames()
  * (Stefan Fruhner - 04.04.2012 15:25:43 CEST)
  * Read in data from an igb file.
  * One can limit the amount of data read into the memory either setting a
  * maximum number of frames or an amount of memory that shall be consumed.
  *
  * @param igbfilename : name of the file
  * @param maxframes   : Maximum numbers of frames to be read in.
  * 					 -1 : No limit, read in the complete file
  * 					  0 : number of frames will be varied using maxbytes
  * @param maxbytes    : Maximum number of bytes to be consumed by the data.
  * 					 Only applicable if maxframes == 0.
  * @return
  **/
bool IGBdata::readFrames(string igbfilename, size_t maxframes, size_t maxbytes) {
	cout << " - Reading IGB file " << igbfilename << endl;
	size_t &max = v_max_frames; 	// nr of max frames before clearing
	clear();					// free all memory used before
	max_frames(max);			// restore value
	data_changed(true);  //! indicates that somethings changed in the data

	//! \attention : freeing the data as above is change enough !!
	;
	cout << " - Opening " << igbfilename << endl;

	gzFile f = gzopen(igbfilename.c_str(), "r");
	if (f == NULL) {
		cerr <<" File not found: "<< igbfilename << endl;
		return false;
	}
	cout << " - Reading header of " << igbfilename << "." << endl;
	if (header) {delete header; header = 0;}
	header = new IGBheader(f);
//	header->read();
	header->fileptr(f);// damit der Header weiß wo die Datei ist

	slice_elements = header->x()*header->y()*header->z(); // size of elements per slice
	try {
		switch (maxframes){
			case (size_t)-1 : this->max_frames(header->t()); break;  // no limit
			case  		   0 : this->max_memory(maxbytes ); break; // limit in bytes
			default 		 : this->max_frames(maxframes); break; // limit in frames
		}
	} catch (string e) {
		cerr << e << endl;
		throw;
	}
	//! allocate memory and read the data
	switch (header->type()) {
		case IGB_FLOAT:
		case IGB_DOUBLE:
		case IGB_INT:
		case IGB_SHORT:
		case IGB_CHAR:
			read_igb_data();
			break;
		default:
			throw myexception(EXCEPTION_ID+"Datatype not implemented");
	}

	cout <<  "Read " << header->t() << " frames." << endl;
	filename=igbfilename;	// save the filename of the igb file
	type = header->type();

	return true;
}

/** IGBData::read_igb_data()
 *
 * Reads in igb data from the file. Due to possibly large files this
 * class reads in data only up to a certain maximum size in memory
 * which corresponds to a maximum number of frames which is allowed to
 * be read (v_max_frames).
 *
 * If the offset is given to be negative it will be corrected to 0, so
 * the calling function need not to care about the sign of the result.
 *
 * @param offset: Starting number of the starting frame of the desired
 * 				  data portion.
 **/
void IGBdata::read_igb_data(long offset) {
	IGBheader &h 	= *header; 	// reference to the header
	v_data_read 	= false;   	// temporarily set data_read flag to false
	scaling_enabled = false;	// Reading of data results in change of min and max.
								// So scaling must be re-enabled
	slice_elements 	 = h.x()*h.y()*h.z(); // size of elements per slice
	size_t slicesize = h.data_size()*slice_elements;	// size of each frame in bytes

	// move data in the array to the front of the array
	// here v_frames_offset has still the old value
	long start_at = 0;

	// max # frames need not to be larger than # of all frames
	v_max_frames = (v_max_frames > h.t()) ? h.t() : v_max_frames;
	// correct negative offsets to be 0
	if (offset < 0 ) offset = 0;

	// reset offset only if it is to large
	if (offset > h.t() - v_max_frames) offset =  h.t() - v_max_frames;

	//! \note From now on offset is only positive !

	// copy data within the array if needed
	if ( (v_frames_read != 0) and (offset > v_frames_offset)
	        and offset < v_max_frames + v_frames_offset) {
		long aoff = offset - v_frames_offset;
		switch (h.type()) {
			case IGB_FLOAT :
				for (long i = aoff ; i < v_max_frames; i++) {
					memcpy((this->f+(slice_elements*(i-aoff)) ), (this->f+slice_elements*i), slicesize);
				}
				break;
			case IGB_DOUBLE:
				for (long i = aoff; i < v_max_frames; i++) {
					memcpy((this->d+(slice_elements*(i-aoff)) ), (this->d+slice_elements*i), slicesize);
				}
				break;
			case IGB_INT :
				for (long i = aoff; i < v_max_frames; i++) {
					memcpy((this->i+(slice_elements*(i-aoff)) ), (this->i+slice_elements*i), slicesize);
				}
				break;
			case IGB_SHORT :
				for (long i = aoff; i < v_max_frames; i++) {
					memcpy((this->s+(slice_elements*(i-aoff)) ), (this->s+slice_elements*i), slicesize);
				}
				break;
			case IGB_CHAR :
				for (long i = aoff; i < v_max_frames; i++) {
					memcpy((this->c+(slice_elements*(i-aoff)) ), (this->c+slice_elements*i), slicesize);
				}
				break;
			default: throw myexception(EXCEPTION_ID+"Type not implemented");
		}
		start_at = v_frames_read - (offset - v_frames_offset);
	}

	v_frames_offset = offset;

	gzFile file = (gzFile) h.fileptr();
	if ( !file ) {
		file = gzopen(filename.c_str(), "r");
		if (!file) throw(string("Could not open file !!!"));
		h.fileptr(file);
	}
	size_t last_file_position = gztell(file); 					// get actual file position
	size_t new_position =  1024 +slicesize*(offset+start_at);	// compute new position
	if (last_file_position != new_position ) {
		z_off_t diff = new_position - last_file_position ;
		cout << " --> Seeking by "<< diff/1024./1024. << " Mbytes ." << endl;
		gzseek(file, diff, SEEK_CUR); //! der Header ist 1024 bytes lang !
	//		gzseek(file, new_position, 0); //! der Header ist 1024 bytes lang !
	}

//	gzseek(file, 1024, 0); // go to the beginning of the file (der Header ist 1024 bytes lang !)

	int frames_to_read = h.t();
	if (h.t() > v_max_frames) frames_to_read = v_max_frames; // limit the size of frames to read

	//! allocate memory
	if (v_frames_read == 0)
		switch (h.type()) {
			case IGB_FLOAT :
				this->f = new float[frames_to_read * slice_elements];
				break;
			case IGB_DOUBLE:
				this->d = new double[frames_to_read * slice_elements];
				break;
			case IGB_INT :
				this->i = new int[frames_to_read * slice_elements];
				break;
			case IGB_SHORT :
				this->s = new short[frames_to_read * slice_elements];
				break;
			case IGB_CHAR :
				this->c = new char[frames_to_read * slice_elements];
				break;
			default: throw myexception(EXCEPTION_ID+"Type not implemented");
		}

//	cout << " - Reading " << frames_to_read << "/"
//	     << h.t() <<" frames. ( "<< IGBheader::TypeID2Name(h.type()) <<" )" << endl;

	cout << " - Reading frames [" << offset << " - " << offset +frames_to_read << "] : "
		 << frames_to_read << "/"
	     << h.t() <<" frames. ( "<< IGBheader::TypeID2Name(h.type()) <<" )" << endl;

	//! read the data
	int numread = 10;
	int cnt     = start_at;

	do { // read single frames
		switch (h.type()) {
			case IGB_FLOAT :
				numread = gzread( file, (this->f+slice_elements*cnt), slicesize );
				break;
			case IGB_DOUBLE:
				numread = gzread( file, (this->d+slice_elements*cnt), slicesize );
				break;
			case IGB_INT :
				numread = gzread( file, (this->i+slice_elements*cnt), slicesize );
				break;
			case IGB_SHORT :
				numread = gzread( file, (this->s+slice_elements*cnt), slicesize );
				break;
			case IGB_CHAR :
				numread = gzread( file, (this->c+slice_elements*cnt), slicesize );
				break;
			default: throw myexception(EXCEPTION_ID+"Type not implemented");
		}

		if (numread < slicesize) {
			cout << " (!!) Apparently reached end of file, read only " << numread << " bytes." << endl;
			h.t(v_frames_offset + cnt);	// correct the number of frames read
			cout << "      Number of frames in IGB file is " << h.t() << "." << start_at<<" " << v_frames_offset << cnt << endl;
		}

		// limit the number of updates on the commandline
		int wstep = (frames_to_read/100 == 0) ? 1 : frames_to_read/100;
		// print some updates
		if (cnt % wstep == 0)
			printf("\r\t%3.2lf %%\t", 100.* (double)cnt/frames_to_read); fflush(stdout);
		cnt++;
	} while ( (numread == slicesize ) and cnt < frames_to_read);

	printf("\r\t100.00%%\t\n");

	v_frames = h.t(); // save the we have h.t() frames
	v_frames_read = frames_to_read;

	this->boundaries(); // important to set v_frame_read before and slicesice
	v_data_read = true; // everything's okay, save that
}

/** set_scale()
 * A scaling to all values may be applied to the stored data.
 * Therefore, the values for the minimum and maximum need to be set.
 * \param mi: minimum
 * \param ma: maximum
 **/
void IGBdata::set_scale(float mi, float ma){
	cout << __FILE__<< ":"<<__LINE__ << " " << __FUNCTION__ << endl;
//	cout << "set_scale("<< mi << ", " << ma << ")" << endl;
	scale_min_i = mi;
	scale_max_i = ma;
	scaling_enabled = true;
}

float IGBdata::rescale(float val){
	if (not scaling_enabled) return val;
	return (val - color_min_i)/(color_max_i - color_min_i) * (scale_max_i - scale_min_i) + scale_min_i;
}

/**
 * name: IGBdata::color_min
 *
 * Determine the minimum value in the desired frame.
 *
 * @param frame: index of a frame.
 * @return Maximum value. Results are always casted to float.
 */
float IGBdata::color_min(size_t frame) {
	if (frame > this->v_frames) throw myexception(EXCEPTION_ID+"Frame number to large");

	if ((frame >= v_frames_read + v_frames_offset) or (frame < v_frames_offset)) {
//		throw myexception(EXCEPTION_ID+"Frame is not in memory");
		read_igb_data(frame - v_max_frames/2 );
	}
	float min = 0.;
	int idx = (frame-v_frames_offset)*slice_elements;
	switch (type) {
		case IGB_FLOAT:
			min = (float) f[idx];
			for (int i = 0; i < slice_elements; i++) {
				min = (float) (min < f[idx + i ]) ? min: f[idx + i ];
			}
			break;
		case IGB_DOUBLE:
			min = (float) d[idx];
			for (int i = 0; i < slice_elements; i++) {
				min = (float) (min < d[idx + i ])? min: d[idx + i ];
			}
			break;
		case IGB_INT:
			min = (float) this->i[idx];
			for (int i = 0; i < slice_elements; i++) {
				min = (float) (min < this->i[idx + i ])? min: this->i[idx + i ];
			}
			break;
		case IGB_SHORT:
			min = (float) s[idx];
			for (int i = 0; i < slice_elements; i++) {
				min = (float) (min < s[idx + i ])? min: s[idx + i ];
			}
			break;
		case IGB_CHAR:
			min = (float) c[idx];
			for (int i = 0; i < slice_elements; i++) {
				min = (float) (min < c[idx + i ])? min: c[idx + i ];
			}
			break;
		default: // falls der Typ unbekannt ist, dann irgendwas zurueck
			throw myexception(EXCEPTION_ID+"Datatype not implemented");
			break;
	}

//	cout << "scale("<< scale_min_i <<", " << scale_max_i << ")" << endl;

//	if (scaling_enabled) return rescale(min);

	return min;
}

/**
 * name: IGBdata::color_max
 *
 * Determine the maximum value in the desired frame.
 *
 * @param frame: index of a frame.
 * @return Maximum value. Results are always casted to float.
 */
float IGBdata::color_max(size_t frame) {
	if (frame > this->v_frames) throw myexception(EXCEPTION_ID+"Frame number to large");

	if ((frame >= v_frames_read + v_frames_offset) or (frame < v_frames_offset)) {
//		throw myexception(EXCEPTION_ID+"Frame is not in memory");
		read_igb_data(frame - v_max_frames/2 );
	}
	float max = 0.;
	int idx = (frame-v_frames_offset)*slice_elements;
	switch (type) {
		case IGB_FLOAT:
			max = (float) f[idx];
			for (int i = 0; i < slice_elements; i++) {
				max = (float) (max > f[idx + i ])? max: f[idx + i ];
			}
			break;
		case IGB_DOUBLE:
			max = (float) d[idx];
			for (int i = 0; i < slice_elements; i++) {
				max = (float) (max > d[idx + i ])? max: d[idx + i ];
			}
			break;
		case IGB_INT:
			max = (float) this->i[idx];
			for (int i = 0; i < slice_elements; i++) {
				max = (float) (max > this->i[idx + i ]) ?max: this->i[idx + i ];
			}
			break;
		case IGB_SHORT:
			max = (float) s[idx];
			for (int i = 0; i < slice_elements; i++) {
				max = (float) (max > s[idx + i ])? max: s[idx + i ];
			}
			break;
		case IGB_CHAR:
			max = (float) c[idx];
			for (int i = 0; i < slice_elements; i++) {
				max = (float) (max > c[idx + i ])? max: c[idx + i ];
			}
			break;
		default: // falls der Typ unbekannt ist, dann irgendwas zurueck
			throw myexception(EXCEPTION_ID+"Datatype not implemented");
			break;
	}
//	if (scaling_enabled) return rescale(max);
	return max;
}

/** name: IGBdata::boundaries
 * Compute the maxima and minima in the present data array.
 * \note Only the maxima and minima of all the values in the array are
 * computed. There may be smaller or larger values in the part of the igb
 * file which is not in memory. By checking if it is the first run of the
 * function, we assure that old values of min and max will be restored
 * if they where larger/smaller that the minimum or maximum in the presently
 * loaded data. **/
void IGBdata::boundaries() {
	static bool first_run=true;

	// save old extrema
	double old_color_min = color_min_i;
	double old_color_max = color_max_i;

	//	determine extrema in the data array
	switch (this->header->type()) {
		case IGB_FLOAT:
			color_max_i = (double) this->f[0];
			color_min_i = (double) this->f[0];
			for(int i=0; i< v_frames_read * slice_elements; i++) {
				color_max_i = (this->f[i] > color_max_i) ? this->f[i] : color_max_i;
				color_min_i = (this->f[i] < color_min_i) ? this->f[i] : color_min_i;
			}		break;
		case IGB_DOUBLE:
			color_max_i = (double) this->d[0];
			color_min_i = (double) this->d[0];

			for(int i=0; i< v_frames_read * slice_elements; i++) {
				color_max_i = (this->d[i] > color_max_i) ? this->d[i] : color_max_i;
				color_min_i = (this->d[i] < color_min_i) ? this->d[i] : color_min_i;
			}
			break;
		case IGB_INT:
			color_max_i = (double) this->i[0];
			color_min_i = (double) this->i[0];

			for(int i=0; i<  v_frames_read * slice_elements; i++) {
				color_max_i = (this->i[i] > color_max_i) ? this->i[i] : color_max_i;
				color_min_i = (this->i[i] < color_min_i) ? this->i[i] : color_min_i;
			}
			break;
		case IGB_SHORT:
			color_max_i = (double) this->s[0];
			color_min_i = (double) this->s[0];

			for(int i=0; i< v_frames_read*slice_elements ; i++) {
				color_max_i = (this->s[i] > color_max_i) ? this->s[i] : color_max_i;
				color_min_i = (this->s[i] < color_min_i) ? this->s[i] : color_min_i;
			}
			break;
		case IGB_CHAR:
			color_max_i = (double) this->c[0];
			color_min_i = (double) this->c[0];

			for(int i=0; i< v_frames_read*slice_elements ; i++) {
				color_max_i = (this->c[i] > color_max_i) ? this->c[i] : color_max_i;
				color_min_i = (this->c[i] < color_min_i) ? this->c[i] : color_min_i;
			}
			break;
		default: throw myexception(EXCEPTION_ID+"Datatype not implemented");
	}

	// check against values of the last time this function was called
	if ( not first_run) {
		color_max_i = (color_max_i > old_color_max) ? color_max_i: old_color_max;
		color_min_i = (color_min_i < old_color_min) ? color_min_i: old_color_min;
	} else first_run = false;


//	cout << "color_max " << color_max_i << endl;
//	cout << "color_min " << color_min_i << endl;
}

/** name: IGBdata::clear
 * Frees all the memory
 */
void IGBdata::clear() {
	if (header and header->fileptr() ) {
		gzclose((gzFile) (header->fileptr()));
		header->fileptr((FILE *) NULL);
	}
	delete[] this->f;
	delete[] this->d;
	delete[] this->i;
	delete[] this->s;
	delete[] this->c;
	delete header; 	header = 0; // remove old headers
	header = new IGBheader(); // load standard header
	this->v_data_read = false;
	this->v_data_changed = false;
	this->v_frames = 0;
	this->v_frames_read = 0;
	this->v_max_frames  = 100;
	this->v_frames_offset = 0;

	this->f = 0;
	this->d = 0;
	this->i = 0;
	this->s = 0;
	this->c = 0;
	this->type = IGB_NONE;
}

void IGBdata::info() {
	cout << "\nInfo for "    << filename 		<< "\n\
	 frames           \t:\t" << v_frames 		<<"\n\
	 frames in memory \t:\t" << v_frames_read	<<"\n\
	 max number frames\t:\t" << v_max_frames 	<<"\n\
	 type             \t:\t" << type 			<< "\n\
	                  \t \t" << IGBheader::TypeID2Name(type) << "\n\
	 slice_elements   \t:\t" << slice_elements	<< "\n\
	 data_read        \t:\t" << v_data_read		<< "\n\
	 data_changed     \t:\t" << v_data_changed	<< "\n\
	 color_min        \t:\t" << color_min_i		<< "\n\
	 color_max        \t:\t" << color_max_i 	<< "\n";
}

void* IGBdata::get_data() {
	if (this->v_data_read == false)
		throw myexception(EXCEPTION_ID+"Error: Data was not read");

	switch (type) {
		case IGB_FLOAT : return (void*)f; break;
		case IGB_DOUBLE: return (void*)d; break;
		case IGB_INT   : return (void*)i; break;
		case IGB_SHORT : return (void*)s; break;
		case IGB_CHAR  : return (void*)c; break;
		default: throw myexception(EXCEPTION_ID+"Type not implemented");
	}
}

/** name: IGBdata::data
 * Copys the data from a float array.
 * \attention The user has to set the dimensions of x,y,z and t before
 * the data are set.
 * \param fdata : pointer to an array of float
 * \return void
 **/
void IGBdata::data(float *fdata) {
	IGBheader &h = *header; 			// reference to the header
	type = IGB_FLOAT;
	h.type(IGB_FLOAT);
	slice_elements = h.x()*h.y()*h.z(); // size of elements per slice
	v_frames = h.t();			//! number of frames

	this->f = new float[h.t() * slice_elements];
	memcpy(f, fdata, sizeof(float) * h.t() * slice_elements);
	v_data_read = true;
	v_data_changed = true;
	v_frames_read = v_frames;
	v_frames_offset = 0;

	color_max_i = (double) this->f[0];
	color_min_i = (double) this->f[0];
	for(int i=0; i< h.t()*slice_elements ; i++) {
		color_max_i = (this->f[i] > color_max_i) ? this->f[i] : color_max_i;
		color_min_i = (this->f[i] < color_min_i) ? this->f[i] : color_min_i;
	}
	return;
}

/** name: IGBdata::data
 * Copys the data from a double array.
 * \attention The user has to set the dimensions of x,y,z and t before
 * the data are set.
 * \param ddata : pointer to an array of double
 * \return void
 **/
void IGBdata::data(double *ddata) {
	IGBheader &h = *header; 			// reference to the header
	type = IGB_DOUBLE;
	h.type(IGB_DOUBLE);
	slice_elements = h.x()*h.y()*h.z(); // size of elements per slice
	v_frames = h.t();			//! number of frames

	this->d = new double[h.t() * slice_elements];
	memcpy(d, ddata, sizeof(double) * h.t() * slice_elements);
	v_data_read = true;
	v_data_changed = true;
	v_frames_read = v_frames;
	v_frames_offset = 0;

	color_max_i = this->d[0];
	color_min_i = this->d[0];
	for(int i=0; i< h.t()*slice_elements ; i++) {
		color_max_i = (this->d[i] > color_max_i) ? this->d[i] : color_max_i;
		color_min_i = (this->d[i] < color_min_i) ? this->d[i] : color_min_i;
	}
	return;
}


void IGBdata::data(short *data) {
	IGBheader &h = *header; 			// reference to the header
	type = IGB_SHORT;
	h.type(IGB_SHORT);
	slice_elements = h.x()*h.y()*h.z(); // size of elements per slice
	v_frames = h.t();			//! number of frames

	this->s = new short[h.t() * slice_elements];
	memcpy(s, data, sizeof(short) * h.t() * slice_elements);
	v_data_read = true;
	v_data_changed = true;
	v_frames_read = v_frames;
	v_frames_offset = 0;

	color_max_i = (double) this->s[0];
	color_min_i = (double) this->s[0];
	for(int i=0; i< h.t()*slice_elements ; i++) {
		color_max_i = (this->s[i] > color_max_i) ? this->s[i] : color_max_i;
		color_min_i = (this->s[i] < color_min_i) ? this->s[i] : color_min_i;
	}
	return;
}

void IGBdata::data(char *data) {
	IGBheader &h = *header; 			//! reference to the header
	type = IGB_CHAR;
	h.type(IGB_CHAR);
	slice_elements = h.x()*h.y()*h.z(); //! size of elements per slice
	v_frames = h.t();					//! number of frames

	this->c = new char[h.t() * slice_elements];
	memcpy(c, data, sizeof(char) * h.t() * slice_elements);
	v_data_read = true;
	v_data_changed = true;
	v_frames_read = v_frames;
	v_frames_offset = 0;

	char m1 = c[0];
	char m2 = c[0];
	for(int i=0; i< h.t()*slice_elements ; i++) {
		m1 = (c[i] > m1) ? c[i] : m1;
		m2 = (c[i] < m2) ? c[i] : m2;
	}
	color_max_i = (double) m1;
	color_min_i = (double) m2;
	return;
}

void* IGBdata::get_framedata(size_t frame) {
	if (this->v_data_read == false)
		throw myexception(EXCEPTION_ID+"Error: Data was not read");
	if (frame > v_frames - 1)
		throw myexception(EXCEPTION_ID+"Frame does not exists.");

	if ((frame >= v_frames_read + v_frames_offset) or (frame < v_frames_offset)) {
		read_igb_data(frame - v_max_frames/2 );
	}

	switch (type) {
		case IGB_FLOAT : return &(f[(frame-v_frames_offset)*slice_elements]); break;
		case IGB_DOUBLE: return &(d[(frame-v_frames_offset)*slice_elements]); break;
		case IGB_INT   : return &(i[(frame-v_frames_offset)*slice_elements]); break;
		case IGB_SHORT : return &(s[(frame-v_frames_offset)*slice_elements]); break;
		case IGB_CHAR  : return &(c[(frame-v_frames_offset)*slice_elements]); break;
		default: throw myexception(EXCEPTION_ID+"Type not implemented");
	}
}

void IGBdata::max_frames(size_t maxval) {
	if (not v_data_read) {
		v_max_frames = maxval;
	} else throw myexception(EXCEPTION_ID+"Could not set v_max_frames !  Must be called before data is read.");
}

void IGBdata::max_memory(size_t maxbytes) {
	if (not v_data_read) {
		if (this->header == 0) throw myexception(EXCEPTION_ID+"No header was read");
		if (this->slice_elements == 0) throw myexception(EXCEPTION_ID+"slice_elements is 0 !");

		double slicesize = this->header->data_size() * slice_elements;
		if (maxbytes/slicesize < 2.) v_max_frames = 2;
		else v_max_frames = (size_t) maxbytes/slicesize;
	} else throw myexception(EXCEPTION_ID+"Could not set v_max_frames ! Must be called before data is read.");
}
