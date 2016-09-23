//		./cmdline.cpp
//
//		Copyright 2012 Stefan Fruhner <stefan.fruhner@gmail.com>
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

#include "cmdline.hpp"

using namespace std;
namespace mylibs{
namespace cmdline {

/** \fn int columns()
 * \brief Checks the width of the bash where the output is written.
 *
 * \return The number of colums, which the bash for the output has.
 */
int columns(){
	int cols = 80;
	FILE *pipe = popen("tput cols", "r");// Open a pipe for determining the width.
	int numread = fscanf(pipe, "%d", &cols);
	if (numread != 1) cols = 80;
    pclose(pipe);
//	printf("Columns = %d\n", cols);
	return cols;
}

/** \fn void cmdline::newline.
 * \brief Prints a newline character.
 */
void newline(){
	putchar('\n');
}

/** \fn void cmdline::section(const char *)
 *
 * Prints out a headline of a new section on stdout. It will be centered
 * an enclosed in '='. This makes console output more easy to read.
 * The width is adjusted to  the number of columns of the terminal.
 * The line with the heading is enclosed by lines with '-' signs.
 *
 * \note There also exists a version that expects a std::string.
 *
 * \param heading: A c-string, the headline to be written.
 */
void section(const char *heading){

	if (heading == NULL) return line('=');

	putchar('\n');
	int i = 0;
	int len = strlen(heading);
	const int w = cmdline::columns();
	for (i = 0; i < w ; i++) putchar('-'); putchar('\n');

	// Test if the heading fits into the line, if this is not the case,
	// we print what fits plus three dots (...).
	if ((len < w-2) && (len> 0)) {
		len = w-2 - strlen(heading);
		for (i = 0; i <= len/2 ; i++) putchar('=');
		printf(" %s ",heading);
		for (i = w - len/2; i < w-1 ; i++) putchar('=');
		if (len % 2 != 0) putchar('=');
		putchar('\n');
	}
	else
	if (len >= w-2) {
		for (i = 0; i < w-3 ; i++) putchar(heading[i]);
		for (i = 0; i < 3 ; i++) putchar('.');
		putchar('\n');
	}
	for (i = 0; i < w ; i++) putchar('-'); putchar('\n');
	printf("\n");
}

/** \fn void cmdline::section(const string)
 * \brief Does the same as cmdline::section(const char*)
 *
 * \param heading: A string, the headline to be written.
 */
void section(const string heading){
	section(heading.c_str());
}

/** \fn void cmdline::subsection(const char *)
 *
 * Prints out a subheadline of a subsection on stdout. It will be centered
 * an enclosed in '='. This makes console output more easy to read.
 * The width is adjusted to the number of columns of the terminal.
 *
 * \note There exist also a version that takes a std::string.
 *
 * \param heading: A c-string, the headline to be written.
 */
void subsection(const char *heading){
	if (heading == NULL) return line('-');
	putchar('\n');
	int i = 0;
	int len = strlen(heading);
	const int w = cmdline::columns();
	if ((len < w-2) && (len> 0)) {
		len = w-2 - strlen(heading);
		for (i = 0; i <= len/2 ; i++) putchar('-');
		printf(" %s ",heading);
		for (i = w - len/2; i < w-1; i++) putchar('-');
		if (len % 2 != 0) putchar('-');
		putchar('\n');
	}
	else
	if (len >= w-2) {
		for (i = 0; i < w-3 ; i++) putchar(heading[i]);
		for (i = 0; i < 3 ; i++) putchar('.');
		putchar('\n');
	}
}

/** \fn void cmdline::subsection(const string)
 * \brief Does the same as cmdline::subsection(const char*)
 *
 * \param heading: A string, the headline to be written.
 */
void subsection(const string heading){
	subsection(heading.c_str());
}

/** \fn void line()
 * \brief Prints a line with '-' (plus newline).
 */
void line(const char ch){
	const int spaces = cmdline::columns();
	for (int i = 0; i < spaces ; i++) putchar(ch);
	newline();
}

/** \fn void ok(const int)
 *
 * Prints a green '[OK]' to stdout. Can be used to indicate that something's
 * successful.
 *
 *\param spaces: The number of spaces that have to be printed before
 *				[OK]. If the value is negative, the number of columns-4
 *				will be used.<br>
 *				Default value is -1.
 * @return Always true (can be used as a single line, e.g. return ok();)
 */
bool ok(int spaces){
	if (spaces < 0) {
		spaces = cmdline::columns();
		printf("\r\033[%dC[\033[1;32mOK\033[0m]\n", spaces-4);
	}
	else
	printf("\033[%dC[\033[1;32mOK\033[0m] ", spaces);
//	int i = 0;
//	for (i = 0; i < spaces-4; i++) putchar(' ');

//	printf("[\033[1;32mOK\033[0m] ");
	return true;
}

/** \fn void cmdline::no(const int)
 *	Prints out a warning on the command line, which is a red [!!].
 * @param spaces : The number of spaces that have to be printed before
 *				[!!]. If the value is negative, the number of columns-4
 *				will be used.<br>
 *				Default value is -1.
 * @return Always false (can be used as a single line, e.g. return no();)
 **/
bool no(int spaces){
	if (spaces < 0){
		spaces = cmdline::columns();
		fprintf(stderr,"\r\033[%dC[\033[1;31m!!\033[0m]\n", spaces-4);
	} else
	fprintf(stderr,"\033[%dC[\033[1;31m!!\033[0m] ", spaces);
//	int i = 0;
//	for (i = 0; i < spaces-4; i++)putchar(' ');

//	fprintf(stderr,"[\033[1;31m!!\033[0m] ");
	return false;
}

/** \fn cmdline::msg(const char *, ostream &)
 *
 *	Prints out a message on the command line.
 *
 * \note There also exists a version that expects a std::string.
 *
 * @param msg	 : C-string, text of the message.
 * @param s		 : Stream, where to put the data.<br>
 *					Default is std::cout.
 **/
void msg(const char *msg, ostream &s){
	vector<size_t> spaces;
	for (size_t i = 0; i < strlen(msg); i++)
		if (msg[i] == ' ') spaces.push_back(i);

	const int w = cmdline::columns();
	queue<size_t> ins;
	size_t start = 0;
	if (spaces.size() > 0 )
	for (size_t i = 0; i < spaces.size()-1; i++){
		size_t pos1 = spaces[i];
		size_t pos2 = spaces[i+1];
		if (pos1 <= start + w && pos2 > start + w){
			start = pos1;
			ins.push(pos1);
		}
	}

	for (size_t i = 0; i < strlen(msg); i++){
		s << msg[i];
		if (ins.size() > 0 && i == ins.front()){
			cerr << endl;
			ins.pop();
		}
	}
	s << endl;
}

/** \fn void msg(const string, ostream &)
 * \brief Does the same as msg(const char *, ostream &)
 *
 * @param mesg	 : std::string, text of the message.
 * @param s		 : Stream, where to put the data.<br>
 *				Default is std::cout.
 */
void msg(const string mesg, ostream &s){
	msg(mesg.c_str(), s);
}

/** \fn void emsg(const char *)
 * \brief Prints no followed by an error message.
 *
 *	This method will first print an error indicator and then an error message.
 *	The latter is done by using msg(mesg, std::cerr).
 *
 * \note There exist also a version that takes a std::string.
 *
 * \param mesg: A c-string, the error message that should be printed.
 */
void emsg(const char *mesg){
	no(0);
	msg(mesg, std::cerr);
}

/** \fn void emsg(const string)
 * \brief Does the same as cmdline::emsg(const char*).
 *
 * \param mesg: A std::string, the error message that should be printed.
 */
void emsg(const string mesg){
	emsg(mesg.c_str());
}


/** \fn void warning(const char *)
 *	Prints out a warning message on the command line, which is a red [!!]
 * followed by the message.
 *
 * \note There exist also a version that takes a std::string.
 *
 * @param mesg : text of the message.
 **/
void warning(const char *mesg){
	newline(); no(0);
	msg(mesg, std::cerr);
}

/** \fn void warning(const string)
 * \brief Does the same as cmdline::warning(const char*).
 *
 * @param mesg	 : text of the message.
 **/
void warning(const string mesg){
	newline();
	warning(mesg.c_str());
}


/** \fn void cmdline::okmsg(const char *)
 *	Prints out a message on the command line with leading [ok]
 * @param mesg   : text of the message.
 **/
void okmsg(const char *mesg){
	ok(0);
	msg(mesg);
}

/** \fn void okmsg(const string mesg)
 * \brief Does the same as cmdline::okmsg(const char*).
 *
 * @param mesg	 : text of the message.
 **/
void okmsg(const string mesg){
	okmsg(mesg.c_str());
}

/** \fn void exit(const char*).
 * \brief Prints an error message and quits the program.
 *
 * \note There exist also a version that takes a std::string.
 *
 * \param message: A c-string, that contains the message.
 */
void exit(const char* message){
	emsg(message);
	std::exit(-1);
}

/** \fn void exit(const string)
 * \brief Does the same as cmdline::exit(const char *).
 *
 * \param message: A std::string, that contains the message.
 */
void exit(const string message){
	exit(message.c_str());
}

/** \fn void run_command(const char *, const char *)
 * \brief Run a command in another directory.
 *
 *	This method will execute a command via system(..), after changing the
 *	directory.
 *	If the command was executed, the directory is changed to the previous one.<br>
 *	The error function is used, if a command fails (which will exit the program).
 *
 * \param cmd		 : 	A c-string, that contains the command to execute.
 * \param working_dir: 	A c-string, that contains the directory where to
 *						execute the command (default : NULL).
 */
void run_command(const char *cmd, const char *working_dir){
	msg(" - Running command : `" +string(cmd) + "`");
	char wd[1024] = "";
	if (!getcwd(wd,1024)){
		int errsv = errno;
		cmdline::no(0);
		error(-1, errsv, "Could not get current working directory (error = %s)",
			  strerror(errsv));
	}

	if (working_dir and (strlen(working_dir) > 0) and (chdir(working_dir) != 0)){
		int errsv = errno;
		cmdline::no(0);
		error(-1, errsv, "Could not change directory from %s to %s. (error = %s )",
				wd, working_dir, strerror(errsv));
	}

	int result = system(cmd);
	if (result != 0){
		cmdline::no(0);
		error(result, 0, "Exit code was %d ",result);
	}
	if (working_dir and (strlen(working_dir) > 0) and (chdir(wd) != 0)){
		int errsv = errno;
		cmdline::no(0);
		error(-1, errsv, "Could not change back into the working directory (error = %s)",
			strerror(errsv));
	}
	cmdline::ok();
}

/** \fn void run_command(const string)
 * \brief Does the same as cmdline::run_command(const char*)
 * \param cmd: A std::string, that contains the command to execute.
 */
void run_command(const string cmd){
	run_command(cmd.c_str());
}

}
}
