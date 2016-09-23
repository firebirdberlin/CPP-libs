//		./myinifiles.hpp
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
 *  \page myIniFiles MyIniFiles

	A very general problem in program development is the parsing of inifiles
	and/or to read in paramters from the commandline.

	\section fast_intro For people with the lack-of-patience-syndrome
	The usage of this class is very minimalistic, when only simple programs
	are written. Here's a quick start example. Copy the following code and dave
	it as myinifile.cpp. You also find this program in demos/myinifile.cpp.

\verbatim
#include <mylibs/myinifiles.hpp>
using namespace std;

int main(int argc, char** argv){
	myIniFiles ini("test.ini",argc, argv);

	ini.register_param("param",  "p", "This is a demo parameter. This help message appears, when the user runs $./test -?");

	ini.check_params();

	int param = ini.read("param", 0); //< 0 is a default value, that is used, when the user supplied no value for "param"

	cout << param << endl;
	return 0;
};
\endverbatim

	Compile this program using
\verbatim
$ g++ myinifile.cpp -o test `mylibs-config --cldflags`
\endverbatim

	Run the program
\verbatim
$ ./test --param 2
2
\endverbatim

\verbatim
$ ./test -?
program description
 params:
  -? [--help]              : this help
  --Doc                    : [key] Prints out the help for a single key
  -f [--inifile]           : inifile to load/save settings
  --free_opts              : Print all chars that are free to use as options
  --bash_completion        : Creates a script to enable bash completion for program parameters.
  -p [--param]             : This is a demo parameter. This help message appears, when the
                             user runs $./test -?
\endverbatim
	\section ini_desc Description of the inifile format
	The class myIniFiles parses ini files by the following set of rules:

	Valid syntax is:
	\verbatim
	 key=value
	 key = value
	\endverbatim

	Curly brackets {} define a list, where the string within these
	brackets is the name(=key) of the list. Every line is one item.
	The list ends with an empty item.
	Also sections can be defined. It is allowed that keys within sections
	are equally named to keys outside sections.
	Lists cannot be stored within section.

	An example:
	\verbatim
	{my list}
	line 1
	line 2

	# a comment
	nextkey=value

	[Section]
	nextkey = anothervalue

	[Section2]
	newkey = anothervalue
	\endverbatim
	Lines starting with '#' are ignored.

	Every parameter can be overrun by the appropriate comand line setting.
	This behaviour can be used in shell scripts when a bunch of settings is
	equal for a set of simulations, but only single parameter shall be changed.

	Allowed is the short command
	\verbatim
	 $ [program name] -f another_inifile.ini ...
	\endverbatim

	... or the long command
	\verbatim
	 $ [program name] --inifile another_inifile.ini ...
	\endverbatim

	Sections support only the long version on the command line, e.g. the
	section is named 'Model' and the paramter is named 'dt':
	\verbatim
	 $ [program name] --Model::dt 0.3
	\endverbatim

	Lists can be supplied, using the name of the list
	\verbatim
	 $ [program name] --list val1 val2 val3
	\endverbatim

	You can also use
	\verbatim
	 $ [program name] --list [val1, val2, val3]
	\endverbatim

	Negative values can be used by setting the argument in brackets, e.g.
	\verbatim
	 $ [program name] --some_parameter_with_value (-5) or --param (-4.0)
	\endverbatim
*/

#ifndef __MYINIFILES__
#define __MYINIFILES__

#include <vector>
#include <set>
#include <stack>
#include "mystring.hpp"
#include "myexception.hpp"
#include <typeinfo>
#include <exception>
#include <iomanip>
#include <string.h>
#include <queue>

#define varname(vn) #vn
#define vardesc(vn) #vn,vn,typeid(test).name()
#define vn(var) #var,var

#define _LINE_ string(__FILE__) +":"+ toString(__LINE__) + " : "


// simple functions
bool contains(vector<mystring> &list, mystring needle);
namespace mylibs {
namespace cmdline {
class parameter {

	public:
		/** \enum type
		 * \brief Enum for the type of a parameter (e.g. flag or list).
		 *
		 * \todo Value for unknown type (e.g. for myIniFiles::param_get_type if
		 *		no parameter is registered).
		 */
		enum type {keyvalue, list, flag};

		mystring section;		//!< name of the section that stores the parameter
		mystring key;			//!< holds the name of the param
		char option;			//!< option without '-', e.g. for -f
		mystring dependency; 	//!< dependencies of this param
		mystring description;	//!< description
		mystring default_value; //!< Save the default_value as mystring, e.g. to print it out at the documentation
		mystring value; 		//!< Save the value as mystring, e.g. for saving them to the inifiles
		mystring min, max;
		type 	 paramtype;		//!< is it a key-value pair or a list ?

	parameter(const string sec_, const string key_, const char opt, const string desc, const bool arg,
				const char * const dep_ = NULL):
		section(sec_), key(key_), option(opt), dependency(mystring()),
		description(desc),	default_value("None"), value("None"),
		min("None"), max("None"), paramtype(keyvalue){

		if (not arg) paramtype = parameter::flag;

		/** \note 	Dependencies are designed such, that key value pairs may
		 * 			only be present if another key was set. These dependencies
		 * 			are checked only within one section. Internally we do this
		 * 			creating a dependency of the form "section::key".
		 **/

		//! prepare dependency in the form section::dependency
		if (dep_)
			dependency = mystring(sec_) + mystring("::") + mystring(dep_);
	}

	parameter(const string sec_, const string key_, const char opt, const string desc, const bool arg,
				const string defaultvalue, const string minimum,
				const string maximum, const char * const dep_=0):
		section(sec_), key(key_), option(opt), dependency(mystring()),
		description(desc), default_value(defaultvalue), value("None"),
		min(minimum), max(maximum), paramtype(keyvalue){

		if (not arg) paramtype = parameter::flag;

		/** \note 	Dependencies are designed such, that key value pairs may
		 * 			only be present if another key was set. These dependencies
		 * 			are checked only within one section. Internally we do this
		 * 			creating a dependency of the form "section::key".
		 **/

		//! prepare dependency in the form section::dependency
		if (dep_)
			dependency = mystring(sec_) + mystring("::") + mystring(dep_);
	}

	void info() const {
		cout << "Parameter info " << endl;
		cout <<" section        :"<<	section			<<endl;
		cout <<" key            :"<<	key				<<endl;
		cout <<" option         :"<<	option			<<endl;
		cout <<" dependency     :"<<	dependency		<<endl;
		cout <<" description    :"<<	description		<<endl;
		cout <<" default_value  :"<<	default_value	<<endl;
		cout <<" value          :"<<	value			<<endl;
		cout <<" min            :"<<	min 			<<endl;
		cout <<" max            :"<<	max				<<endl;
		cout <<" type           :"<<	paramtype		<<endl;
	}

	string desc() const {
		if (not section.empty())
			return	string("--"+section+"::"+key +"( -" +option + " ) :");
		return	string("--"+key +" ( -" +option + " ) :");
	}
};
} // end of namespace cmdline
} // end of namespace mylibs

/** \class myIniFiles
 * \brief Class for handling of commandline parameters and settings via file.
 *
 * This class is designed for handling the parameters of a program, given either
 * via commandline and/or within a file.<br>
 * Parameters given via commandline have thereby precedence over those within
 * a file.<br>
 * This class also enables a 'build-in' help for the parameters.<br>
 *
 *
 * After creation of a myIniFiles-object, the needed parameters must be
 * registered. If this is done they can be read and write.
 * If your program closes, you could use the save-method, to save the settings.
 *
 **/
class myIniFiles{
	public:

////////////////////////////// EXCEPTIONS //////////////////////////////////////
	class Exception_NEEDS_ARG : public myexception {
	public:	Exception_NEEDS_ARG(std::string id) :
			myexception(id+" Parameter needs an argument, but it couldn't be found. "){}
	};
	class Exception_NEEDS_NO_ARG : public myexception {
	public:	Exception_NEEDS_NO_ARG(std::string id) :
			myexception(id+" Parameter needs no argument.  Use ::exists()."){}
	};
	class Exception_NOT_A_FLAG : public myexception {
	public:	Exception_NOT_A_FLAG(std::string id) :
			myexception(id+" The key is assumed to be a flag, but in fact it is NOT !"){}
	};
	class Exception_SECTION_FAILURE : public myexception {
		public:	Exception_SECTION_FAILURE(std::string id) :
			myexception(id+" Active section set, which makes no sense.\n\
Use unset_active_section() before ."){}
	};
	class Exception_WRONG_SECTION_ASSUMED : public myexception {
		public:	Exception_WRONG_SECTION_ASSUMED(std::string id) :
			myexception(id+" Active section is not the one you expected.\n"){}
	};
	class Exception_DEPENDENCY : public myexception {
		public:	Exception_DEPENDENCY(std::string id) :
			myexception(id+" Dependency not fulfilled."){}
	};
	class Exception_UNKNOWN_OPT : public myexception {
		public:	Exception_UNKNOWN_OPT(std::string id) :
			myexception(id+" Unknown options found."){}
	};
	class Exception_ARG_FILE : public myexception {
		public:	Exception_ARG_FILE(std::string id) :
			myexception(id+" Mismatch in the argument read from file."){}
	};
	class Exception_ARG : public myexception {
		public:	Exception_ARG(std::string id) :
			myexception(id+" Mismatch in the cmdline argument."){}
	};
	class Exception_REGISTRATION : public myexception {
		public:	Exception_REGISTRATION(std::string id) :
			myexception(id+" Could not register option."){}
	};
	class Exception_UNREGISTERED_KEY : public myexception {
		public:	Exception_UNREGISTERED_KEY(std::string id) :
			myexception(id+" Key not registered. No documentation's included, command line reading does not work!"){}
	};
	class Exception_NO_ARGV : public myexception {
		public:	Exception_NO_ARGV(std::string id) :
			myexception(id+" No argc and/or argv supplied but expected."){}
	};
	class Exception_INVALID_KEY : public myexception {
		public:	Exception_INVALID_KEY(std::string id) :
			myexception(id+" Invalid key."){}
	};
	class Exception_FILE_OPEN : public myexception {
		public:	Exception_FILE_OPEN(std::string id) :
			myexception(id+" Could not open file !"){}
	};
	class Exception_FILENAME : public myexception {
		public:	Exception_FILENAME(std::string id) :
			myexception(id+" Filename mismatch."){}
	};
////////////////////////////////////////////////////////////////////////////////

	public:

		myIniFiles(const string inifile, const int argcnt = 0, char ** args = NULL, const bool debugging = false);
		myIniFiles(const int argcnt = 0, char ** args = NULL, const bool debugging = false); // shorter version
		void dependency_checking(const bool on){dependency_checking_enabled = on;}
		void set_info(const string msg){infotext = mystring(msg);}

		void set_active_section(const char * const sec);
		void unset_active_section();
		void unset_active_section(const char * const sec);
		mystring get_active_section() const;

		void save(mystring fn = mystring());
		void save(iostream &ini);
		void gen_ini_template(const char * const fn) const;

		template <class T>
		void write(const string key, const T value);
		template<class Type>
		void writeVector(const string key, const vector<Type> &list);

		void writeStringListv(const string key, vector<mystring> &list);

		template <class T>
		bool readVector(const string key, vector<T> &list, const vector<T> init = vector<T>())  __attribute__((deprecated));

		template <class T>
		bool read(const string key, vector<T> &list, const vector<T> init = vector<T>());

		template <class T>
		bool read(const string key, set<T> &list, const set<T> init = set<T>());

		double readdouble(const string key, const double standard = 0.);
		float readfloat(const string key, const float standard = 0.);
		float readint(const string key, const int standard = 0);
		long readlong(const string key, const long standard = 0);
		short readshort(const string key, const short standard = 0);
		bool readbool(const string key, const bool standard = false);
		string readstring(const string key, const string standard = string());

		template <class Typ>
		Typ read(const string key, const Typ standard);
		void readStringListv(const string key, vector<mystring> &list)  __attribute__ ((deprecated));
		void readStringList(const string key, myStringList &alist)  	__attribute__ ((deprecated));
		template <class T>
		mystring totext(const char * const name, const T value, const char * const type) const;
		bool exists(const string key);


		void print_help(const mystring topic=mystring("")) const;
		void check_params(const int min_opts = 1) __attribute_deprecated__{check(min_opts);};
		void check(const int min_opts = 2);

		mystring check_inifile();
		void print_free_opt_chars() const;
		bool register_param(const string key, const char * const opt, const string desc,
					const bool needs_arg=true, const char * const dependency=0);
		bool register_param(const char * const key, const char * const opt,
					const string desc=" >>> The program developer was too lazy to add documentation here. <<<",
					const bool needs_arg=true, const char * const dependency=0);
		bool register_param(const char *key, const char * const opt, const string desc,
					const char *default_string, const char * const dependency=0);
		template <class T>
		bool register_param(const string key, const char * const opt, const string desc,
							const T defaultvalue,
							const vector<T> allowed_values,
							const char * const dependency=0);
		template <class T>
		bool register_param(const string key, const char * const opt, const string desc,
							const T defaultvalue,
							const T minimum,
							const T maximum,
							const char * const dependency=0);
		bool register_flag( const char * const key, const char * const opt,
							const string desc=" >>> The program developer was too lazy to add documentation here. <<<",
							const char * const dependency=0);


		bool param_exists(const string key) const;
		bool param_needs_arg(const string key) const;
		bool param_needs_arg(const string section, const string key) const;
		mylibs::cmdline::parameter::type param_get_type(const string key) const;
		mylibs::cmdline::parameter::type param_get_type(const string section, const string key) const;
		const mylibs::cmdline::parameter& get_param(const string key) const;
		const mylibs::cmdline::parameter& get_param(const string section, const string key) const;

		mystring param_non_opt_args() const;
		bool param_getarg(string &var, const string key) const;
		template <class T>
		bool param_getarg(vector<T> &vec,string key) const;
		template <class T>
		bool param_getarg(set<T> &Set,string key) const;

		void verbose(bool on=true){debug = on;}
		mystring GetFilename()const {return filename;}

		bool param_registered(string const key) const;
		bool param_registered(string const section, string const key) const;

	private:
		stringstream text; //!< stores the ini-file
						   //!< It is loaded once, when the ini object
						   //!< is created
		bool debug;
		bool checks_done;
		bool dependency_checking_enabled;
		char comment_char;
		mystring filename;
		int argc;			// number of arguments
		char **argv;		// arguments given y commandline
		vector<mylibs::cmdline::parameter> params;
		mystring infotext;
		//mystring current_section;	//!< allows to set a section name, that is
									////!< currently selected. This selection
									////!< shall be used by all functions
		stack<mystring> sections;

		void print_param(const mylibs::cmdline::parameter param, const size_t max) const;
		bool register_param_sec(string section, string key,const char *opt,
							const string desc,	bool needs_arg,
							const string defaultvalue,
							const string minimum,
							const string maximum,
							const char *dependency);

		void create_bash_completion_script() const;
		mystring check_commandline(const int argc, const char * const * const argv);
		bool flag_exists_inifile(const string key);
		mystring getkeyline(const string key);
};

// #############################################################################
//               Now the definition of template and inline methods.
// #############################################################################

/** function: write
 * Sets a key=value pair. Searches for an old setting and overwrites it.
 * If current_section is set, then the value is searched within a section.
 * Appending of sectioned values, that are not listed yet, is done at the end
 * of the file
 *
 * \todo Writing parameters without arguments, no '='
 * \param key : the key of the parameter.
 * \param value : the value that is to be set.
 */
template <class T>
void myIniFiles::write(const string key, const T value){

	mylibs::cmdline::parameter::type ptype = mylibs::cmdline::parameter::keyvalue;
	// write the value to the parameters structure
	for (size_t i = 0; i < params.size(); i++){
		mylibs::cmdline::parameter &p = params[i];
		if (key == p.key and get_active_section() == p.section){
			p.value = toString(value);
			ptype = p.paramtype;
		}
	}

	mystring buf;
	stringstream out;

	text.seekg(ios::beg);	// set the get pointer to the beginning of the
							// stream
	text.clear(); 			// Sets a new value for the error control state.

	bool in_section = false;

	if (debug) cout << "Want to write "<< key << "=" << value << endl;

	//! copy the stream line by line, until the key was found
	if ( get_active_section().empty() ){
		if ( debug ) cout << "Searching in main section ... ";
		bool written=false;
		while (getline(text, buf)){
			//! stop when key was found
			if (buf.strip().startswith(key)){
				out << key;											// write the key-value pair
				if (ptype == mylibs::cmdline::parameter::keyvalue)
					out << "=" << setprecision(9) << value << endl;
				else out << endl;
				written = true;
				if (debug)
					cout << "Writing " <<key<<"="<<value<<" instead of " << buf <<endl;
				break;
			}

			// at the first section we stop searching
			if ( buf.strip().startswith("[") and buf.strip().endswith("]")){
				out << key;
				if (ptype == mylibs::cmdline::parameter::keyvalue)
					out << "=" << setprecision(9) << value << endl;	// write the key-value pair
				else out << endl;

				out << buf << endl;												// copy the current line
				written = true;

				if (debug)
					cout << "Writing " <<key<<"="<<value<<" before section starts: " << buf <<endl;
				break;
			}

			out << buf << endl; // copy the current line to the stream 'out'
		}

		if (not written){
			out << key;
			if (ptype == mylibs::cmdline::parameter::keyvalue)
				out << "=" << setprecision(9) << value << endl; // write the key-value pair
			else out << endl;

			if (debug)	cout << "Writing new " <<key<<"="<<value<< endl;
		}
	}
	else { // current section was set, we are searching in certain sections only
		if ( debug ) cout << "Searching in section " << get_active_section() << "... ";
		while (getline(text, buf)){ // search for key
			// continue until section is found
			if (( buf.strip().startswith("[") and buf.strip().endswith("]"))
				and buf(1,-1) == get_active_section()){
				in_section = true;
			}

			// start of next section
			if ( in_section and buf.strip().startswith("[")
				 and buf.strip().endswith("]") and buf(1,-1) != get_active_section()){
				in_section = false;
			}

			if ( in_section and  buf.strip().startswith(key) ) break; // key was found

			out << buf << endl; // write line to stream 'out'
		}

		// insert new section header (with a newline for visibility)
		if (not get_active_section().empty() and not in_section)
			out << endl << "[" << get_active_section() << "]" << endl;

		if (debug)
			cout << "Writing " <<key<<"="<<value<<" instead of " << buf <<endl;

		out << key;
		if (ptype == mylibs::cmdline::parameter::keyvalue)
			out << "=" << setprecision(9) << value << endl; // write the key-value pair
		else out << endl;
	}

	/** copy the rest of the stream
	 * /note If the key was not found then text is already at EOF.
	 **/
	if (not text.eof() ){
		while (getline(text, buf)) out << buf << endl;
//		cout << buf <<endl;
	}

	text.str("");		// clear the stream
	text.clear();		// clear the error control state
	text.str(out.str());// write the contents of out into the stream
	text.seekg(ios::beg);
}

template <class Typ>
/** \fn Typ myIniFiles::read(const string, const Typ)
 * Tries to read a <key, value> pair from an ini file. Secondly it reads the
 * same pair from command line and overrides the setting. If both where not
 * found then a standard value is set.
 * @param key : the long version of the key
 * @param standard : the standard value.
 * @return The 'value'
 **/
Typ myIniFiles::read(const string key, const Typ standard){

	// If the user forgot to make the call to check_params(), we do it now
	if (not checks_done) check(1);

	const mylibs::cmdline::parameter::type pt = param_get_type(key);

	// error checking
	if (pt != mylibs::cmdline::parameter::keyvalue){
		cerr << "key :" << key << endl;
		mystring msg = _LINE_ + "Key '" + key + "' is not of key-value type. " ;
		throw myIniFiles::Exception_NEEDS_NO_ARG(msg);
	}

	// would fail for lists
//	if (not param_needs_arg(key)){
//		cerr << "key :" << key << endl;
//		mystring msg = _LINE_ + "Key '" + key + "' needs no argument. " ;
//		throw myIniFilesException_NEEDS_NO_ARG(msg);
//	}

	text.seekg(ios::beg); // set the read buffer back to the begin of the file

	mystring line = getkeyline(key).strip();  // get the line containing the parameter
	if ( debug ) cout << get_active_section()<<"::"<< key << "=" << line
					  << " ( type: " << typeid(standard).name() << ")"
					  << endl;
	Typ val;
	if (line[0]){ // if found
		stringstream stream;
		stream << line; // do the conversion
		if ( ( stream >> val).fail() ){
			throw myIniFiles::Exception_ARG_FILE(EXCEPTION_ID
								+ "\nConversion failure: '"+ line +"'" );
		}
		if ( debug ) cout << "value : " << val << " ( type: " << typeid(standard).name() << ")" << endl;
	}
	else{
		if ( debug ) cout << " (!) using standard value : "<< standard << endl;
		val = standard;
	}

	// if no commandline options present.
	if ( argc == 0 ) return val;

	mystring arg;
	bool rescmd = false;

	rescmd = this->param_getarg(arg, key);
	if (debug and rescmd)
		cout << " (!) overridden by commandline : " << key << "=" << arg << endl;


	if (rescmd)	{	// If parameter was found,
		try{ 		// conversion of the key
			arg.strip("[{()}]"); // negative values need to be enclosed in brackets
			arg >>val; // override ini-file
		} catch (mystring_exception& e){
			mystring msg = _LINE_ + "Could not convert " +key+ " = '" + arg +"'. to " + typeid(Typ).name()+".";
			throw myIniFiles::Exception_ARG(EXCEPTION_ID+msg);
		}
	}


	// check the results here
	// ranges
	size_t pidx = string::npos;
	for (size_t i = 0; i < params.size(); i++){
		if (key == params[i].key and get_active_section() == params[i].section){

			params[i].default_value = toString(standard); // save the default_value
			params[i].value 		= toString(val);	  // save the value

			pidx = i;
			if (params[i].min != "None"){
				Typ minimum;
				try{
					params[i].min >> minimum;
				} catch (mystring_exception& e){
					mystring msg = _LINE_ + "Could not convert " +key+ " = '" + arg +"'. to " + typeid(Typ).name()+".";
					throw myIniFiles::Exception_ARG(EXCEPTION_ID+msg);
				}

				if (val < minimum){
					val =  minimum;
					if (debug)
						cout << " (!) " << key << "="<< val << " ( = minimum value ) " << endl;
					mystring msg = "Parameter out of range: " + arg +".\n";
					throw myIniFiles::Exception_ARG(EXCEPTION_ID+msg);
				}
			}
			if (params[i].max != "None"){
				Typ maximum;
				try{
					params[i].max >> maximum;
				} catch (mystring_exception& e){
					mystring msg = _LINE_ + "Could not convert " +key+ " = '" + arg +"'. to " + typeid(Typ).name()+".";
					throw myIniFiles::Exception_ARG(EXCEPTION_ID+msg);
				}
				if (val > maximum){
					val =  maximum;
					if (debug) cout << " (!) " << key << "=" << val << " ( = maximum value ) " << endl;
					mystring msg = "Parameter out of range: " + arg +".\n";
					throw myIniFiles::Exception_ARG(EXCEPTION_ID+msg);
				}
			}
			break;
		}
	}

	if (not dependency_checking_enabled or params[pidx].dependency.empty() or not exists(key)) return val;

	// dependencies
	mystring &dep = params[pidx].dependency;
	myStringList l = dep.split("=<>");
	l[0].lstrip(':');

	if (not exists(l[0])) throw myIniFiles::Exception_DEPENDENCY("key='"+key+"' dependency='"+dep+"'");

	mystring v = mystring();
	//! \note in check_params() we have already checked for '=' and flags
	if (dep.contains('>')){
		if (l.size() != 2) throw  myIniFiles::Exception_DEPENDENCY("dep. > ?");

		param_getarg(v, l[0]);
		Typ dvalue, cond;
		try{
			v    >> dvalue;
			l[1] >> cond;
		} catch (mystring_exception& e){
				mystring msg = _LINE_ + "Could not convert " + arg +" for key ='" + key +"'.";
				throw myIniFiles::Exception_ARG(EXCEPTION_ID + msg);
		}
		if (debug)	cout << "dependency :" << dep << " ? : " << l[0] <<"==" << dvalue << endl;
		if (cond >= dvalue)
			throw myIniFiles::Exception_DEPENDENCY(" (!) for key = '" +key
											+"' dependency :" + dep +".");
	}
	else
	if (dep.contains('<')){
		if (l.size() != 2) throw myIniFiles::Exception_DEPENDENCY("dep. < ?");
		param_getarg(v, l[0]);
		Typ dvalue, cond;
		try{
			v    >> dvalue;
			l[1] >> cond;
		} catch (mystring_exception& e){
				mystring msg = _LINE_ + "Could not convert " + arg +".";
				throw myIniFiles::Exception_ARG(EXCEPTION_ID + msg);
		}
		if (debug) cout << "dependency :" << dep << " ? : " << l[0] <<"==" << dvalue << endl;
		if (cond <= dvalue)
			throw myIniFiles::Exception_DEPENDENCY(" (!) for key = '" +key
											+"' dependency :" + dep +".");
	}
	return val;
}

template<class Type>
void myIniFiles::writeVector(string key, const vector<Type> &list){
	if (get_active_section().size())	throw myIniFiles::Exception_SECTION_FAILURE(get_active_section());

	key = '{' + key + '}';
	if (debug)
		cout << "writing list " <<  key << endl;
	mystring buf, bufs;
	stringstream out;

	text.seekg(ios::beg);	// set the get pointer to the beginning of the
							// stream
	text.clear(); // Sets a new value for the error control state.

	// copy the stream line by line, until the key was found
	bool written = false;
	while (getline(text, buf)){
		bufs = buf.strip();
		if ( bufs.startswith(key)) break;

		if (bufs.startswith('[') and bufs.endswith(']')){
			out <<  key << endl;
			for (size_t i = 0; i < list.size(); i++) out << setprecision(9) << std::fixed << list[i] << endl;
			out << endl; // and a newline to mark the end of the list
			written = true;
			break;
		}

		out << buf << endl;
	}

	/** write the list. If it was not found, then we are at the end of the file **/
	if (not written){
		out <<  key << endl;
		for (size_t i = 0; i < list.size(); i++) out << setprecision(9) << std::fixed << list[i] << endl;
		out << endl; // and a newline to mark the end of the list
	}

	//ignore the rest of the old list
	while (getline(text, buf)){
		if (buf.strip().empty()) break; //! when an empty line is found
										//! then the old list has ended
	}

	// copy the rest of the stream
	if (not text.eof() )
		while (getline(text, buf)) out << buf << endl;

	text.str("");		// clear the stream
	text.clear();		// clear the error control state
	text.str(out.str());// write the contents of out into the stream
	text.seekg(ios::beg);
}

template <class T>
mystring myIniFiles::totext(const char * const name, const T value, const char * const type) const {
	stringstream out;
	out << name << "=" << value << "\t#type: " << type;
	return out.str();
}

/** function read
 * Reads in a vector ( = list ) from the ini file.
 * In the ini file a list isdeclared as
 * \verbatim
 * {long identifier}
 * value 1
 * value 2
 *
 * # the empty line at the end is important
 * \endverbatim
 *
 * Given in the commandline a vector can be seperated by commas or whitespaces.
 * It also can be embraced in [].
 *
 * \attention non option arguments cannot follow a vector.
 *
 * \param key : identifier of the list
 * \param list: the vector which is filled with results
 * \param init: a vector used to initialize, when the vector was not found
 * 				in the ini file or on the command line
 * \return true if values where read, false otherwise
 */
template <class T>
bool myIniFiles::read(const string key, vector<T> &list, const vector<T> init){
	// search in ini file
	bool res = false;

	for (size_t i = 0; i < params.size(); i++){
		mylibs::cmdline::parameter &p = params[i];
		if (key == p.key and get_active_section() == p.section) {
			p.paramtype = mylibs::cmdline::parameter::list;
			break;
		}
	}

	string vkey = '{' + key + '}';
	mystring buf;
	list.clear();
	text.seekg(ios::beg);
	//! search line with list identifier '{identifier}'
	while (getline(text, buf)){
		if ( buf.strip().startswith(vkey) ) break;
	}

	int result = text.tellg();
	if (result != -1){ // if not at the end of the stream
		while (getline(text, buf)){
			buf.strip();
			if (! buf[0]) break; // while line not empty, add to the list
			if ( not buf.startswith(comment_char) ) {
				T tmp;
				buf >> tmp;
				list.push_back(tmp);
			}
		}
		res = true;
	}

	text.seekg(ios::beg);
	text.clear();
	if (debug) cout << vkey << " - read " << list.size() << " items " << endl;

	// set standard values
	if (list.size() == 0 ) {
		if (debug)
			cout << " (!) using standard values : " << init.size() << " items " << endl;
		list = init;
	}

	// override by commandline
	bool rescmd = this->param_getarg(list, key);
	if (debug and rescmd)
		cout << " (!) overidden by commandline : " << list.size() << " items " << endl;

	return res;
}

template <class T>
bool myIniFiles::readVector(const string key, vector<T> &list, const vector<T> init){
	return read(key, list, init);
}

/** function read(const string key, set<T> &list, const set<T> init)
 * Reads in a vector ( = list ) from the ini file.
 * In the ini file a list isdeclared as
 * \verbatim
 * {long identifier}
 * value 1
 * value 2
 *
 * # the empty line at the end is important
 * \endverbatim
 *
 * Given in the commandline a vector can be seperated by commas or whitespaces.
 * It also can be embraced in [].
 *
 * \attention non option arguments cannot follow a vector.
 *
 * \param key : identifier of the list
 * \param list: the vector which is filled with results
 * \param init: a vector used to initialize, when the vector was not found
 * 				in the ini file or on the command line
 * \return true if values where read, false otherwise
 */
template <class T>
bool myIniFiles::read(const string key, set<T> &list, const set<T> init){
	// search in ini file
	bool res = false;

	for (size_t i = 0; i < params.size(); i++){
		mylibs::cmdline::parameter &p = params[i];
		if (key == p.key and get_active_section() == p.section) {
			p.paramtype = mylibs::cmdline::parameter::list;
			break;
		}
	}

	string vkey = '{' + key + '}';
	mystring buf;
	list.clear();
	text.seekg(ios::beg);
	//! search line with list identifier '{identifier}'
	while (getline(text, buf)){
		if ( buf.strip().startswith(vkey) ) break;
	}

	int result = text.tellg();
	if (result != -1){ // if not at the end of the stream
		while (getline(text, buf)){
			buf.strip();
			if (! buf[0]) break; // while line not empty, add to the list
			if ( not buf.startswith(comment_char) ) {
				T tmp;
				buf >> tmp;
				list.insert(tmp);
			}
		}
		res = true;
	}

	text.seekg(ios::beg);
	text.clear();
	if (debug) cout << vkey << " - read " << list.size() << " items " << endl;

	// set standard values
	if (list.size() == 0 ) {
		if (debug)
			cout << " (!) using standard values : " << init.size() << " items " << endl;
		list = init;
	}

	// override by commandline
	bool rescmd = this->param_getarg(list, key);
	if (debug and rescmd)
		cout << " (!) overidden by commandline : " << list.size() << " items " << endl;

	return res;
}

/** \fn template <class T> bool myIniFiles::param_getarg(vector<T> &, const string) const
 * \brief Get the arguments of a parameter.
 *
 * \param vec: Vector of class T, where the values should be stored.
 * \param key: The name of the parameter for which to get the arguments.
 */
template <class T>
bool myIniFiles::param_getarg(vector<T> &vec, const string key) const {
	mystring result;
	bool success = param_getarg(result, key);
	if (not success) return false;

	//strip braces
	size_t lb = result.find('[');
	size_t rb = result.rfind(']');
	if (lb != string::npos and rb != string::npos) result = result(lb+1,rb);

	myStringList list = result.split(" ,:");
	vec.clear(); // empty the list, to override standard settings
	for (size_t i = 0; i < list.size(); i++){
		T tmp;
		try{
			list[i] >> tmp;; // override ini-file
			vec.push_back(tmp);
		} catch (mystring_exception& e){
			string msg = "\nConversion failure. The vector you provided was '" + result
				+ "'. If there are too many arguments in the list, try to enclose "
				+ "your vector data in [], eg. -v [2 3 4].";
			throw myIniFiles::Exception_ARG(EXCEPTION_ID + msg);
		}
	}
	return true;
}

/** \fn template <class T> bool myIniFiles::param_getarg(set<T> &, const string) const
 * \brief Get the arguments of a parameter.
 *
 * \param Set: Set of class T, where the values should be stored.
 * \param key: The name of the parameter for which to get the arguments.
 */
template <class T>
bool myIniFiles::param_getarg(set<T> &Set, const string key) const {
	mystring result;
	bool success = param_getarg(result, key);
	if (not success) return false;

	//strip braces
	size_t lb = result.find('[');
	size_t rb = result.rfind(']');
	if (lb != string::npos and rb != string::npos) result = result(lb+1,rb);

	myStringList list = result.split(" ,:");
	Set.clear(); // empty the list, to override standard settings
	for (size_t i = 0; i < list.size(); i++){
		T tmp;
		try{
			list[i] >> tmp;; // override ini-file
			Set.insert(tmp);
		} catch (mystring_exception& e){
			string msg = "\nConversion failure. The vector you provided was '" + result
				+ "'. If there are too many arguments in the list, try to enclose "
				+ "your vector data in [], eg. -v [2 3 4].";
			throw myIniFiles::Exception_ARG(EXCEPTION_ID + msg);
		}
	}
	return true;
}

template <class T>
bool myIniFiles::register_param( const string key, const char * const opt, const string desc,
								 const T defaultvalue,
								 const vector<T> allowed_values,
								 const char * const dependency
								){

	mystring default_string = mystring("");
	default_string << defaultvalue ;
	if (typeid(defaultvalue) == typeid(0.5)) default_string += " ( float )";
	else
	if (typeid(defaultvalue) == typeid(0))   default_string += " ( int )";
	else
	if (typeid(defaultvalue) == typeid(true))default_string += " ( boolean )";
	else
	if ((typeid(T) == typeid(string("Text")))
		or	(typeid(T) == typeid(const char*))
		or	(typeid(T) == typeid("Text"))
		or	(typeid(T) == typeid(mystring("Text")))	){
		default_string += " ( string )";
	}
	else default_string += " (" + string(typeid(defaultvalue).name()) + " )";

	//! \todo Add list of allowed values
	if ( not get_active_section().empty() )
		return this->register_param_sec(get_active_section(), key, opt, desc,
										(default_string != "None"), default_string,
										string("None"), string("None"), dependency);
	else
		return this->register_param_sec(string(), key, opt, desc,
										(default_string != "None"), default_string,
										string("None"),string("None"), dependency);
}

template <class T>
bool myIniFiles::register_param(const string key, const char * const opt, const string desc,
							const T defaultvalue, const T minimum,
							const T maximum, const char * const dependency){

	mystring default_string = mystring("");
	default_string << defaultvalue ;
	if (typeid(defaultvalue) == typeid(0.5)) default_string += " ( float )";
	else
	if (typeid(defaultvalue) == typeid(0))   default_string += " ( int )";
	else
	if (typeid(defaultvalue) == typeid(true))default_string += " ( boolean )";
	else
	if ((typeid(T) == typeid(string("Text")))
		or	(typeid(T) == typeid(const char*))
		or	(typeid(T) == typeid("Text"))
		or	(typeid(T) == typeid(mystring("Text")))	){
		default_string += " ( string )";
	}
	else default_string += " (" + string(typeid(defaultvalue).name()) + " )";

	mystring mini_string = mystring("");
	mini_string << minimum;
	mystring maxi_string = mystring("");
	maxi_string << maximum;
	if ( not get_active_section().empty() )
		return this->register_param_sec(get_active_section(), key, opt, desc,
										(default_string != "None"), default_string,
										mini_string, maxi_string, dependency);
	else
		return this->register_param_sec(string(), key, opt, desc,
										(default_string != "None"), default_string,
										mini_string, maxi_string, dependency);
}



#undef varname
#undef vn
#undef _LINE_
#endif

