=Introduction=

This is a collection of my personal libraries. All the classes may be completely different in meaning. Changes made in this library must be made very carefully since I'm using a lot of functions in several projects.

=Documentation=

The documentation can be created using doxygen
{{{
 $ doxygen Doxyfile
}}}

or simply using
{{{
 $ make doc
}}}

=Compilation=

All *.cpp files will be compiled into object code by the makefile. They will be put all together into a library lib/libmylib.a.
{{{
  make OPT=-O3
 }}}

In order to add debug code to the library replace -O3 by -g.

=Installation=

For simplicity you can install the libraries into a folder of your choice,
e.g to install the lib to $HOME/local/stow/mylibs run

If your preferred directory is called different you'll have to replace
MYSTOW=$HOME/local/stow by the path of your choice.
The installation procedure needs a working installation of 'stow'. 
{{{
  $ export MYSTOW=$HOME/local/stow
  $ make installall
}}}

The following step is done by th makefile
You should also get used to the neat tool 'stow', which cares in a very
simple manner for installed programs and libraries.

With
{{{
  $ cd $MYSTOW && stow mylibs
}}}

all the stuff is linked directly into the $MYLOCAL tree, whose subfolders should be included into your standard search paths.

If you are using bash, then the environment variable PATH has to be edited.

Therefor in the file $HOME/.bashrc add the line :
{{{
	export PATH=$HOME/local/bin:$PATH
}}}

=Usage=

For every class (e.g. mystring) you'd like to utilize in your project add the
appropriate *.hpp file to your header.

{{{
 #include <mylibs/mystring.hpp>
}}}

Programs can include this library using
{{{
g++ [your stuff] -I[path/to/include/] -L[path/to/libmylib.a] -lmylib
}}}
or even simpler, if you installed the library as explained above
{{{
 g++ [your stuff] `mylibs-config --cldflags`
}}}