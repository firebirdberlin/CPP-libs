//		./Symbolic/Lexer.cpp
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
#ifndef LEXER_CPP
#define LEXER_CPP

/** \file Lexer.cpp
 * \brief Defines methods of Lexer-class
 *
 *	This file defines the methods of the Lexer-class. Also an enum is defined,
 *	which is used by the Check-method.
 */

#include <locale>
#include <sstream>

#include "Lexer.hpp"

namespace Symbolic {

/** \enum Symbolic::Status
 *	\brief Context of a char.
 *
 *	The values of this enum are used by the Check-method of the Lexer-class to
 *	indicate in which kind of context a char is.
 *	For example "123.5" would suggest the status NUMBER.
 */
enum Status { OUTSIDE, IDENTIFIER, NUMBER };

/** \fn Lexer::Lexer(string const &, Parameter const * const)
 *	Constructor which takes a string and a Pointer to a Parameter-object.
 *	Initializes all the members of this object and creates the literal.
 *
 * \param ma: Reference to a string, which contains a function.
 * \param param: Pointer to a Parameter-object, which is used to initialize the
 *				own Parameter-pointer.
 */
Lexer::Lexer(string const & ma, Parameter const * const param)
	:	number(0.0), literal(NULL), parameter(param), math(ma),
	    stream(this->math.begin()), end(this->math.end()), // lastsym()
	    opcode(OP_BAD), sym(SYM_BAD) {
	literal	= new Literal(raw_literals, raw_literals + 20);
}

/** \fn Lexer::Lexer(Lexer const &)
 * \brief Copyconstructor
 *
 * \param other: Reference to an already existing Lexer which should be copied.
 */
Lexer::Lexer(Lexer const & other)
	:	number(other.GetNumber()), literal(NULL), parameter(other.GetParameter()),
	    math(other.GetExpression()),
	    stream(this->math.begin()), end(this->math.end()), // lastsym()
	    opcode(other.GetOpcode()), sym(other.GetSymbol()) {
	literal	= new Literal(raw_literals, raw_literals + 20);
}

/** \fn Lexer::~Lexer()
 *	Destructor, deletes the created Literal.
 */
Lexer::~Lexer() {
	delete literal;
	literal = NULL;
}

/** \fn void Lexer::Check() throw(LexerException)
 *	Checks the string, which is a member of this object, for a number, an
 *	identifier or a symbol. Depending on what is found, the members sym and
 *	opcode are set.<br>
 */
void Lexer::Check() throw(LexerException) {
	Status status= OUTSIDE;// Defines the context in which the next character is.
	string buffer;
	locale loc;

	lastsym = stream;
	sym = SYM_BAD;

	// Going through all the characters of the string.
	while(1) {
		switch(status) {

			case OUTSIDE:
				if(stream == end)		 { sym = SYM_END;		return;}
				if(isspace(*stream,loc)) { stream++; 			break; }
				if(isalpha(*stream,loc)) { status = IDENTIFIER;	break; }
				if(isdigit(*stream,loc)) { status = NUMBER;		break; }

				switch(*stream) {// Check which character is it.
					case '+': stream++; sym = SYM_PLUS;				opcode=OP_PLUS;				return;
					case '-': stream++; sym = SYM_MINUS;			opcode=OP_MINUS;			return;
					case '*': stream++; sym = SYM_TIMES;			opcode=OP_TIMES;			return;
					case '/': stream++; sym = SYM_DIVIDES;			opcode=OP_DIVIDES;			return;
					case '\\':
					case '%': stream++; sym = SYM_MODULO;			opcode=OP_MODULO;			return;
					case '^': stream++; sym = SYM_EXPONENTATION;	opcode=OP_EXPONENTATION;	return;
					case '[':
					case '{':
					case '(': stream++; sym = SYM_OPEN_PARENTHESIS;								return;
					case ']':
					case '}':
					case ')': stream++; sym = SYM_CLOSE_PARENTHESIS;							return;
					case ',':
					case ';': stream++; sym = SYM_SEMICOLON;									return;
					default:  throw LexerException(&(*lastsym), UNKNOWN_OPERATOR_OR_SYMBOL);
				}
				break;

			case IDENTIFIER:
				if(stream != end && isalpha(*stream, loc)) {
					buffer += *stream;
					stream++;
				} else {
					// If all the characters are read in, the identifier is complete, so it can be checked which one it is.
					status = OUTSIDE;
					sym = ScanIdentifier(buffer);
					buffer.clear();
					return;
				}
				break;

			case NUMBER:
				if(stream != end && (isdigit(*stream, loc) || *stream == '.')) {
					buffer += *stream;
					stream++;
				} else {
					// If all the characters are read in, the number is complet, so an istringstream is used to write the number into the member with the same name.
					status=OUTSIDE;
					std::istringstream istr(buffer, std::stringstream::in);
					istr >> number;
					buffer.clear();
					sym = SYM_NUMBER;
					opcode = OP_CONST;// added
					return;
				}
				break;
		}
	}
}

/** \fn Symbol Lexer::ScanIdentifier(string const) throw(LexerException)
 *	This method tries to identify a given string as an identifier which is known.
 *	This is done by first searching in the literals. If there is no matching
 *	identifier found the parameters are loooked through, if given.<br>
 *	In both cases is the corresponding Symbol returned and in the first case is
 *	additionally the opcode set, in the later case the number.<br>
 *
 * \param ident: The name which should be identified.
 * \return The Symbol that corresponds to the identifier.
 */
Symbol Lexer::ScanIdentifier(string const ident) throw(LexerException) {
	Literal::iterator match = literal->find(ident);

	if(match != literal->end()) {
		opcode = match->second.code;
		return match->second.sym;
	}

	if(parameter) {
		Parameter::const_iterator match = parameter->find(ident);

		if(match != parameter->end()) {
			number= match->second;
			return SYM_NUMBER;
		}
	}

	std::stringstream err;
	err<<__FILE__<<":"<<__FUNCTION__<<"(..):"<<__LINE__<<"\n";
	err<<"Identifier "<<ident<<" unknown (rest of the string to parse is "<<&(*lastsym)<<").\n";
	err<<"List of known literals:\n";
	for(Literal::const_iterator it = literal->begin(); it != literal->end(); ++it) {
		err<<"    "<<it->first<<" "<<it->second.sym<<" "<<it->second.code<<"\n";
	}
	if(NULL != parameter) {
		err<<"List of known parameters:\n";
		for(Parameter::const_iterator it = parameter->begin(); it != parameter->end(); ++it) {
			err<<"    "<<it->first<<" "<<it->second<<"\n";
		}
	}
	err<<"--------------------------\n";
	throw LexerException(err.str(), UNKNOWN_IDENTIFIER);
}

}

#endif // LEXER_CPP
