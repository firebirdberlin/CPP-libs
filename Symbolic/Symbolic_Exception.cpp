//		./Symbolic/Symbolic_Exception.cpp
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
#ifndef SYMBOLIC_EXCEPTION_CPP
#define SYMBOLIC_EXCEPTION_CPP

/** \file Symbolic/Symbolic_Exception.cpp
 * \brief Defines methods of the exception classes.
 *
 *	This file defines the methods of the exception-classes for Lexerm Bytecode
 *	and Parser.
 */

#include "Symbolic_Exception.hpp"

namespace Symbolic {

using namespace std;

// #############################################################################
// ---------------- Definition of LexerException members -----------------------
// #############################################################################

/** \fn LexerException::LexerException(std::string const &, Lexer_exceptions const)
 *	Constructor, takes a string and an id.
 * \param str: String that describes the errror and provide additional information.
 * \param id: Error-id.
 */
LexerException::LexerException(std::string const & str, Lexer_exceptions const id) {
	this->ex_id = id;
	this->s =str;
	SetString(id);
}

/** \fn LexerException::LexerException(Lexer_exceptions const)
 *	Constructor, takes an id.
 * \param id: Error-id.
 */
LexerException::LexerException(Lexer_exceptions const id) {
	this->ex_id = id;
	this->s = string();
	SetString(id);
}

/** \fn void LexerException::SetString(Lexer_exceptions const)
 *	Switches the given id and sets depending on that an error message.
 * \param id: A Lexer_expetions enum type, containing information, what kind of error occoured.
 * \return void
 */
void LexerException::SetString(Lexer_exceptions const id) {
	switch (id) {
		case UNKNOWN_OPERATOR_OR_SYMBOL:
			this->s += "Lexer-error: unknwon operator or symbol.\n";
			break;
		case UNKNOWN_IDENTIFIER:
			this->s += "Lexer-error: unknown identifier.\n";
			break;
		default:
			this->s += "Lexer-error: unclarified exception.\n";
			break;
	}
}

// #############################################################################
// ---------------- Definition of LexerException members -----------------------
// #############################################################################

/** \fn NodeException::NodeException(std::string const &, type const)
 *	Constructor, takes a string and an id.
 * \param str: String that describes the errror and provide additional information.
 * \param id: Error-id.
 */
NodeException::NodeException(std::string const & str, type const id)
	:	ex_id(id), s(str) {
	SetString();
}

/** \fn NodeException::NodeException(type const)
 *	Constructor, takes an id.
 * \param id: Error-id.
 */
NodeException::NodeException(type const id)
	: ex_id(id), s(string()) {
	SetString();
}

/** \fn void NodeException::SetString()
 *	Switches the given id and sets depending on that an error message.
 */
void NodeException::SetString() {
	switch (ex_id) {
		case CANNOT_HANDLE_THIS_OPCODE:
			this->s += "Node-error: opcode can't be handled here.\n";
			break;
		case CANNOT_SOLVE_THIS_CONFIGURATION:
			this->s += "Node-error: configuration can't be solved with this method.\n";
			break;
		case OPCODE_NOT_FOUND:
			this->s += "Node-error: specified opcode not found.\n";
		default:
			this->s += "Node-error: unclarified exception.\n";
			break;
	}
}

// #############################################################################
// --------------- Definition of BytecodeException members ---------------------
// #############################################################################

/** \fn BytecodeException::BytecodeException(std::string const &, Bytecode_exceptions const)
 *	Constructor, takes a string and an id.
 * \param str: String that describes the errror and provide additional information.
 * \param id: Error-id.
 */
BytecodeException::BytecodeException(std::string const & str, Bytecode_exceptions const id) {
	this->ex_id = id;
	this->s =str;
	SetString(id);
}

/** \fn BytecodeException::BytecodeException(Bytecode_exceptions const)
 *	Constructor, takes an id.
 * \param id: Error-id.
 */
BytecodeException::BytecodeException(Bytecode_exceptions const id) {
	this->ex_id = id;
	this->s = string();
	SetString(id);
}

/** \fn void BytecodeException::SetString(Bytecode_exceptions const)
 *	Switches the given id and sets depending on that an error message.
 * \param id: A Bytecode_expetions enum type, containing information, what kind of error occoured.
 * \return void
 */
void BytecodeException::SetString(Bytecode_exceptions const id) {
	switch (id) {
		case OPCODE_BAD:
			this->s += "Bytecode-error: unknwon operator or symbol.\n";
			break;
		case OPCODE_UNKNOWN:
			this->s += "Bytecode-error: unknown identifier.\n";
			break;
		default:
			this->s += "Bytecode-error: unclarified exception.\n";
			break;
	}
}

// #############################################################################
// ---------------- Definition of ParserException members ----------------------
// #############################################################################

/** \fn ParserException::ParserException(std::string const &, Parser_exceptions const)
 *	Constructor, takes a string and an id.
 * \param str: String that describes the errror and provide additional information.
 * \param id: Error-id.
 */
ParserException::ParserException(std::string const & str, Parser_exceptions const id) {
	this->ex_id = id;
	this->s =str;
	SetString(id);
}

/** \fn ParserException::ParserException(Parser_exceptions const)
 *	Constructor, takes an id.
 * \param id: Error-id.
 */
ParserException::ParserException(Parser_exceptions const id) {
	this->ex_id = id;
	this->s = string();
	SetString(id);
}


/** \fn void ParserException::SetString(Parser_exceptions const)
 *	Switches the given id and sets depending on that an error message.
 * \param id: A Parser_expetions enum type, containing information, what kind of error occoured.
 * \return void
 */
void ParserException::SetString(Parser_exceptions const id) {
	switch (id) {
		case EXPECTED_EXPRESSION:
			this->s += "Parser-syntax-error: expect end of expression.\n";
			break;
		case EXPECTED_PARENTHESIS_AFTER_FUNCTIONNAME:
			this->s += "Parser-syntax-error: expect \'(\' after functionname.\n";
			break;
		case EXPECTED_PARENTHESIS_AFTER_FUNCTIONPARAM:
			this->s += "Parser-syntax-error: expect \')\' after functionparamter.\n";
			break;
		case EXPECTED_PARENTHESIS_AFTER_EXPRESSION:
			this->s += "Parser-syntax-error: expect \')\' after expresion.\n";
			break;
		case WRONG_SYMBOL:
			this->s += "Parser-syntax-error: unexpected symbol.\n Expected are variable, function, number, open paranthesis or minus.\n";
			break;
		default:
			this->s += "Parser-error: unclarified exception.\n";
			break;
	}
}

}// namespace Symbolic

#endif // SYMBOLIC_EXCEPTION_CPP
