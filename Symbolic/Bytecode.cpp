//		./Symbolic/Bytecode.cpp
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
#ifndef BYTECODE_CPP
#define BYTECODE_CPP

/** \file Symbolic/Bytecode.hpp
 * \brief Definiton of Bytecode-methods.
 *
 *	This file defines the methods of the Bytecode-class.
 *	Thereby the definiton of one method is dependent on a define-constant.
 */

#include <cmath>
#include <cstdio>

#ifdef DEBUG
#include <iostream>
#endif

#include "Bytecode.hpp"

namespace Symbolic {

using namespace std;

/** name: Bytecode::Bytecode
 *	Standardconstructor, initializes the pointers with NULL and the other
 *	members with zero.
 */
Bytecode::Bytecode()
	: stack(NULL), code(NULL), stacksize(0), codesize(0) {
	// empty
}

/** name: Bytecode::Bytecode
 *	Constructor, takes a Node, which is assumed to be the top of a whole tree.
 *	This is used to initialize the members of this class.
 *	The stack is created as array with size equal to the depth of the tree plus
 *	one and the code as array with size equal to the length of the tree plus one.
 *	These sizes are stored in the corresponding members.
 *
 *	The code is then created via the Postorder-method of the Node.
 *
 * \param tree: The root of the tree, from which to create the code.
 */
Bytecode::Bytecode(Node const & tree)
	: stack(NULL), code(NULL), stacksize(tree.Depth() + 1), codesize(tree.SerializedLength() + 1) {
	Node temp(tree);
	temp.ReduceTree();
	stacksize = temp.Depth() + 1;
	codesize = temp.SerializedLength() + 1;
	stack = new double[stacksize];
	code  = new Cell[codesize];
	temp.Postorder(code);
}

/** name: Bytecode::Bytecode
 * \brief Copyconstructor
 *
 *	This is the copyconstructor of the bytecode-class. The size-members are
 *	directly initialized by the corresponding member of the other object.
 *	Then the arrays are allocated and the values of the other objects are copied
 *	item per item.
 *
 * \param other: Reference to an already existing Bytecode-object which should
 *				be copied.
 */
Bytecode::Bytecode(Bytecode const & other)
	: stack(NULL), code(NULL), stacksize(other.GetStacksize()), codesize(other.GetCodesize()) {
	stack = new double[stacksize];
	for(size_t i = 0; i < stacksize; ++i) {
		*(stack + i) = *(other.GetStack() + i);
	}

	code  = new Cell[codesize];
	for(size_t i = 0; i < codesize; ++i) {
		*(code + i) = *(other.GetCode() + i);
	}
}

/** name: Bytecode::Bytecode
 *	Destructor, frees the space that was (maybe) reserved and sets the pointers
 *	to NULL.
 */
Bytecode::~Bytecode() {
	delete[] stack;
	stack = NULL;
	delete[] code;
	code = NULL;
}

/** name: Bytecode::CreateCode
 *	\brief Generates the code for a function.
 *
 *	This method generates the bytecode for a function from a tree of Nodes.
 *	Therefore the Postorder-method of the Node(-tree) is used.
 *
 *	If there are already arrays, then those are deleted, before the new ones are
 *	created.
 *
 *	The members for the size of the code and the stack are also adjusted.
 *
 * \param tree: The root-Node of the function-tree.
 * \return void
 */
void Bytecode::CreateCode(Node const tree) {
	if(NULL != stack) {delete[] stack; stack = NULL; stacksize = 0;}
	Node temp(tree);
	temp.ReduceTree();
	stacksize = temp.Depth() + 1;
	stack = new double[stacksize];

	for(size_t i = 0; i < stacksize; ++i) {
		stack[i] = 0.0;
	}

	if(NULL != code) {delete[] code; code = NULL; codesize = 0;}
	codesize = temp.SerializedLength() + 1;
	code  = new Cell[codesize];
	temp.Postorder(code);
}

/** name: Bytecode::Compute
 *	\brief Evaluates the function.
 *
 *	This method evaluates the function for a given set of variables.<br>
 *	This is done by running through the code array till an Opcode OP_END is
 *	reached.
 *	For every item, is the corresponding action done by using a switch-operation.
 *
 * \note One should take into account, to make this method inline, because this
 *				make the whole computation, buts its size is maybe a drawback.
 *
 * \param x:
 * \param y:
 * \param z:
 * \param t: The values for those the function should be evaluated.
 * \return The function value as double.
 */
double Bytecode::Compute(double const x, double const y, double const z, double const t) const throw(BytecodeException) {
	//! Local pointers for the operation and the value stack.
	Cell *ip = code;
	double *sp = stack;

	//! Boolean for the loop, is set to false when the Opcode OP_END is found.
	bool run = true;

	while(run) {
		switch(ip->opcode) {
			case OP_PLUS:			*(sp-1) += *sp;					sp--;	break;
			case OP_MINUS:			*(sp-1) -= *sp;					sp--;	break;
			case OP_TIMES:			*(sp-1) *= *sp;					sp--;	break;
			case OP_DIVIDES:		*(sp-1) /= *sp;					sp--;	break;
			case OP_MODULO:			*(sp-1)  = fmod(*(sp- 1), *sp);	sp--;	break;
			case OP_EXPONENTATION:	*(sp-1)  = pow(*(sp - 1), *sp);	sp--;	break;

			case OP_CONST:			sp++;	*sp = (ip + 1)->number;	ip++;	break;
			case OP_X:				sp++;	*sp = x;						break;
			case OP_Y:				sp++;	*sp = y;						break;
			case OP_Z:				sp++;	*sp = z;						break;
			case OP_T:				sp++;	*sp = t;						break;

			case OP_EXP:			*sp = exp(*sp);							break;
			case OP_LN:				*sp = log(*sp);							break;
			case OP_SQRT:			*sp = sqrt(*sp);						break;
			case OP_SIN:			*sp = sin(*sp);							break;
			case OP_COS:			*sp = cos(*sp);							break;
			case OP_TAN:			*sp = tan(*sp);							break;
			case OP_SINH:			*sp = sinh(*sp);						break;
			case OP_COSH:			*sp = cosh(*sp);						break;
			case OP_TANH:			*sp = tanh(*sp);						break;
			case OP_ASIN:			*sp = asin(*sp);						break;
			case OP_ACOS:			*sp = acos(*sp);						break;
			case OP_ATAN:			*sp = atan(*sp);						break;
			case OP_ASINH:			*sp = asinh(*sp);						break;
			case OP_ACOSH:			*sp = acosh(*sp);						break;
			case OP_ATANH:			*sp = atanh(*sp);						break;
			case OP_HEAVISIDE:		*sp = ((*sp > 0) ? 1 : 0);				break;

			case OP_END:			run = false;							break;

			case OP_BAD:			throw BytecodeException(OPCODE_BAD);	break;
			default: {
				char buffer [50];
				sprintf (buffer, "%d\0", ip->opcode);// add stackvalue and x,y,z,t
				throw BytecodeException(buffer, OPCODE_UNKNOWN);
			}
			break;
		}
		ip++;
	}

	return *sp;
}

/** name: Symbolic::Bytecode::Exists
 * Checks if an operator is found in the expression.
 * @param op: The operator to be checked.
 * @return True if the operator was found in the tree, false otherwise.
 ***/
bool Bytecode::Exists(Opcode const op) const throw(BytecodeException) {
	//! Local pointers for the operation and the value stack.
	Cell *ip = code;

	//! Boolean for the loop, is set to false when the Opcode OP_END is found.
	bool run = true;
	while(run) {
		if (ip->opcode == op) return true; // if the operator is found return true

		if(OP_END == ip->opcode) {
			run = false;
		} else if(OP_BAD == ip->opcode) {
			throw BytecodeException(OPCODE_BAD);
			run = false;
		}
		ip++;
	}

	return false;
}

#ifdef DEBUG
/** name: Bytecode::Info
 * \brief Prints the code-array.
 *
 *	These method prints out the operation-code-numbers of the code-array and the
 *	values of the stack-array.
 *
 * \note This method will only be defined if DEBUG is defined.
 */
void Bytecode::Info() const {
	size_t i = 0;

	std::cout<<"code:\n";
	while(i < codesize) {
		std::cout<<i<<" "<<(code + i)->opcode<<"\n";
		if(OP_CONST == (code + i)->opcode) {++i; std::cout<<i<<"  "<<(code + i)->number<<"\n";}
		++i;
	}

	std::cout<<"stack:\n";
	while(i < stacksize) {
		std::cout<<i<<" "<<*(stack + i)<<"\n";
		++i;
	}
}
#endif

}

#endif // BYTECODE_CPP
