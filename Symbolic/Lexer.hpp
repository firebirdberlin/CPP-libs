//		./Symbolic/Lexer.hpp
//
//		Copyright 2011 Fabian Paul <fab@itp.tu-berlin.de>
//		Copyright 2011 Rico Buchholz <buchholz@itp.tu-berlin.de>
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
#ifndef LEXER_HPP
#define LEXER_HPP

/** \file Symbolic/Lexer.hpp
 * \brief Declares Lexer-class
 *
 *	This file declares the Lexer-class.
 */

#include "Definitions.h"
#include "Symbolic_Exception.hpp"

namespace Symbolic {

using namespace std;

/** \class Lexer
 * \brief Class for parsing function strings.
 *
 * This class is used to check what part of a function a string contains.
 *	The name of the variables and functions that the string can contain is
 *	thereby restricted.<br>
 *	For a list of the available variable and functions names that can be used,
 *	see the documentation of the Parser class.<br>
 *	These list can't be changed at runtime. But you can provide a map with
 *	pairs of constants and theyr values, which are then used.
 */
class Lexer {
	public:
		Lexer(string const & ma, Parameter const * const param = NULL);
		Lexer(Lexer const & other);
		~Lexer();

		void Check() throw(LexerException);

		double					GetNumber()		const {return number;};
		Opcode					GetOpcode()		const {return opcode;};
		Symbol					GetSymbol()		const {return sym;};
		Parameter	const *		GetParameter()	const {return parameter;};
		string::const_iterator	GetLastSym()	const {return lastsym;};
		string		const &		GetExpression()	const {return math;}

	private:
		double number;//!< If the string contains a number, the value is stored in this member.
		Literal *literal;
		Parameter const * const parameter;//!< Where to find the parameters.
		string math;//!< The string that has to be parsed.
		string::iterator stream;//!< Iterators which is used to going through the string.
		string::iterator end;//!< For tests if the end of the string is reached.
		string::iterator lastsym;//!< The last symbol that was checked.
		Opcode opcode;
		Symbol sym;

		Symbol ScanIdentifier(string const ident) throw(LexerException);
};

}
#endif // LEXER_HPP
