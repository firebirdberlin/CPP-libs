//		./Fl_Functions.hpp
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


#include "Fl_Functions.hpp"

using namespace std;

namespace mylibs {

/** name: GetFilename
 *	Fills a string, given as reference, with a filename/path. Therefore a
 *	fltk-file-chooser-window is used.
 * \param filename: Reference to a string where to store the choosen filename. The
 *				string isn't changed if the user hits cancel.
 * \param title: Reference to a string, containing the title the file-chooser-
 *				dialog should have. Standardvalue is NULL, in which case
 *				'Choose path and filename' is used.
 * \param default_name: Reference to a string, containing the default name and
 *				path for the file. Standardvalue is NULL, in which case
 *				'save.dat' is used.
 * \param pattern: A c-string which contains a pattern for delimiting the shown
 *				files. Multiple patterns have to be sepereated by tabs, e.g.
 *				"*.jpg\t*.png\t*.gif\t*".
 * \return True if a filename was choosen. False if cancel was clicked or the
 *				dialog was closed.
 */
bool Fl_ChooseFile(string &filename, const string &title, const string &default_name, const char* pattern) {
	char * ptr_filename = fl_file_chooser(title.c_str(), pattern, default_name.c_str(), 0);

	if(!ptr_filename) return false;

	filename = string(ptr_filename);
	return true;
}

/** name: GetInput
 *	Fills a string, given as reference, with user input. Therefore an fltk-input-
 *	window is utilized. The label of the input-window and a default-value can be
 *	supplied.
 * \param input: Reference to a string, where the input should be stored.
 * \param label: Reference to a string, which contains the label of the input-
 *				window. Default value is "Enter the value:".
 * \param def: 	Pointer to a string, wich contains a value, which should be
 *				used if no input was made. Default value is NULL, which means no
 *				input is handled as if there was the cancel button hit.
 * \return True if an input was valid and the ok-button clicked, false otherwise.
 */
bool Fl_ReadInput(string &input, const mystring &label, const mystring &default_value){
	char const * ptr_input;

	Fl::grab();
	if(not default_value.empty())	ptr_input = fl_input("%s", default_value.c_str(),label.c_str());
	else							ptr_input = fl_input("%s", ""					,label.c_str());

	if(!ptr_input) return false;

	input = string(ptr_input);
	Fl::grab(0);
	return true;
}

/** name: GetPath
 *	Fills a string, given as reference, with a filename/path. Therefore a
 *	fltk-file-chooser-window is used.
 * \param path: Reference to a string where to store the choosen path. The string
 *				isn't changed if the user hits cancel.
 * \param title: Reference to a string, containing the title the file-chooser-
 *				dialog should have. Standardvalue is NULL, in which case
 *				'Choose path' is used.
 * \param default_path: Reference to a string, containing the default path for
 *				the file. Standardvalue is NULL, in which case 'folder.dat' is
 *				used.
 * \return True if a path name was choosen. False if cancel was clicked or the
 *				dialog was closed.
 */
bool Fl_ChoosePath(string &path, const string &title, const string &default_path) {
	char * ptr_path = fl_dir_chooser(title.c_str(), default_path.c_str(), 0);

	if(! ptr_path)	return false;

	path = string(ptr_path);
	return true;
}

}; // end of namespace mylibs
