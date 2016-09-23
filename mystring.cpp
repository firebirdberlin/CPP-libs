//		./mystring.cpp
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
#ifndef MYSTRING_CPP
#define MYSTRING_CPP

#include "mystring.hpp"

/** name: mystring::slice
 *	Get a substring out of this mystring. Can handle negative indices, which are
 *	thought to be counted from the right.
 *
 *	Examples using "mystring":
 *	- slice(2,7) -> "string"
 *	- slice(2,-1) -> "string"
 *	- slice(2,-6) -> "mystring", because left and right refering to the
 *		's', which means 'left' isn't left from 'right'.
 *
 * \param left:
 * \param right: Left/right borders (which are included) of the substring that
 *				should be extracted. Negative values are thought to be indices
 *				where counting starts from the end.
 * \return The substring if 'left' is left of 'right'. The type string ensures
 *		   that the result can be assigned to standard string as well as a mystring.
 */
string mystring::slice(int left, int right) const {
	int len = length();

	// negative Werte : angabe der pos von hinten
	// umrechnung auf pos indices
	if (right < 0) right += len;
	if (left < 0) left += len;


	// Grenzen pruefen und auf maxima begrenzen
	if (right > len) right = len;
	if (left < 0) left = 0;

	if (right > left)
		return mystring(substr(left, (right-left)));
	return mystring();
}

/** name: mystring::is_comment()
 *	Tests if this mystring is a comment, e.g. it starts with a certain
 *	charakter or is empty.
 * \param commentchar: A (single) char, which is used for the indication of
 *				comments.
 * \return True if the (stripped) string starts with the commentchar or if
 *				if it is empty, false otherwise.
 */
bool mystring::is_comment(const char commentchar){
	mystring stripped = strip();
	return ( (stripped.size()==0) or (stripped.startswith(commentchar)));
}

/** name: mystring::isdigit()
 * (Stefan Fruhner - 17.10.2011 16:54:16 CEST)
 * Checks if the string contains only digits.
 * @return Boolean
 **/
bool mystring::isdigit() const{
	for ( string::const_iterator it = begin() ; it != end(); it++ ){
		if (not std::isdigit(*it)) return false;
	}
	return true;
}

/** name: mystring::isalpha()
 * (Stefan Fruhner - 17.10.2011 16:54:16 CEST)
 * Checks if the string contains only chars that are uppercase or lowercase letters.
 * @return Boolean
 **/
bool mystring::isalpha() const{
	for ( string::const_iterator it=begin() ; it != end(); it++ ){
		if (not std::isalpha(*it)) return false;
	}
	return true;
}

/** name: mystring::isalnum()
 * (Stefan Fruhner - 17.10.2011 16:54:16 CEST)
 * Checks if the string contains only alphanumerical chars.
 * @return Boolean
 **/
bool mystring::isalnum() const{
	for ( string::const_iterator it=begin() ; it != end(); it++ ){
		if (not std::isalnum(*it)) return false;
	}
	return true;
}

/** name: mystring::replace()
 *	Replace all occurrences of a string 'from' with the string given by 'to'.
 *	Examples using "myfeststring":
 *	- replace(f, t) -> "myteststring"
 *	- replace(my, our) -> "ourteststring"
 *	- replace(teststring, string) -> "mystring"
 *
 * @param from: The string that is to be replaced.
 * @param to  : The replacement string.
 */
mystring mystring::replace(const char * const from, const char * const to){
	size_t pos = 0;
	pos = this->find(from);
	while ( pos != string::npos){
		string::replace(pos, strlen(from), to, strlen(to));
		pos+=strlen(to); // advance to position after last replacement
		pos = this->find(from, pos);
	}
	return *this;
}

/** mystring::fill()
	Fills string to a constant width with the char fillchar from the left
	If the string is too long, then it is shortened to count
	\param count : number of chars of the resulting string
	\param fillchar : char to fill the string with (optional, standard value is ' ')
	\param sign : add a + sign for numeric values
	\return mystring
 **/
mystring mystring::fill(const unsigned int count, const char fillchar, const bool sign){

	//! if the value is numeric and > 0 add '+'
	if ((sign) && (ToDouble() > 0)) *this = '+' + *this;
	// negative values are already represented by a -, no need to handle this case

	if (length() < count){
		for (unsigned int i=0; i<length() - count; i++)
			*this = fillchar + *this;
	}
	else *this = slice(0,count);//! \todo Simply cutting the string my be unexepcted behaviour. Better solution ?
	return *this;
}

/**  mystring::lfill
 *	Alias for fill().
 *	\param count : number of chars of the resulting string
 *	\param fillchar : char to fill the string with (optional, standard value is ' ')
 *	\param sign : add a + sign for numeric values
 *	\return mystring
 **/
mystring mystring::lfill(const unsigned int count, const char fillchar, const bool sign){
	return fill(count, fillchar, sign);
}

/**  mystring::rfill()
 *	Fills string to a constant width with the char fillchar from the right
 *	If the string is too long, then it is shortened to count
 *	\param count : number of chars of the resulting string
 *	\param fillchar : char to fill the string with (optional, standard value is ' ')
 *	\param sign : add a + sign for numeric values
 * 	\return mystring
 **/
mystring mystring::rfill(unsigned int count, char fillchar,  bool sign){
	//! if the value is numeric and > 0 add '+'
	if ((sign) && (ToDouble() > 0)) *this = '+' + *this;

	if (length() < count){
		for (size_t i=0; i<length() - count; i++)
			*this += fillchar;
	}
	else *this = slice(0,count);
	return *this;
}


/*! file_absolute
 *	checks if a file path is absolute (starting with /)
 * \return True if this string starts with '/', false otherwise.
 **/
bool mystring::file_is_absolute() const {
	int slash = rfind('/');
	return (slash == 0);
}

/*! mystring::file_ext
	If a string is given file_ext returns just the extension of the file
	without '.'
	\param max_length : if the extension is longer than max_length then it will be ignored
	\return Extension without '.' if path was valid, else return empty mystring
*/
mystring mystring::file_ext(const int max_length) const {
	size_t dot = rfind('.');
	size_t slash = rfind('/');
	size_t s = size();
	if (slash > s) slash = 0; // if no slash was found
	if (dot   > s) dot = 0; // if no dot was found
	if ( dot > slash ) {//! only if index of last . is larger then index of '/'
		if ( (max_length < 0) or (max_length > 0 and ((s -1 - dot) <= (size_t) max_length)))
			return slice(dot+1, length());
	}

	return mystring("");
}

/*! mystring::file_base
	If a string is given file_base() returns the basename of the path
	/folder/basename.ext.
	\param strip_path: Boolean, if the path shall be stripped (optional,
			standard: false)
	\return basename if path is valid and otherwise an empty mystring
*/
mystring mystring::file_base(const bool strip_path) const {
	int slash = rfind('/');
	int dot = rfind('.');
	if (slash > (int) size()) slash = 0; // if no slash was found
	if (dot   > (int) size()) dot = 0; // if no slash was found

	if (dot > slash) {
		if (strip_path) return slice(slash+1, dot);
		else
		return slice(0,dot);
	}
	else {
		if (strip_path)
			return slice(slash+1, size());
		else
			return *this;
	}
//		return mystring();
}

/*! file_exists
	Check if the file in the path exists.
	\return True if file exists.
*/
bool mystring::file_exists() const {
	FILE *f = fopen(this->c_str(), "r");
	if (f) { fclose(f); return true;}
	return false;
}

/** name: mystring::file_new_ext()
 *	Assuming that this object contains a filename, this method exchanges the
 *	extension no matter what the extension was before.
 * \param new_ext: c-string with the new extension.
 * \return this
 */
mystring mystring::file_new_ext(const char* new_ext){
	if (strchr(new_ext, '.') != new_ext)
		*this = file_base() +'.' + new_ext;
	else
		*this = file_base() + new_ext;
	return *this;
}

/** name: mystring::file_new_ext()
 *	Assuming that this object contains a filename, this method exchanges the
 *	extension no matter what the extension was before.
 * \param new_ext: c-string with the new extension.
 * \return this
 */
mystring mystring::file_new_ext(const char* new_ext) const {
	mystring r(this->c_str());
	if (strchr(new_ext, '.') != new_ext)
		r = file_base() +'.' + new_ext;
	else
		r = file_base() + new_ext;
	return r;
}

/** \fn mystring mystring::file_strip_ext()
 *
 *	This method returns the base of the filename without the file_ext()
 *	(including the dot).
 *
 * \return The string with the extension removed.
 **/
mystring mystring::file_strip_ext() const{
	if (file_ext().empty()) return *this;

	const mystring ext = "."+file_ext();
	return slice(0, -ext.length());
}

/** name: mystring::file_replace_ext()
 * 	Assuming that this object contains a filename, this method exchanges the
 *	extension 'old_ext' by the extension 'new_ext'
 * \param old_ext: the old extension, that shall be replaced
 * \param new_ext: c-string with the new extension.
 * \return this
 */
mystring mystring::file_replace_ext(const char* old_ext, const char* new_ext){

	mystring old(old_ext);
	old.lstrip('.');

	if (file_ext() == old) 			// old extension was present
		return file_new_ext(new_ext);	// set the new extension
	else return *this;					// otherwise do nothing
}

/** name: mystring::file_lower_ext()
 * 	Assuming that this object contains a filename, this method changes the
 * file extension to lowercase .
 * \return this
 */
mystring mystring::file_lower_ext(){
	mystring ext = file_ext();
	if (not ext.empty()){
		ext.lower();		// lower extension
		*this = file_base();		// strip old extension
		*this += '.' + ext;	// add new one
	}

	return *this;
}


/** name: mystring::file_path()
 *	If a string is given file_path returns the path of a file without its
 *  name : path/name.ext..
 *
 *	\return path. If path is valid and empty mystring if it is not valid
 **/
mystring mystring::file_path() const {
	int slash = rfind('/');
	if (slash > -1) return slice(0, slash);
	return mystring();
}

/*! mystring::path_join
	Checks if the path delimiter is the last char and concatenates the
	path 'itm'
	\param itm: path to be concatenated to the string
	\param path_delimiter: for unix-systems '/' (default value).
	\return New mystring conatining the joined path
*/
mystring mystring::path_join(const mystring itm, const char path_delimiter) const {
	if ( length() == 0 )  return itm;
	else
	if ( endswith(path_delimiter) ) return *this + itm;
	else 							return *this + path_delimiter + itm;
}

/** mystring::path_join()
 * \param itm: string to concatenate.
 * \param path_delimiter: for unix-systems '/' (default value).
 * \return New mystring containing the path.
 **/
mystring mystring::path_join(const string itm, char const path_delimiter) const {
	return path_join(mystring(itm), path_delimiter);
}

/** mystring::path_join()
 * \param itm: string to concatenate.
 * \param path_delimiter: for unix-systems '/' (default value).
 * \return New mystring containing the path.
 **/
mystring mystring::path_join(const char * const itm, char const path_delimiter) const{
	return path_join(mystring(itm), path_delimiter);
}


/** path_strip()
 * If a string is given, path_strip() returns the name of the file without
 * path
 *	\return filename
 **/
mystring mystring::path_strip() const {
	int slash = rfind('/');
	if (slash > -1) return slice(slash+1, INT_MAX);
	return *this;
}

/*! mystring::strip
 * Strips all chars from the beginning and the end of a string
 * that contain certain chars given by c
 * \param c: Array of chars that shall be stripped.
 * \return Reference to the stripped string.
*/
mystring &mystring::strip(const char * const c){
	if (length()==0) return *this; // nothing to strip
	mystring stripchars(c);
	mystring &me = *this;
	size_t cnt = 0;
	int cnl = length()-1;
	// erase at the beginning of the string
	while ((cnt < length()) && (stripchars.contains(me[cnt])))
		cnt++;

	while ((cnl >= 0) && ((stripchars.contains(me[cnl]))))
		cnl--;
	*this = slice(cnt, cnl+1);
	return *this;
}

/*! strip
 * Strips all chars out of ' \\t\\v\\f\\n\\r\\b' from a string.
 * \return Reference to the stripped string.
 */
mystring &mystring::strip(){
	return strip(" \t\v\f\n\r\b");
}


/*! mystring::lstrip
 strips whitespaces,tabs and newlines from the beginning of this mystring
 \return reference to the string
 */
mystring &mystring::lstrip(){
	mystring &me = *this;
	size_t cnt = 0;
	// erase at the beginning of the string
	while ((cnt < length()) && ((me[cnt] == ' ') || (me[cnt] == '\t') || (me[cnt] == '\n')))
		cnt++;

	*this = slice(cnt, length());
	return *this;
}

/*! mystring::lstrip
 Strips a specific character from the beginning of this mystring
 \param sc: The character that should be removed from the beginning of this
 string.
 \return reference to the string
 */
mystring &mystring::lstrip(const char sc){
	lstrip();
	mystring &me = *this;
	size_t cnt = 0;
	// erase at the beginning of the string
	while ((cnt < length()) && (me[cnt] == sc))	cnt++;

	*this = slice(cnt, length());
	return *this;
}

/*! mystring::rstrip
 strips whitespaces,tabs and newlines from the end of a string
 \return reference to the string
 */
mystring &mystring::rstrip(){
	mystring &me = *this;
	int cnl = length()-1;
	// erase at the end of the string
	while ((cnl >= 0) && ((me[cnl] == ' ') || (me[cnl] == '\t') || (me[cnl] == '\n')))
		cnl--;

	*this = slice(0, cnl+1);
	return *this;
}

/*! mystring::rstrip
 Strips a specific character from the end of this mystring
 \todo Needs to be tested.
 \param sc: The character that should be removed from the end of this string.
 \return reference to the string
 */
mystring &mystring::rstrip(const char sc){
	mystring &me = *this;
	int cnl = length()-1;
	// erase at the end of the string
	while ((cnl >= 0) && (me[cnl] == sc))	cnl--;

	*this = slice(0, cnl+1);
	return *this;
}

/** mystring::lower
  * Converts all chars of the string to lowercase
  */
mystring& mystring::lower(){
	for (size_t i = 0; i < this->length(); i++){
		char &c = this->operator[](i);
		this->operator[](i) = tolower(c);
	}
	return *this;
}

/** mystring::upper
 * Converts all chars of the string to uppercase
 */
mystring& mystring::upper(){
	for (size_t i = 0; i < this->length(); i++){
		char &c = this->operator[](i);
		this->operator[](i) = toupper(c);
	}
	return *this;
}

/*! mystring::split()
 *  Splits a string into a list of strings.
 *  \param c : chars that should be used for split (c-string)
 * \return myStringList
*/
myStringList mystring::split(const char * const c ) const {
	mystring splitchars = mystring(c);

	myStringList slist = myStringList();
	if (length()==0) return slist; // nothing to split
	mystring const &me = *this;
	mystring tmp = mystring();

	for (size_t i = 0; i < size(); i++){
		if ( splitchars.contains(me[i]) ) {
				if (tmp.size() > 0) slist.append(tmp);
				tmp.clear();
			}
		else tmp += me[i];
	}
	if (tmp.size() > 0) slist.append(tmp); // append the last part

	return slist;
}

/** mystring::split()
 *  Splits a string using standard delimiters (whitespaces, tabs, line endings ...)
 * \return myStringList
 */
myStringList mystring::split() const{
	return split(" \t\v\f\n");
}

/** mystring::split(char c)
 *  Splits a string using a single char
 * \param c: char which is the delimiter
 * \return myStringList
 */
myStringList mystring::split(const char c) const{
	char chr[2];
	chr[0] = c;
	chr[1] = '\0'; //terminate string
	return split(chr);
}

/*! mystring::splitv()
 *  Splits a string into a list of strings.
 *  \param c : chars that should be used for split (c-string)
 * \return vector<mystring>
*/
vector<mystring> mystring::splitv(const char * const c ) const {

	mystring splitchars = mystring(c);

	vector<mystring> slist(0);
	if (length()==0) return slist; // nothing to split
	mystring const &me = *this;
	mystring tmp = mystring();

	for (size_t i = 0; i < size(); i++){
		if ( splitchars.contains(me[i]) ) {
				if (tmp.size() > 0) slist.push_back(tmp);
				tmp.clear();
			}
		else tmp += me[i];
	}
	if (tmp.size() > 0) slist.push_back(tmp); // append the last part

	return slist;
}

/** mystring::splitv()
 *  Splits a string using standard delimiters (whitespaces, tabs, line endings ...)
 * \return vector<mystring>
 */
vector<mystring> mystring::splitv() const{
	return splitv(" \t\v\f\n");
}

/** mystring::splitv(char c)
 *  Splits a string using a single char
 * \param c: char which is the delimiter
 * \return vector<mystring>
 */
vector<mystring> mystring::splitv(const char c) const{
	char chr[2];
	chr[0] = c;
	chr[1] = '\0'; //terminate string
	return splitv(chr);
}

#endif
