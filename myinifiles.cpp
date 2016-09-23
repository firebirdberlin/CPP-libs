//		./myinifiles.cpp
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


#include "myinifiles.hpp"
#define _LINE_ string(__FILE__) +":"+ toString(__LINE__) + " : "

bool contains(vector<mystring> &list, mystring needle){
	for (vector<mystring>::iterator it = list.begin(); it != list.end(); ++it){
		if (*it == needle ) return true;
	}

	return false;
}

namespace {
	void msg80(const char *msg, ostream &s = std::cout){
		myList<size_t> spaces; // count whitespaces
		for (size_t i = 0; i < strlen(msg); i++)
			if (msg[i] == ' ') spaces.append(i);

		queue<size_t> ins;
		size_t start = 0;
		if (spaces.size() > 0 )
		for (size_t i = 0; i < spaces.size()-1; i++){
			size_t &pos1 = spaces[i];
			size_t &pos2 = spaces[i+1];
			size_t nl = pos1;
			for (size_t j = pos1; j < pos2; j++) if (msg[j] == '\n') nl = j; // find newline in the interval
			if (nl != pos1) start = nl;
			if (pos1 <= start + 80 && pos2 > start + 80){
				start = pos1;	// here we want to add a new line
				ins.push(pos1);
			}
		}

		for (size_t i = 0; i < strlen(msg); i++){
			s << msg[i];
			if (ins.size() > 0 and i == ins.front()){
				s << endl;
				ins.pop();
			}
		}
		s << endl;
	}
}

/** name: myIniFiles::myIniFiles()
 * (Stefan Fruhner - 03.11.2011 11:03:56 CET)
 *
 * Creates an instance of myInifiles. Therefore a default name for the inifile
 * has to be given, that is used if no filename was given via the commandline.
 * Also the number of commandline arguments, the array of the commandline
 * arguments have to be given. If the last parameter is true, additional info
 * messages are printed to the standard output.
 *
 * @param inifile: DEFAULT file name if no file was supplied on the commandline
 * @param argcnt : Number of arguments on the commandline
 * @param args   : Arguments from the commandline
 * @param verbose: True if debug messages shall be printed to stdout (default: false).
 **/
myIniFiles::myIniFiles(const string inifile, const int argcnt, char ** args, const bool verbose)
		: text(stringstream::in | stringstream::out),
		  debug(verbose),
		  checks_done(false),
		  dependency_checking_enabled(true),
		  comment_char('#'),
		  filename(inifile),
		  argc(argcnt), argv(args),
		  params(),
		  infotext("program description"),
		  //current_section(mystring()),
		  sections() {

	register_param("help", "?", "this help", false);
	register_param("Doc", NULL, "[key] Prints out the help for a single key", true);
	register_param("inifile", "f", "inifile to load/save settings", true);
	register_param("free_opts", NULL, "Print all chars that are free to use as options", false);
	register_param("bash_completion", NULL , "Creates a script to enable bash completion for program parameters.", false);
	register_param("initemplate", NULL , "Creates a simple ini template so that the user only has to fill in the values.", false);

	// check if the user supplied a filename
	filename = check_commandline(argcnt, args);

	// If filename invalid then set the default name
	if (filename.strip().empty()) filename = inifile;

	if (debug) {
		cout << " --> Using inifile "<< filename  << endl;
		if (argc == 0 or argv == NULL)
			cout << " -ATTENTION- : No commandline args supplied. I will read only from the file !" << endl;
	}

	// Read the inifile to memory
	fstream ini(filename.c_str(), fstream::in);
	if (ini.is_open()){				// file opened successfully, read the data
		ini.seekg(0, ios::end);		// set get pointer to the end
		size_t size = ini.tellg();	// position = size
		ini.seekg(0, ios::beg);		// set get point to the beginning
		char *buf = new char[size];
		ini.read(buf,size);
		text.write(buf, size);
		ini.close();
		delete[] buf;
	}
	else
	if (debug) { // exception if file could not be opened

		std::cout << "Could not open ini file " << inifile <<
		".\nCreating new one if requested by the program." << endl;
	}
	return;
}

/** \fn myIniFiles::myIniFiles(const string, const int, char **, const bool)
 * \brief Constructor without given default filename.
 * (Stefan Fruhner - 03.11.2011 11:03:56 CET)
 *
 *	Creates an instance of myInifiles. Therefore the number of commandline
 *	arguments and the array of the commandline arguments have to be given. If
 *	the last parameter is true, additional info messages are printed to the
 *	standard output. If no filename was given via the commandline, a default-
 *	filename is constructed from the name of the program.
 *
 * @param argcnt : Number of arguments on the commandline
 * @param args   : Arguments from the commandline
 * @param verbose: True if debug messages shall be printed to stdout.<br>
 *				Default is false.
 **/
myIniFiles::myIniFiles(const int argcnt, char ** args, const bool verbose)
		: text(stringstream::in | stringstream::out),
		  debug(verbose),
		  checks_done(false),
		  dependency_checking_enabled(true),
		  comment_char('#'),
		  argc(argcnt), argv(args),
		  params(),
		  infotext("program description"),
		  //current_section(mystring()),
		  sections() {

	register_param("help", "?", "this help", false);
	register_param("Doc", NULL, "[key] Prints out the help for a single key", true);
	register_param("inifile", "f", "inifile to load/save settings", true);
	register_param("free_opts", NULL, "Print all chars that are free to use as options", false);
	register_param("bash_completion", NULL , "Creates a script to enable bash completion for program parameters.", false);
	register_param("initemplate", NULL , "Creates a simple ini template so that the user only has to fill in the values.", false);

	// check if the user supplied a filename
	filename = check_commandline(argcnt, args);
	if (debug) cout << " --> Commandline wants to load "<< filename  << endl;

	if (filename.strip().empty() and argcnt > 0){ // compute an alternative ini file name from the name of the program
		mystring progname(args[0]);
		filename = progname.file_base(true).file_new_ext(".ini");
	}

	if (debug) cout << " --> Using inifile "<< filename  << endl;

	// Read the inifile to memory
	fstream ini(filename.c_str(), fstream::in);
	if (ini.is_open()){				// file opened successfully, read the data
		ini.seekg(0, ios::end);		// set get pointer to the end
		size_t size = ini.tellg();	// position = size
		ini.seekg(0, ios::beg);		// set get point to the beginning
		char *buf = new char[size];
		ini.read(buf,size);
		text.write(buf, size);
		ini.close();
		delete[] buf;
	}
	else
	if (debug) { // exception if file could not be opened

		std::cout << "Could not open ini file " << filename <<
		".\nCreating new one if requested by the program." << endl;
	}
	return;
}

/** \fn void myIniFiles::set_active_section(const char * const)
 * \brief Change the current section.
 *
 * \param sec: A C-string, that contains the new section.
 */
void myIniFiles::set_active_section(const char * const sec){
	if(debug) std::cout<<__FUNCTION__<<": Adding section '"<<sec<<"' to the stack (number "<<sections.size() + 1<<").\n";
	sections.push(mystring(sec));
	//current_section = sections.back();
}

/**\fn void myIniFiles::unset_active_section()
 * \brief Clear the current section.
 *
 * This method will unset the current set section, so globally
 * defined parameters can be read.
 *
 **/
void myIniFiles::unset_active_section(){
	if(debug) std::cout<<__FUNCTION__<<": Removing any section from the stack (number "<<sections.size()<<").\n";
	if(sections.size() > 0) sections.pop();
}


/**\fn void myIniFiles::unset_active_section(const char * const)
 * \brief Clear the current section.
 *
 * This method will unset the current set section, so globally
 * defined parameters can be read.
 *
 **/
void myIniFiles::unset_active_section(const char * const sec){
	if(get_active_section() == mystring(sec)) {
		if(debug) std::cout<<__FUNCTION__<<": Removing section '"<<sec<<"' from the stack (number "<<sections.size()<<").\n";
		sections.pop();
		return;
	}

	std::stringstream err;
	err <<__FILE__<<":"<<__LINE__<<":"<<__FUNCTION__<<"(..):\n";
	err <<" Active section: "<<get_active_section()<<"\n";
	err <<"  Expected:      "<<sec<<"\n";
	throw Exception_WRONG_SECTION_ASSUMED(err.str());
}

/** \fn mystring myIniFiles::get_active_section() const
 * \brief Returns the current set section.
 *
 * \return The current section that is set.
 */
mystring myIniFiles::get_active_section() const {
	//return current_section;
	if(0 == sections.size())	return mystring("");
	else						return sections.top();
}

/** \fn myIniFiles::save(iostream &)
 *  Saves the ini file to the hard disk with a given reference to an open
 *  iostream.
 * @param ini: pointer to iostream to append the ini-file data
 */
void myIniFiles::save(iostream &ini){

	stringstream tmp(text.str());
	for (size_t i = 0; i < params.size(); i++){
		mylibs::cmdline::parameter &p = params[i];
		set_active_section(p.section.c_str());

		switch (p.paramtype){
		case mylibs::cmdline::parameter::keyvalue:
			if (p.value			!= "None")	write(p.key, p.value);
			else
			if (p.default_value != "None")	write(p.key, p.default_value);
			else							write(p.key, string());
			break;
		case mylibs::cmdline::parameter::flag    :
			if ( exists(p.key) ) write(p.key, string());
			//! \todo what about flags ?
			break;
		case mylibs::cmdline::parameter::list    :
			//! \todo what about lists ?
			break;
		}

	unset_active_section();
	}

	//unset_active_section();

	if (not ini.good())	throw myIniFiles::Exception_FILE_OPEN(_LINE_);
	ini << text.str();

	text.str(tmp.str());
}

/** \fn void myIniFiles::save(const mystring)
 *  Saves the ini file to the hard disk.
 * \param fn: Mystring, path and filename where to store the inifile.
 **/
void myIniFiles::save(const mystring fn){
	mystring fname(fn);
	if (fn.empty()) {
		if ( debug ) cout << "ini.save(): Using default inifile: " << filename  << endl;
		fname = filename;
	}

	fstream ini(fname.c_str(), fstream::out);
	if (! ini.is_open()){
		std::cerr << "Could not open ini file :" << fname << "." << endl;
		return;
	}
	this->save(ini);
//	ini << text.str();
	ini.close();
}

/** \fn void myIniFiles::gen_ini_template(const char * const) const
 *  Saves an empty ini file to the hard disk, containing all options and
 *  descriptions.
 *
 * \todo Lists should be saved as {\<listname\>}
 * *
 * \param fn: A c-string, containing path and filename, where the template
 *				should be stored.
 */
void myIniFiles::gen_ini_template(const char * const fn) const {
	if (strlen(fn) == 0) throw myIniFiles::Exception_FILENAME(_LINE_ + "Filename invalid.");

	fstream ini(fn, fstream::out);
	if (! ini.is_open()){
		std::cerr << "Could not open ini file " << filename << "." << endl;
		return;
	}

	mystring temp_infotext(infotext);

	ini << "# " << temp_infotext.replace("\n", "\n#") << endl << endl;
	size_t max = 0;
	// find maximum length of the key strings
	for (size_t i = 0; i < params.size() ; i++){
		size_t l = params[i].key.size() + 11;
		max = (l > max) ? l : max;
	}

	for (size_t i = 0; i < params.size() ; i++){
		if (params[i].section.size())		continue;	// ignore sections
		if (params[i].dependency.size())	continue;	// ignore dependencies
		if (params[i].key == "inifile")		continue;	// ignore inifile param
		if (params[i].key == "help")		continue;	// ignore inifile param

		ini << endl << "# " << params[i].description << endl;
		ini << params[i].key << " = " << endl;
	}

	mystring last_sec;
	for (size_t i = 0; i < params.size() ; i++){
		if (not params[i].section.empty()){
			ini << endl
				<< "################################################################"
				<< endl
				<< "# sections:" << endl;
			break;
		}
	}

	for (size_t i = 0; i < params.size() ; i++){
		if (params[i].section.empty()) continue; 	// ignore if not in a section
		if (params[i].dependency.size()) continue; 	// ignore dependencies

		if (last_sec != params[i].section){
			ini << endl << "["<< params[i].section << "]"<< endl;
			last_sec = params[i].section;
		}
		ini << endl << "# " << params[i].description << endl;

		switch (params[i].paramtype){
		case mylibs::cmdline::parameter::keyvalue: ini << params[i].key << " = " << endl;break;
		case mylibs::cmdline::parameter::flag	: ini << params[i].key << endl; break;
		case mylibs::cmdline::parameter::list	: ini << "{"<< params[i].key<<"}" << endl << endl; break;
		default: throw myexception("Type error.");
		}
	}

	// determine if there are dependencies
	bool dependencies_found = false;
	for (size_t i = 0; i < params.size() ; i++){
		if (params[i].dependency.size()) {dependencies_found = true; break;}
	}
	if (not dependencies_found)	{ini.close(); return;}// If there where no dependencys, return at this point.

	ini << endl
		<< "################################################################"
		<< endl
		<< "# The following settings are dependent on other key-value-pairs."
		<< endl;

	//char last[256];
	mystring last;
	for (size_t i = 0; i < params.size() ; i++){
		if (not params[i].dependency.size()) continue;

		if (last != params[i].dependency){
			ini << endl << "# "<< params[i].dependency << endl;
			last = params[i].dependency;
		}

		switch (params[i].paramtype){
		case mylibs::cmdline::parameter::keyvalue: ini << "# " << params[i].key << " = " << endl;break;
		case mylibs::cmdline::parameter::flag	: ini << "# " << params[i].key << endl; break;
		case mylibs::cmdline::parameter::list	: ini << "#{"<< params[i].key<<"}" << endl << endl; break;
		default: throw myexception("Type error.");
		}
	}
	ini.close();
}

/** \fn void myIniFiles::writeStringListv(const string, vector<mystring> &)
 * \deprecated {Attention: writeStringListv(string, vector<mystring>&) is \
 *				deprecated, use writeVector with vector<mystring> instead !}
 *
 * \param key: Name of the list that sohould be written.
 * \param list: The vector of mystrings, that should be written.
 */
void myIniFiles::writeStringListv(const string key, vector<mystring> &list){
	writeVector(key, list);
}

/** \fn double myIniFiles::readdouble(const string, const double)
 * \brief Same as read<double>(key, standard).
 *
 * \param key: Name of the parameter that should be read.
 * \param standard: Default value that should be used if the parameter was not
 *				set within the inifile.
 **/
double myIniFiles::readdouble(const string key, const double standard){
	return read<double>(key, standard);
}

/** \fn double myIniFiles::readfloat(const string, const float)
 * \brief Same as read<float>(key, standard).
 *
 * \param key: Name of the parameter that should be read.
 * \param standard: Default value that should be used if the parameter was not
 *				set within the inifile.
 */
float myIniFiles::readfloat(const string key, const float standard){
	return read<float>(key, standard);
}

/** \fn double myIniFiles::readint(const string, const int)
 * \brief Same as read<int>(key, standard).
 *
 * \param key: Name of the parameter that should be read.
 * \param standard: Default value that should be used if the parameter was not
 *				set within the inifile.
 */
float myIniFiles::readint(const string key, const int standard){
	return read<int>(key, standard);
}

/** \fn double myIniFiles::readlong(const string, const long)
 * \brief Same as read<long>(key, standard).
 *
 * \param key: Name of the parameter that should be read.
 * \param standard: Default value that should be used if the parameter was not
 *				set within the inifile.
 */
long myIniFiles::readlong(const string key, const long standard){
	return read<long>(key, standard);
}

/** \fn double myIniFiles::readshort(const string, const short)
 * \brief Same as read<short>(key, standard).
 *
 * \param key: Name of the parameter that should be read.
 * \param standard: Default value that should be used if the parameter was not
 *				set within the inifile.
 */
short myIniFiles::readshort(const string key, const short standard){
	return read<short>(key, standard);
}

/** \fn double myIniFiles::readbool(const string, const bool)
 * \brief Same as read<bool>(key, standard).
 *
 * \param key: Name of the parameter that should be read.
 * \param standard: Default value that should be used if the parameter was not
 *				set within the inifile.
 */
bool myIniFiles::readbool(const string key, const bool standard){
	return read<bool>(key, standard);
}

/** \fn double myIniFiles::readstring(const string, const string)
 * \brief Same as read<string>(key, standard).
 *
 * \param key: Name of the parameter that should be read.
 * \param standard: Default value that should be used if the parameter was not
 *				set within the inifile.
 */
string myIniFiles::readstring(const string key, const string standard){
	return read<string>(key, standard);
}


/** readStringList
*	Reads a list of strings from a settings file
*	Lists are defined by an 'identifier' enclosed in culy brackets
*	( { and } ), namely
*	{identifier}
*	every line that follows is interpreted as list element until an
*	empty line follows.
* 	\deprecated {Attention: readStringList() is deprecated, use readVector
* 				with vector<mystring> instead !}
*	\param key		: the identifier, without brackets
*	\param alist	: a myStringList to which the items will be assigned
**/
void myIniFiles::readStringList(string key, myStringList &alist){
	cerr << "Attention: readStringList() is deprecated, use read(string , vector<mystring>)\
 instead !" << endl;

	 for (size_t i = 0; i < params.size(); i++){
		mylibs::cmdline::parameter &p = params[i];
		if (key == p.key and get_active_section() == p.section) {
			p.paramtype = mylibs::cmdline::parameter::list;
			break;
		}
	}

	key = '{' + key + '}';
	mystring buf;
	text.seekg(ios::beg);

	//! search line with list identifier '{identifier}'
	while (getline(text, buf))
		if ( buf.strip().startswith(key) ) break;

	while (getline(text, buf)){
		buf.strip();
		if (! buf[0]) break; // while line not empty, add to the list
		if ( not buf.startswith(comment_char) ) alist.append(buf);
	}
	text.seekg(0);
	text.clear();
}

/** \fn void myIniFiles::readStringListv(const string, vector<mystring> &)
 * \deprecated {Attention: readStringListv() is deprecated, use readVector
 *				with vector<mystring> instead !}
 *
 *	\param key	: the identifier, without brackets
 *	\param list	: a vector<mystring> to wich the items will be assigned
 */
void myIniFiles::readStringListv(const string key, vector<mystring> &list){
	read(key, list);
}

/** \fn mystring myIniFiles::check_commandline(const int, const char * const * const)
 *	Checks the commandline for '-f [filename]'
 *
 * \attention 	This function is no longer public. The filename is determined
 * 				automatically in the constructor.
 *
 * \param argc : Number of arguments
 * \param argv : paramters
 * \return filename of the ini file
 */
mystring myIniFiles::check_commandline(const int argc, const char * const * const argv){
	for (int i = 0; i < argc; i++){
		mystring opt(argv[i]);
		if (opt.length() == 0 or opt[0]!='-') continue;

		if (i < argc - 1){
			mystring arg(argv[i+1]);
			arg.strip();
			if (arg[0]=='-') continue;	// if next option then continue
			if (opt[1] == 'f') return arg;	// okay string found
		}
	}

	return mystring();
}

/** \fn bool myIniFiles::exists(const string)
 * checks if a key exists in the inifile. This can be a key value pair or
 * a section or a list. To check for sections the key must be embraced
 * in []. For lists you need to use {}.
 * @param key: the key to search for.
 * @return true or false
 **/
bool myIniFiles::exists(const string key){

	// If the user forgot to make the call to check(), we do it now
	if (not checks_done) check(1);

	mystring buf;
	bool found = false;

	// search in the inifile
	switch (key[0]){
		case '[': // search for sections
		case '{':{ //search for lists
			char e = 'a';
			switch (key[0]){
				case '[': e = ']';	break;
				case '{': e = '}';	break;
			}
			if ( e != key[key.size()-1] ) {
				stringstream ss;
				ss << "Descriptor " << key << " not valid : " << key[0]
					<< "..." << key[key.size()-1];
				cerr << ss.str() << endl;

				throw myIniFiles::Exception_INVALID_KEY(_LINE_ + ss.str());
			}
			text.seekg(ios::beg);
			text.clear();
			while (getline(text, buf))
				if ( buf.strip().startswith(key)) {
					 found = true;
					 break;
				}
			// for sections we can stop here, since there is no parameter line checking
			if (e == ']') return found;
			}
			break;
		default:
			mylibs::cmdline::parameter::type ptype = param_get_type(key);

			switch (ptype){
			case mylibs::cmdline::parameter::keyvalue: // search for keys
				buf = getkeyline(key).strip();
				found = (not buf.empty());
				break;
			case mylibs::cmdline::parameter::flag: // search for flags
				found = flag_exists_inifile(key);	break;
			case mylibs::cmdline::parameter::list:
				found = exists("{" + key + "}");	break; // if the user passed a list, then we add curly brackets
//				throw myexception_UnknownError(_LINE_" Seems as if you want to know if a list exists. Please use exists("{}")");
			// No need to check for lists, since they have to be checked with "{}"
			// Also lists have to be completely global
			}
	}
	text.seekg(ios::beg);
	text.clear();

	// search on the commandline
	mystring k(key);
	k.strip("[]{}");
	bool found_in_param = this->param_exists(k);

	if (debug ){
		if (found_in_param or found) cout << " Key " <<"'"<<key<<"'" << " exists." << endl;
		else cout << " Key " <<"'"<<key<<"'" << " NOT exists." << endl;
	}
	return (found_in_param or found);
}

/** \fn bool myIniFiles::flag_exists_inifile(const string)
 * (Stefan Fruhner - 07.11.2011 10:50:49 CET)
 * Checks if a flag exists in the inifile.
 * @param key : The key of the flag
 * @return True or false
 **/
bool myIniFiles::flag_exists_inifile(const string key) {
	mystring buf;
	text.seekg(ios::beg);

	// check if it is a flag at all
	for (size_t i = 0; i < this->params.size(); i++){
		if (params[i].key == key and params[i].section == get_active_section()) {
			if ( params[i].paramtype != mylibs::cmdline::parameter::flag )
				throw myIniFiles::Exception_NOT_A_FLAG(params[i].desc());
		}
	}

	bool fexists = false;
	if ( get_active_section().empty() ){
		while (getline(text, buf)){ // search for key
			// start of some section found >> stop searching
			if ( buf.strip().startswith("[") and buf.strip().endswith("]")){
				buf.clear();
				break;
			}

			if (buf.strip().startswith(key) ) {fexists = true; break;}
		}
	}
	else { // current section was set, we are searching in certain sections only
		bool in_section = false;
		while (getline(text, buf)){ // search for key
			// continue until section is found
			if (( buf.strip().startswith("[") and buf.strip().endswith("]"))
				and buf(1,-1) == get_active_section()){
				in_section = true;
				continue;
			}

			// start of next section
			if (	in_section and buf.strip().startswith("[")
					and buf.strip().endswith("]") and buf(1,-1) != get_active_section())
				in_section = false;

			if (not in_section) continue;

			if (buf.strip().startswith(key) ) {fexists = true; break;}
		}
	}

	text.seekg(ios::beg);
	text.clear();

	return fexists;
}

/** \fn void myIniFiles::print_param(const mylibs::cmdline::parameter, const size_t) const
 * @param param : Parameter object
 * @param max   : maximum length of all long identifiers
 * @return void
 */
void myIniFiles::print_param(const mylibs::cmdline::parameter param, const size_t max) const {
	stringstream ss;
	const size_t max_line_width = 76;

	if (not param.section.empty()){
		ss << "  --" << param.section << "::" << param.key;
	}
	else{
		if (param.option != '-')
			ss << "  -" << param.option << " [--" << param.key << "]";
		else
			ss << "  --" << param.key;
	}
	cout << ss.str() ;
	if (max > ss.str().size() )
		for (size_t j = 0; j < max - ss.str().size(); j++) cout << " ";

	// no line break needed
	if (max > 50 or param.description.size() <= max_line_width - max - 3)
		cout << " : "<<param.description << endl;
	else{
		cout << " : ";
		myStringList l = param.description.split();
		size_t c = max+3;
		for (size_t i = 0; i < l.size(); i++){ // every word
			if (c + l[i].size()+1 > max_line_width){
				cout << endl; // newline
				// indentation
				for (size_t k = 0; k < max+3; k++)	cout << " ";
				c = max+3;
			}
			cout << l[i] << " ";
			c += l[i].size()+1;
		}
		cout << endl;
	}
}

/** void myIniFiles::print_help(const mystring) const
 * Prints all parameters and quits the program.
 * This function is also invoked for --Doc topic.
 * @param topic: a keyword defining the parameter that is to be described.
 **/
void myIniFiles::print_help(const mystring topic) const {

	if (topic.size() > 0){ // --Doc
		cout << "Documentation for " << topic << ":"<< endl;
		mystring sec = mystring();
		mystring top = topic;
		myStringList needle;
		if (topic.contains("::"))
			needle = top.split("::");
		if (needle.size() == 2) {
			sec = needle[0];
			top = needle[1];
		}

		mystring last;
		size_t maxoffset = 0;
		for (size_t i = 0; i < params.size() ; i++){
			const mylibs::cmdline::parameter &p = params[i];
			if ( (top.size() == 1 and top[0] == params[i].option)
					or ( params[i].key.startswith(top))
					or ( params[i].section.startswith(top))){

				if (not sec.empty()
					and not ( params[i].section.startswith(sec))
					and not ( params[i].section.startswith(top)))
						continue; // wrong section

				size_t offset = p.section.size()+4+p.key.size() + 8;
				maxoffset = (offset > maxoffset) ?  offset : maxoffset;
			}

		}
		bool printed_param = false;
		for (size_t i = 0; i < params.size() ; i++){
			const mylibs::cmdline::parameter &p = params[i];
			if ( (top.size() == 1 and top[0] == params[i].option)
					or ( params[i].key.startswith(top))
					or ( params[i].section.startswith(top))){
//					(top == params[i].section)){

				if (not sec.empty()
					and not ( params[i].section.startswith(sec))
					and not ( params[i].section.startswith(top)))
//					and top !=  params[i].section)
						continue; // wrong section

				// if a section was opened
				if (not params[i].section.empty() and last != params[i].section){
					cout 	<< "Section [" << params[i].section	<<"]" << endl;
					last = params[i].section;
				}

				print_param(p, maxoffset);
				printed_param = true;
				if (p.default_value != "None"){
					for (size_t i = 0; i < maxoffset-7; i++) cout << " ";
					cout << "default : " << p.default_value << endl;
				}
				if (not p.default_value.contains("boolean")) {
					if (p.min != "None"){
						for (size_t i = 0; i < maxoffset-7; i++) cout << " ";
						cout << "min     : "  << p.min << endl;
					}
					if (p.max != "None"){
						for (size_t i = 0; i < maxoffset-7; i++) cout << " ";
						cout << "max     : "  << p.max << endl;
					}
				}
			}

		}

		if(printed_param) exit(0);
		// Documentation for concatenated flags
		maxoffset = 0;
		bool isconcat = true;
		for (size_t k = 0; k < top.size(); k++){
			bool found = false;
			for (size_t i = 0; i < params.size() ; i++){
				const mylibs::cmdline::parameter &p = params[i];
				if (top[k] == p.option) {
					size_t offset = p.section.size()+4+p.key.size() + 6;
					maxoffset = (offset > maxoffset) ?  offset : maxoffset;
					found = true; break;
				}
			}
			if (not found) {isconcat = false; break;}
		}

		if (isconcat)
		for (size_t k = 0; k < top.size(); k++){
			for (size_t i = 0; i < params.size() ; i++){
				const mylibs::cmdline::parameter &p = params[i];

				if (top[k] == p.option) print_param(p, maxoffset);
			}
		}
		exit(0);
	}

	::msg80(infotext.c_str());

	cout << endl;
//	cout << infotext << endl << endl;
	size_t max = 0;
	// find maximum length of the key strings
	for (size_t i = 0; i < params.size() ; i++){
		size_t l = params[i].key.size() + 11;
		max = (l > max) ? l : max;
	}

	cout << " params:" << endl;
	for (size_t i = 0; i < params.size() ; i++){
		if (params[i].section.size()) continue; 	// ignore sections
		if (params[i].dependency.size()) continue; 	// ignore dependencies
		print_param(params[i], max);
	}
	cout << endl;

	mystring last_sec;
	for (size_t i = 0; i < params.size() ; i++){
		if (not params[i].section.empty()){
			cout << " sections:" << endl;
			break;
		}
	}

	for (size_t i = 0; i < params.size() ; i++){
		if (params[i].section.empty()) continue; 	// ignore if not in a section
		if (params[i].dependency.size()) continue; 	// ignore dependencies

		if (last_sec != params[i].section){
			cout<< endl << "  ["<< params[i].section <<"]" << endl;
			last_sec = params[i].section;
		}

		print_param(params[i], max);
	}

	// determine if there are dependencies
	bool dependencies_found = false;
	for (size_t i = 0; i < params.size() ; i++){
		if (params[i].dependency.size()) {dependencies_found = true; break;}
	}
	if (not dependencies_found)	exit(0);

	cout<< endl << " dependent params:" << endl;
	//char last[256];
	mystring last;
	for (size_t i = 0; i < params.size() ; i++){
		if (not params[i].dependency.size()) continue;

		if (last != params[i].dependency){
			cout<< endl << "  "<< params[i].dependency << endl;
			last = params[i].dependency;
		}

		print_param(params[i], max);
	}
	exit(0);
}

/** \fn mystring myIniFiles::check_inifile()
 * Check the inifile for unknown parameters or lists. Finds also unknown
 * parameters which are within sections.
 *
 * \note Checks not for unknown sections and currently not for flags
 *
 * @return mystring containing unknown params.
 **/
mystring myIniFiles::check_inifile(){
	// check parameters in the inifile
	mystring unknowns;
	mystring buf;
	text.seekg(ios::beg);
	mystring sctn = "";
	if ( get_active_section().empty() ){
		while (getline(text, buf)){ // search for key
			buf = buf.strip();
			if ( buf.startswith("[") and buf.endswith("]")){
				sctn = buf.strip("[]");
				buf.clear();
				continue;
			}

			// search for unknown lists
			if ( buf.startswith("{") and buf.endswith("}")){
				buf = buf.strip("{}");
				bool found = false;
				for (size_t j = 0; j < params.size(); j++){
					if (params[j].key == buf) {found = true; break;}
				}

				if (not found){
					unknowns += buf + " (list in " + filename +") ";
				}
				continue;
			}

			if (buf.startswith('#') ) continue;
			//if ( buf.strip().startswith(key) and buf.contains('=')) break;
			if (buf.contains('=')){ // (key, value) pair found
				mystring buf2(buf);
				buf2 = buf2.slice(0, buf2.find('='));
				buf2.strip();
				bool found = false;
				for (size_t j = 0; j < params.size(); j++){
					if (params[j].section == sctn and params[j].key == buf2) {found = true; break;}
				}

				if (not found){
					unknowns += sctn+"::"+ buf2 + " (in " + filename +") ";
				}
			}
		}
	}

	text.seekg(0);  // return the read pointer to the beginning
	text.clear();	// clear the error buffer
	return unknowns;
}

#define BUG cout << __LINE__ << endl;
/** \fn void myIniFiles::check()
 * Checks the arguments given to the main program, if there are unknown
 * options or the flag '-?' or '--help' or '--Doc' or '--bash_completion'
 * or '--free_opts'
 *
 * In the case that unknown options are found, an exception is thrown.
 *
 * \param min_opts : minimum number of options needed.
 */
void myIniFiles::check(const int min_opts){
	checks_done = true; // save that checks were done (do it here, to prevent an infinity loop)
	string unknowns("");
	bool help_wanted = (argc > 0 and argc < min_opts);

	if (help_wanted) cerr << "Not enough args. There are "<< argc-1 <<"/"<<min_opts-1 <<endl;
	mystring help_topic("");
	bool ignore_lists = false;
	for (int i = 1; i < argc; i++){ // go through all the arguments on the cmdline
		mystring opt(argv[i]);
		opt.strip().replace("\\:\\:", "::");
		mystring t = opt;
		if (opt.startswith('[')) ignore_lists = true;
		if (ignore_lists and opt.endswith(']')) {ignore_lists = false; continue;}
		if (ignore_lists) continue;

		if (opt.length() == 0 or opt[0]!='-') continue; // check if option

		bool found = false;
		for (size_t j = 0; j < params.size(); j++){
			mylibs::cmdline::parameter &p = params[j];
			if (opt == "-?" or opt == "--help"	) {help_wanted = true; break;}
			if (opt == "--free_opts"		) { // create bash_completion script
				print_free_opt_chars();
				exit(EXIT_SUCCESS);
				break;
			}

			if (opt == "--bash_completion"		) { // create bash_completion script
				create_bash_completion_script();
				exit(EXIT_SUCCESS);
				break;
			}

			if (opt == "--initemplate"		) { // create bash_completion script
				gen_ini_template((filename+".template").c_str());
				exit(EXIT_SUCCESS);
				break;
			}

			if (opt == "--Doc") { // create bash_completion script
				param_getarg(help_topic, "Doc");
				found = true; help_wanted = true;
				break;
			}

			if (	(opt.length() == 2 and opt[1] == p.option and opt != "--") // short version matches
				or  (not p.section.empty() and opt == "--"+p.section+"::"+p.key)
				or 	(p.section.empty() and opt == "--"+p.key) ){											// long version matches
				found = true;
				break;
			}
		}

		// scan for concatenated flags, e.g. -PgoM
		if (opt.length() > 2 and (opt[1] != '-')){ // key is not '--' and longer than 2 chars
			mystring opt2(opt);
			opt2.strip("-");
			size_t cnt = 0;
			for (size_t k = 1; k < opt.length(); k++){
				bool f = false;
				for (size_t j = 0; j < params.size(); j++){
					mylibs::cmdline::parameter &p = params[j];
					if (p.paramtype != mylibs::cmdline::parameter::flag) continue;		// we are searching only for flags

					if (opt[k] == p.option) {
						cnt++;
						f=true;
						break;
					}
				}
				// if f is still false, then there was an unknown option
				if (not f) {
					found = false;
					break;
				}
			}
			if (cnt == opt.length()-1)	found = true; // all params where checked

		}

		if (not found) unknowns += opt + " ";
	}

	unknowns += check_inifile();

	// dependency checking
	if (not help_wanted and dependency_checking_enabled)
	for (size_t j = 0; j < params.size(); j++){
		mylibs::cmdline::parameter &p = params[j];
		if (p.dependency.size() > 0 and param_exists(p.key)){
			mystring dependency(p.dependency);
			myStringList l = dependency.split("="); // check only for '=' conditions here
			if (l.size() > 0){
				l[0].lstrip(':');
				mystring v;
				switch (l.size()){
					case 1: // dependency on a flag
						// more complex conditions are checked on read
						if (dependency.contains('>') or dependency.contains('<')) break;
						if (not exists(l[0])){
							cerr << " (!) Key = '" << p.key
								 << "' is dependent on the existence of " << l[0]
								 << " which is not present."<< endl;
							help_wanted = true;
						}
						break;
					case 2: // dependency on a key value pair
						param_getarg(v,l[0]);
						if (v != l[1]){
							cerr << " (!) For key = " << p.key
								 << " dependency not fulfilled " << dependency
								 << "." << endl;
							help_wanted = true;
						}
						break;
				}

				if (help_wanted) {
					help_topic = p.key;
					break;
				}
			}
		}
	}

	if ( help_wanted ) this->print_help(help_topic);

	if (unknowns.size() > 0){
		throw myIniFiles::Exception_UNKNOWN_OPT("Unknown options: "+unknowns);
	}
}

/** \fn void myIniFiles::print_free_opt_chars() const
 *
 * Prints out free chars that can be used as options. May be helpful if you
 * have a lot of parameters in your program. This function is automatically invoked,
 * if the parameter '--free_opts' exists.
 *
 **/
void myIniFiles::print_free_opt_chars() const {

	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	size_t len = strlen(alphanum);

	string free_opts = "";

	for (size_t i = 0; i < len; i++){
		bool found = false;
		for (size_t j = 0; j < params.size(); j++){
			if (params[j].option == alphanum[i]) {found = true; break;}
		}
		if ( not found ) free_opts += alphanum[i];
	}

	cout << "Free chars to use for options " << endl;
	for (size_t i = 0; i < free_opts.size(); i++){
		cout<< free_opts[i] << " ";
	}
	cout << endl;

	exit(EXIT_SUCCESS);
}

/** \fn bool myIniFiles::register_param(const string, const char * const, const string, const bool, const char * const)
 * As register_param but without section identifier. Needed for compatibility
 * reasons to ini files where no sections are needed.
 * @param key : string of the key = long version key for the command line
 * @param opt : short version for the command line
 * @param desc : description as shown in the help
 * @param needs_arg : True if an anrgumanet has to be supplied
 * @param dependency : String of the dependency, another key
 * @returns true on success
 **/
bool myIniFiles::register_param(const string key, const char * const opt, const string desc,
					const bool needs_arg, const char * const dependency){

	string dvalue = "None";
	if (!needs_arg) dvalue = "( flag )";

	if ( not get_active_section().empty() )
		return this->register_param_sec(get_active_section(), key, opt, desc,
										needs_arg, dvalue,
										string("None"), string("None"),
										dependency);
	else
		return this->register_param_sec(string(), key, opt, desc,
										needs_arg, dvalue,
										string("None"), string("None"),
										dependency);
}

/** \fn bool myIniFiles::register_param(const char * const, const char * const, const string, const bool, const char * const)
 * \todo Documentation: Difference between the register_param methods.
 **/
bool myIniFiles::register_param(const char * const key, const char * const opt, const string desc,
					const bool needs_arg, const char * const dependency){
	return this->register_param(string(key), opt, desc, needs_arg, dependency);
}

/** \fn bool myIniFiles::register_param(const char *, const char * const, const string, const char * const, const char * const)
 *
 **/
bool myIniFiles::register_param(const char * key, const char * const opt, const string desc,
					const char * const default_string, const char * const dependency){

	if ( not get_active_section().empty() )
		return this->register_param_sec(get_active_section(), string(key), opt, desc,
										true, default_string,
										string("None"), string("None"),
										dependency);
	else
		return this->register_param_sec(string(), string(key), opt, desc,
										true, default_string,
										string("None"), string("None"),
										dependency);
}

/**\fn bool myIniFiles::register_flag(const char * const, const char * const, const string, const char * const)
 * \brief Alias for register_param(const string, const char * const, const string, const bool needs_arg, const char * const) with needs_arg = false.
 *
 * @param key : string of the key = long version key for the command line
 * @param opt : short version for the command line
 * @param desc : description as shown in the help
 * @param dependency : String of the dependency, another key
 * @returns true on success
 **/
bool myIniFiles::register_flag(const char * const key, const char * const opt,
								const string desc, const char * const dependency){
	return this->register_param(string(key), opt, desc, false, dependency);
}

/** \fn bool myIniFiles::register_param_sec(const string, const string, const char * const, const string,
		const bool, const string, const string, const string, const char * const)
 * Registers any parameter that is used in the ini file. Does error and
 * consistency checking. Appends key to the params-vector.
 *
 * @param section : section name of the key value pair
 * @param key : string of the key = long version key for the command line
 * @param opt : short version for the command line
 * @param desc : description as shown in the help
 * @param needs_arg : True if an argument has to be supplied
 * @param dependency : String of the dependency, another key
 * @param defaultvalue : Default value of the parameter
 * @param minimum : minumum value
 * @param maximum : maximum value
 * @return true on success
 */
bool myIniFiles::register_param_sec(const string section, const string key, const char * const opt, const string desc,
					const bool needs_arg, const string defaultvalue,
					const string minimum, const string maximum,
					const char * const dependency){
	// create dependency string according to 'section::key'
	mystring dep = mystring();
	if (dependency) dep = section+"::"+mystring(dependency);

	// check for existing keys
	for (size_t i = 0; i < params.size(); i++){
		if ((params[i].section == section) and (params[i].key == key)) { // section and key were found

			// no dependency
			if (params[i].dependency.empty()
				or not dependency_checking_enabled or ( dep.empty())){
				string msg = _LINE_ + "Key '" + key +"' in section '"+ section
							 + "' already in the list.";
				throw myIniFiles::Exception_REGISTRATION(msg);
			}
			if (params[i].dependency == dep){ //! if dependencies are equal
				string msg = _LINE_ + "Key '"+ key + "' already in section '"
					 + section + "' with dependency '" +dep +"'.";
				throw myIniFiles::Exception_REGISTRATION(msg);
			}
		}
	}

	/** check if options already exist
	 * \note 	There is no section checking for options, using sections only
	 * 			the long versions of the keys are applicable !
	 **/
	if (opt){
		for (size_t i = 0; i < params.size(); i++){
			if (params[i].option == opt[0]){
				if (not section.empty() or params[i].dependency.empty()
					or not dependency_checking_enabled){
					string msg = _LINE_+"Option '"+opt[0]+"' already in use.";
					throw myIniFiles::Exception_REGISTRATION(msg);
				}

				if (params[i].dependency == dep){
						string msg = _LINE_+"Option '"+opt[0]
										+"' already in use with dependency '"
										+ dep+"'.";
						throw myIniFiles::Exception_REGISTRATION(msg);
				}
			}
		}
//		params.push_back(mylibs::cmdline::parameter(section, key, opt[0], desc, needs_arg, dependency));
		params.push_back(mylibs::cmdline::parameter(section, key, opt[0], desc, needs_arg,defaultvalue, minimum, maximum, dependency));
	}
	else // no one-char option will be treated as '--'
		params.push_back(mylibs::cmdline::parameter(section, key, '-', desc, needs_arg, defaultvalue, minimum, maximum, dependency));
//		params.push_back(mylibs::cmdline::parameter(section, key, '-', desc, needs_arg, dependency));
	return true;
}

/** \fn bool myIniFiles::param_needs_arg(const string) const
 * \brief Check if a key needs an argument, section has to be set before.
 *
 *	This is an alias for param_needs_arg(\a get_active_section(), key).
 *
 * \param key: A string, the name of the parameter, for which to check if it
 *				needs an argument.
 * \return True if the parameter needs an argument, false if not.
 */
bool myIniFiles::param_needs_arg(const string key) const {
	return myIniFiles::param_needs_arg(get_active_section(), key);
}

/** \fn bool myIniFiles::param_needs_arg(const string, const string) const
 * \brief Check if a key needs an argument, section is given.
 *
 *	This method will check if a given parameter needs an argument. Therefore
 *	the section has to be set.<br>
 *	If there registered parameters, but the key was not found an
 *	Exception_UNREGISTERED_KEY is thrown.<br>
 *	If no parameter is registered until now, false is returned.
 *
 * \param section: Within which section is the key. string("") means the key is
 *				not within a section.
 * \param key: A string, the name of the parameter, for which to check if it
 *				needs an argument.
 * \return True if the parameter needs an argument, false if not.
 */
bool myIniFiles::param_needs_arg(const string section, const string key) const {
	for (size_t i = 0; i < this->params.size(); i++)
		if (params[i].key == key and params[i].section == section)
			return (params[i].paramtype != mylibs::cmdline::parameter::flag);// If the parameter is not a flag, it needs an argument.

	throw myIniFiles::Exception_UNREGISTERED_KEY("'"+ section + "::" +key + "'");
	return false;
}
/** \fn mylibs::cmdline::parameter::type myIniFiles::param_get_type(const string) const
 * \brief Get the type of a key. The section must be set.
 *
 *	This is an alias for param_get_type(get_active_section(), key).
 *
 * \param key: The name of the parameter for which to return the type.
 * \return The type of the key (e.g. if it is a list or a flag).
 */
mylibs::cmdline::parameter::type myIniFiles::param_get_type(const string key) const {
	//for (size_t i = 0; i < this->params.size(); i++){
		//if (params[i].key == key and params[i].section == get_active_section())
			//return (params[i].paramtype);
	//}

	//if (params.size() > 0)
		//throw myIniFiles::Exception_UNREGISTERED_KEY("'"+ get_active_section() + "::" +key + "'");
	//return mylibs::cmdline::parameter::keyvalue;

	return param_get_type(get_active_section(), key);

}

/** \fn mylibs::cmdline::parameter::type myIniFiles::param_get_type(const string, const string) const
 * \brief Get the type of a key within a given section.
 *
 *	This method will return the type of a key, that was registered in a given
 *	section.<br>
 *	If parameters are registered, but the key was not found, an
 *	Exception_UNREGISTERED_KEY is thrown.<br>
 *	If no parameter is registered until now, keyvalue is returned as default.
 *
 * \param section: The section within which the key is registered.
 * \param key: The name of the parameter for which to return the type.
 * \return The type of the key (e.g. if it is a list or a flag).
 */
mylibs::cmdline::parameter::type myIniFiles::param_get_type(const string section, const string key) const {
	for (size_t i = 0; i < this->params.size(); i++){
		//if (params[i].key == key and params[i].section == get_active_section())
		if (params[i].key == key and params[i].section == section)

			return (params[i].paramtype);
	}

	throw myIniFiles::Exception_UNREGISTERED_KEY("'"+ section + "::" +key + "'");
	return mylibs::cmdline::parameter::keyvalue;

}

/** \fn const mylibs::cmdline::parameter& myIniFiles::get_param(const string) const
 * \brief Returns a commandline parameter. The section must be set.
 *
 *	This is an alias for get_param(get_active_section(), key)
 *
 * \param key: Name of the parameter, that should be returned.
 * \return A constant reference to the parameter.
 */
const mylibs::cmdline::parameter& myIniFiles::get_param(const string key) const {
	//for (size_t i = 0; i < this->params.size(); i++){
		//if (params[i].key == key and params[i].section == get_active_section())
			//return params[i];
	//}

	//if (params.size() > 0)
		//throw myIniFiles::Exception_UNREGISTERED_KEY("'"+ get_active_section() + "::" +key + "'");
	//return params[0];

	return get_param(get_active_section(), key);
}

/** \fn const mylibs::cmdline::parameter& myIniFiles::get_param(const string, const string) const
 * \brief Returns a commandline parameter within a given section.
 *
 *	This method will return a parameter within a given section.
 *	If already parameters are registered, but the key is not found, an
 *	Exception_UNREGISTERED_KEY is thrown.
 *
 * \param section: The section within which the parameter is.
 * \param key: Name of the parameter, that should be returned.
 * \return A constant reference to the parameter.
 **/
const mylibs::cmdline::parameter& myIniFiles::get_param(const string section, const string key) const {
	for (size_t i = 0; i < this->params.size(); i++){
		//if (params[i].key == key and params[i].section == get_active_section())
		if (params[i].key == key and params[i].section == section)
			return params[i];
	}

	throw myIniFiles::Exception_UNREGISTERED_KEY("'"+ section + "::" +key + "'");
	return params[0];
}

/** \fn bool myIniFiles::param_exists(constr string) const
 *
 * Checks if the key was found on the command line.
 *
 * @param key: long version of the key (without the section)
 * @return True, if the parameter exists
 **/
bool myIniFiles::param_exists(const string key) const {
	if ((this->argc == 0) or (this->argv == NULL)) return false; // stop here, if there is no option
//		throw myIniFiles::Exception_NO_ARGV(_LINE_);
	char option = '\0';
	string longopt("");
	bool needs_arg = false;
	for (size_t i = 0; i < this->params.size(); i++)
		if (params[i].key == key) {
			option  = params[i].option;		// get the appropriate option
			if (not get_active_section().empty() and params[i].section != get_active_section()) continue;
			if (get_active_section().empty())
				longopt = "--"+params[i].key;	// and the long version
			else
				longopt   = "--" + get_active_section() + "::" + params[i].key;

//			needs_arg = params[i].needs_argument;
			needs_arg = not (params[i].paramtype == mylibs::cmdline::parameter::flag);
			break;
		}
	if (option=='\0') // key obviously not registered
		throw myIniFiles::Exception_UNREGISTERED_KEY("'" +key + "'");

	// start parsing the commandline
	// Step 1) Options given as '-O' or '--Option'
	for (int i = 0; i < argc; i++){
		mystring opt(argv[i]);
		opt.strip().replace("\\:\\:", "::");
		if (opt.length() == 0 or opt[0]!='-') continue; 	// check if option
		if ( 	(opt.length() == 2 and (opt[1]==option) and (opt != "--"))
			or 	(opt == longopt) ){ // option was found
			return true;
		}
	}

	// Step2) Short options concatenated into one single option, e.g. -PgOS
	if (not needs_arg)
		for (int i = 1; i < argc; i++){
			mystring opt(argv[i]);
			opt.strip().replace("\\:\\:", "::");
			if (opt.length() == 0 or opt[0]!='-') continue; 	// check if option
			if (opt.length() > 2 and (opt[1] != '-')){ // key is not '--' and longer than 2 chars
				for (size_t j = 1; j < opt.length(); j++){
					if (opt[j] == option) return true;
				}
			}
		}
	return false; // stop here, if there is no option
}

/** \fn mystring myIniFiles::param_non_opt_args() const
 * Parses the argument list on the commandline and returns all arguments that
 * do not belong to command line switches.
 * \attention A problem appears using lists on the commandline --
 * e.g. -v 1 2 3 4 -y test. They are allowed to be separated by whitespaces.
 * Therefore we cannot distinguish between remaining args and list elements
 *
 * \todo How to fix this ? Leave the sanity checking for the user ?
 * @return mystring containing remaining args.
**/
mystring myIniFiles::param_non_opt_args() const {
	if ((this->argc == 0) or (this->argv == NULL)) return mystring(); // stop here, if there is no option
//		throw myIniFiles::Exception_NO_ARGV(_LINE_);
//			throw string("You have to register_cmdline(argc, argv) first.");

	mystring args;
	//for (int j = 1; j < argc; j++){ // parse the remaining params
		//mystring arg(argv[j]);
		//arg.strip();
		//if (arg[0]=='-') break; // next option started
		//if (args.size() > 0 ) args += ' ';
		//args += arg;
	//}
	//return args;

	for (int i = 1; i < argc; i++){
		bool found = false;
		mystring arg(argv[i]);
		arg.strip();

			if (arg[0] == '-'){
			bool long_opt = arg.startswith("--");
			arg.strip("-");

			for (size_t j = 0; j < params.size(); j++){
				const mylibs::cmdline::parameter &p = params[j];
				if ( (long_opt and arg == p.key ) or
					 (not long_opt and arg[0] == p.option)){ // param found
//					if (p.needs_argument) i++;	//skip argument/ increment outer loop
					if (p.paramtype != mylibs::cmdline::parameter::flag){
						// check if following arg starts with '-'
						if (i+1 < argc and argv[i+1][0] == '-')
							throw myIniFiles::Exception_NEEDS_ARG(p.desc());
						i++;	//skip argument/ increment outer loop
					}
					found = true;
					break;// stop inner loop
				}
			}
		}
		if (found) continue;
		args += arg+" ";
	}

	return args.rstrip(); // remove trailing
}


/** \fn bool myIniFiles::param_getarg(string &, const string) const
 * Searches for a command line parameter. This can be provided using the
 * short version (-x) or the long version (--long_key)
 *
 * @param var : Where to write the result.
 * @param key : long version of the key without -- and without section identifier
 * @return True, if the value was found.
 **/
bool myIniFiles::param_getarg(string &var, const string key) const {
	if ((this->argc == 0) or (this->argv == NULL)) return false; // stop here, if there is no cmdline
//		throw myIniFiles::Exception_NO_ARGV(_LINE_);
	char option = '\0';
	bool needs_arg = false;
	string longopt;

	for (size_t i = 0; i < this->params.size(); i++) // go through the list of params
		if (params[i].key == key) {					 // parameter found
			if (not get_active_section().empty() and params[i].section != get_active_section()) continue;
			option	= params[i].option;				 //
			if (get_active_section().empty())			 // add the section to the cmdline
				longopt   = "--" + params[i].key;
			else
				longopt   = "--" + get_active_section() + "::" + params[i].key;

//			needs_arg = params[i].needs_argument;
			needs_arg = (params[i].paramtype != mylibs::cmdline::parameter::flag);
			// check for dependencies
			if (dependency_checking_enabled and params[i].dependency.size() > 0){
				mystring dependency(params[i].dependency);
				/*! \todo Dependencies on flags is not currently supported.
				 * 		   Dependencies must be given in the form \< key \>=\< value \>
				 *
				 */
				myStringList l = dependency.split('=');
				l[0] = l[0].lstrip(':');
				mystring v;
				switch (l.size()){
					case 1: // dependency on a flag or a more complicated condition
						break; // nothing to do here
					case 2: // dependency on a key value pair
						param_getarg(v,l[0]);
						if (v != l[1]){
							string msg = _LINE_+ "For key = " + key
								 + " dependency not fulfilled " + dependency
								 + ".";
							throw myIniFiles::Exception_DEPENDENCY(EXCEPTION_ID + msg);
						}
						break;
				}
			}
			break;
		}
	if (option=='\0') throw myIniFiles::Exception_UNREGISTERED_KEY(EXCEPTION_ID + "'" +key + "'");

	// find index in the parameter list, that belongs to the key
	int optindex = -1;
	for (int i = 1; i < argc; i++){
		mystring opt(argv[i]);
		opt.strip().replace("\\:\\:", "::");
		if (opt.length() == 0 or opt[0]!='-') continue; 	// check if option
		if ( (opt.length() == 2 and (opt[1]==option) and (opt != "--"))
			or (opt == longopt) ){ // option was found
			optindex = i;
			var = ""; 		// clear the result
			break;
		}
	}
	if (optindex == -1) return false; // stop here, if there is no option
	else if (not needs_arg) throw myIniFiles::Exception_NEEDS_NO_ARG(EXCEPTION_ID +"Key='"+key+"'.");

	bool ignore_lists = false;
	for (int j = optindex+1; j < argc; j++){ // parse the remaining params
		mystring arg(argv[j]);
		arg.strip();

		if (arg.startswith('[')) ignore_lists = true;				 // beginning of a list found
		if (not ignore_lists and arg[0]=='-') break; 				 // next option started
		if (ignore_lists and arg.endswith(']')) ignore_lists = false;// end of a list found
		if (var.size() > 0 ) var += ' ';
		var += arg;
	}

//	mystring tmp = mystring(var);
//	if (debug) cout << var << endl;
//	var = tmp.strip("'");
//	if (debug) cout << var << endl;

	switch (needs_arg){
		case true:
			if (var.size() == 0) throw myIniFiles::Exception_ARG(EXCEPTION_ID + "Key='"+key+"'.");
			break;
		case false:
			if (var.size() != 0) throw myIniFiles::Exception_ARG(EXCEPTION_ID + "Key='"+key+"'. To many args:" + var+'.');
			break;
	}
	return true;
}

/** \fn mystring myIniFiles::getkeyline(const string)
 *	Searches for a given key and returns the string that refers to
 *	the value in a key=value pair. If a value is present multiple times, then
 *	the first occurence is reported.
 *	If the value 'current_section' was set then the function only find values
 *	in this section. Sections can be reopened.
 *
 *	\attention 	Comments are ignored automatically, because keys are
 *				not allowed to start with the comment_char.
 *
 *	\param key : identifier of the key
 *	\return the 'value' of the string "key = value", If no '=' was
 *			found, the complete line is returned, If no result was found then
 * 			an empty string is returned.
 **/
mystring myIniFiles::getkeyline(const string key) {
	mystring buf;
	text.seekg(ios::beg);

	if ( get_active_section().empty() ){
		while (getline(text, buf)){ // search for key
			// start of some section found >> stop searching
			if ( buf.strip().startswith("[") and buf.strip().endswith("]")){
				buf.clear();
				break;
			}

			//if ( buf.strip().startswith(key) and buf.contains('=')) break;
			if (buf.contains('=')){ // (key, value) pair found
				mystring buf2(buf);
				buf2 = buf2.slice(0, buf2.find('='));
				buf2.strip();
				if ( buf2 == key ) break;
			}
		}
	}
	else { // current section was set, we are searching in certain sections only
		bool in_section = false;
		while (getline(text, buf)){ // search for key
			// continue until section is found
			buf.strip();
			if (( buf.startswith("[") and buf.endswith("]"))
				and buf(1,-1) == get_active_section()){
				in_section = true;
				continue;
			}

			// start of next section
			if (	in_section and buf.startswith("[")
					and buf.endswith("]") and buf(1,-1) != get_active_section())
				in_section = false;

			if (not in_section) continue;

			//if ( buf.strip().startswith(key) and buf.contains('=')) break;
			if (buf.contains('=')){ // (key, value) pair found
				mystring buf2(buf);
				buf2 = buf2.slice(0, buf2.find('='));
				buf2.strip();
				if ( buf2 == key ) break;
			}
		}
	}

	text.seekg(ios::beg);
	text.clear();
	size_t eq = buf.find('=');
	return buf(eq+1); //! return only the 'value' of the string "key = value"
}

/** \fn  void myIniFiles::create_bash_completion_script() const
 *	Bash completion can be activated by sourcing a script defining the
 * 	completion behaviour. Since all parameters have to be registered to this
 * 	library it's easy to write this script. Calling this function creates
 * 	this script and prints hints to stdout how to use bash completion.
 *
 * \todo sections use colons on the commandline, but for completion they need to be escaped
 */
void myIniFiles::create_bash_completion_script() const {

	mystring progname("");
	if (argv) progname = mystring(argv[0]);

	int slash = progname.find('/');
	if (slash > (int) progname.size()) slash = 0; // if no slash was found
	progname = progname(slash+1);					// cut absolute path

	if (progname.empty()) throw myIniFiles::Exception_ARG(EXCEPTION_ID + "argv[0] empty ?");
	mystring fn = progname + mystring(".bash_completion");

	fstream ini(fn.c_str(), fstream::out);
	if (! ini.is_open()){
		std::cerr << "Could not open file " << fn << "." << endl;
		return;
	}

	ini << "_"<< progname <<"()								\n\
{															\n\
	local cur prev opts										\n\
	COMPREPLY=()											\n\
	cur=\"${COMP_WORDS[COMP_CWORD]}\"						\n\
	prev=\"${COMP_WORDS[COMP_CWORD-1]}\"					\n\
	opts=\"";
	for (size_t i = 0; i < params.size(); i++){
		if (params[i].option and params[i].option != '-')
			ini << "-" << params[i].option <<" ";

		if (params[i].section.empty())
			ini <<  "--"<<params[i].key << " " << endl;
		else
			ini <<  "--" << params[i].section <<"\\:\\:" << params[i].key << " " << endl;
	}

//		COMPREPLY=( $(compgen -W \"${opts}\" -- ${cur} | sed 's/\\:/\\\\\\:/g'))
	ini << "\"												\n\
															\n\
	if [[ ${cur} == -* ]] ; then							\n\
		COMPRES=`compgen -W \"${opts}\" -- ${cur}`			\n\
		RESULTS=`echo $COMPRES| wc -w`						\n\
															\n\
		COMPREPLY=( $(compgen -W \"${opts}\" -- ${cur} ))\n\
															\n\
		if [ $RESULTS == \"1\" ]; then						\n\
			COMPRES=`echo $COMPRES | sed 's/-//g' `			\n\
			if [ \"$LAST_HINT\" != \"$COMPRES\" ]; then		\n\
				export LAST_HINT=$COMPRES;					\n\
				echo 										\n\
" << progname << " --Doc $COMPRES;							\n\
				echo -en \"$PS2\"							\n\
				echo -n \"${COMP_WORDS[*]}\"				\n\
			fi												\n\
		fi													\n\
		return 0											\n\
	fi														\n\
	if [[ ${prev} == --Doc ]] ; then						\n\
		opts=`echo ${opts} | sed 's/-//g'`					\n\
		COMPREPLY=( $(compgen -W \"${opts}\" -- ${cur}))\n\
		return 0											\n\
	fi														\n\
}															\n\
complete -F _"<< progname << " -f -d "<< progname	<< "			\n";
	cout << "File " << fn << " written. 				\n\
To activate bash completion add the line 'source "
	<< fn << "' to your .bashrc and add "<< progname
	<<" to your search PATH.						\n";
	ini.close();
	exit(0);
}

/** \fn bool myIniFiles::param_registered(string const) const
 * \brief Check if a parameter is registered.
 *
 *	This method uses the overloaded method. How, depends on key. If key does not
 *	contain a section (example 'help'), then the \a current_section is used.
 *	Otherwise the section given via the key is used.
 *
 * \return True if the parameter is already registered, false if not.
 */
bool myIniFiles::param_registered(string const key) const {
	if(key.find(string("::")) != string::npos) {
		myStringList sl(mystring(key).split(':'));
		if(debug) {
			std::cout<<"Parameter split into section '"<< sl.front() <<"' and key '"<< sl.back() <<"'.\n";
		}
		return param_registered(sl.front(), sl.back());
	} else {
		return param_registered(get_active_section(), key);
	}
}

/** \fn bool myIniFiles::param_registered(string const, string const) const
 * \brief Check if a parameter within a section is already registered.
 *
 * \param section: The section in which to search for the key.
 * \param key: The name of the parameter.
 * \return True if the key is already registered, false if not.
 */
bool myIniFiles::param_registered(string const key, string const section) const {
	for (size_t i = 0; i < this->params.size(); ++i){
		if (params[i].key == key and params[i].section == section)
			return true;
	}

	return false;
}
