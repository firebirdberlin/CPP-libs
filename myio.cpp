// myio.cpp
//
// Copyright 2012 Stefan Fruhner <stefan.fruhner@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA 02110-1301, USA.


#include "myio.hpp"

namespace mylibs{
namespace IO{

// recursive mkdir
bool mkdir(mystring dir, mode_t mode){

//	cmdline::msg("mkdir("+dir+", 0740)");

	if (not dir.empty()){
		vector<mystring> splits = dir.splitv("/");
		mystring current;
		list<mystring> l(splits.begin(), splits.end());

		// preserve leading slash
		if (dir.startswith("/")) l.front() = "/"+l.front();
		while (l.size() > 0){
			current = current.path_join(l.front());
			if(::mkdir(current.c_str(), mode) == -1){
				int errsv = errno;
				if (errno == EEXIST) {
					l.pop_front();
					continue;
				}
				//! The directory couldn't be created, so give a error message to the user.
				//errno == EACCES	Zugriff nicht gestattet
				//errno == ENOENT	Datei oder Verzeichnis nicht gefunden
				cmdline::exit("Error creating " + dir + " (" + strerror(errsv) + ").");
				return false;
			}
			l.pop_front();
		}
	}
	return true;
}

// remove files
bool rm(const char *filename){

	if (not file_exists(filename)) return false;
	int result = remove(filename);
	if (result == -1) {
		int errsv = errno;
		cmdline::warning("Error removing " + string(filename) + " (" + strerror(errsv) + ").");
		return false;
	}
	return true;
}

//copy files
int cp(const char *from, const char *to){

	std::cout << "cp " << from << " " << to << std::endl;

    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0)
        return -1;

    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return 0;
    }

  out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
    return -1;
}

int cp(const mystring &from, const mystring& to){
	return cp(from.c_str(), to.c_str());
}

/**
 * name: mylibs::IO::file_size
 * @param filename: name of the file
 * @return size in bytes
 **/
size_t file_size(const char *filename){
	ifstream file( filename, ios::binary | ios::ate);
	size_t sz = file.tellg();
	file.close();
	return sz;
}

// check if file exists
//
bool file_exists(const char *filename, int mode){
	if ( mode < 0 || mode > 7){
		throw myexception(EXCEPTION_ID + "mode has to be an integer between 0 and 7");
		return false;
	}

    // int _access(const char *path, int mode);
    //  returns 0 if the file has the given mode,
    //  it returns -1 if the named file does not exist or is not accessible in
    //  the given mode
    // mode = 0 (F_OK) (default): checks file for existence only
    // mode = 1 (X_OK): execution permission
    // mode = 2 (W_OK): write permission
    // mode = 4 (R_OK): read permission
    // mode = 6       : read and write permission
    // mode = 7       : read, write and execution permission
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
	if (_access(filename, mode) == 0)
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
	if (access(filename, mode) == 0)
#endif
	{
		return true;
	}
	else{
		return false;
	}

}


bool is_executable(const char *cmd){
	char *path = getenv("PATH");
    if (!path) return false;

	mystring p(path);
    vector<mystring> l;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
	l = p.splitv(';');
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    l = p.splitv(':');
#endif

	for (vector<mystring>::iterator it = l.begin(); it != l.end(); ++it){
		mystring tmp = it->path_join(mystring(cmd));
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
			if ( IO::file_exists(tmp.c_str(),0) ) // check existence
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
			if ( IO::file_exists(tmp.c_str(),1) ) // check existence and execution permission
#endif
			{
				return true;
			}
	}

	return false;
}

/**
 * name: get_config_dir(const char *)
 *
 * Returns the path of a valid configuration directory.
 *
 * The path is chosen likely to linux system as $HOME/.config/<program>
 *
 * The directory is created if needed.
 *
 * @param name_of_program: 	Defines the name of the calling program,
 * 							which is used as subfolder within the
 * 							config directory.
 * @return 	string which is empty on error or otherwise contains the
 * 			directory.
 *
 */
const string get_config_dir(const char *name_of_program){
	char* HOME = getenv("HOME");
	mystring config_dir(HOME);
	config_dir = config_dir.path_join(".config");
	config_dir = config_dir.path_join(name_of_program);
	if (not file_exists(config_dir.c_str(), 7)){
		cout << "mkdir " << config_dir << endl;
		mylibs::IO::mkdir(config_dir.c_str());
	}

	return config_dir;
}
}
}
